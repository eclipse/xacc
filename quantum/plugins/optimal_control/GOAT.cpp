#include "GOAT.hpp"
#include <iostream>
#include "LBFGS.h"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
#include "PauliOperator.hpp"

namespace {
constexpr int DEFAULT_NUMBER_STEPS = 10000;
const std::complex<double> I(0.0, 1.0);

using namespace xacc;
class RungeKutta
{
public:
    RungeKutta(std::function<Matrix(float, const Matrix&)> in_func): 
        m_func(in_func) 
    {}

    // Simple RK stepper
    Matrix step(double in_time, const Matrix& in_mat, double in_dt) const
    {
        Matrix k1, k2, k3, k4;
        k1 = m_func(in_time, in_mat);
        k2 = m_func(in_time + in_dt / 2, in_mat + (in_dt / 2) * k1);
        k3 = m_func(in_time + in_dt / 2, in_mat + (in_dt / 2) * k2);
        k4 = m_func(in_time + in_dt, in_mat + in_dt * k3);

        return in_mat + (k1 + 2 * k2 + 2 * k3 + k4) * in_dt / 6;
    }

private:
    std::function<Matrix(float, const Matrix&)> m_func;
};

Matrix vstack(const Matrix& in_mat1, const Matrix& in_mat2)
{
    if(in_mat1.rows() == 0)
    {
        return in_mat2;
    }
    else if(in_mat2.rows() == 0)
    {
        return in_mat1;
    }

    auto nbCols = in_mat1.cols();
    
    if(nbCols == 0)
    {
        nbCols = in_mat2.cols();
    }

    Matrix result(in_mat1.rows()+in_mat2.rows(), nbCols);
    result << in_mat1, in_mat2;
  
    return result;
}

Matrix getPauliMatrix(const std::string& in_pauli)
{
    if (in_pauli == "X")
    {
        Matrix pauliX{ Matrix::Zero(2, 2) };
        pauliX << 0, 1, 1, 0;
        return pauliX;
    }

    if (in_pauli == "Y")
    {
        Matrix pauliY{ Matrix::Zero(2, 2) };
        pauliY << 0, -I, I, 0;
        return pauliY;
    }

    if (in_pauli == "Z")
    {
        Matrix pauliZ{ Matrix::Zero(2, 2) };
        pauliZ << 1, 0, 0, -1;
        return pauliZ;
    }

    return Matrix::Identity(2, 2);
}

Matrix kron(const Matrix& in_mat1, const Matrix& in_mat2)
{
    const auto nbRowsMat1 = in_mat1.rows();
    const auto nbColsMat1 = in_mat1.cols();
    const auto nbRowsMat2 = in_mat2.rows();
    const auto nbColsMat2 =in_mat2.cols();
  
    Matrix resultMat(nbRowsMat1*nbRowsMat2, nbColsMat1*nbColsMat2);
    for (int i = 0; i < nbRowsMat1; ++i) 
    {
        for (int j = 0; j < nbColsMat1; ++j) 
        {
            resultMat.block(i*nbRowsMat2, j*nbColsMat2, nbRowsMat2, nbColsMat2) =  in_mat1(i,j)*in_mat2;
        }
    }

    return resultMat;
}
}

namespace xacc {
Matrix GOAT_PulseOptim::constructMatrixFromPauliString(const std::string& in_pauliString, int in_dimension)
{
    auto hamiltonianOps = std::static_pointer_cast<xacc::quantum::PauliOperator>(xacc::quantum::getObservable("pauli", in_pauliString));
    Matrix result;
    std::complex<double> coefficient = 1.0;
    for (auto termIter = hamiltonianOps->begin(); termIter != hamiltonianOps->end(); ++termIter)
    {
        coefficient = coefficient * termIter->second.coeff();
        std::unordered_map<int, std::string> qubitIdxToPauliOp;
        for (const auto& termOp : termIter->second.ops())
        {
            qubitIdxToPauliOp.emplace(termOp.first, termOp.second);
        }

        for (int i = 0; i < in_dimension; ++i)
        {
            const auto iter = qubitIdxToPauliOp.find(i);
            if (i == 0)
            {
                if (iter != qubitIdxToPauliOp.end())
                {
                    result = getPauliMatrix(iter->second);
                }
                else
                {
                    result = getPauliMatrix("I");
                }
            }
            else
            {
                if (iter != qubitIdxToPauliOp.end())
                {
                    result = kron(result, getPauliMatrix(iter->second));
                }
                else
                {
                    result = kron(result, getPauliMatrix("I"));
                }
            }
        }
    } 

    return coefficient * result;
}

void GoatHamiltonian::construct(int in_dimension, const std::string& in_H0, const std::vector<std::string>& in_Hi, const std::vector<std::string>& in_fi, const std::vector<std::string>& in_params)
{
    if (!in_H0.empty())
    {
        m_h0 = GOAT_PulseOptim::constructMatrixFromPauliString(in_H0, in_dimension);
    }
    else
    {
        m_h0 = Matrix::Zero(1 << in_dimension, 1 << in_dimension);
    }
    
    for (int i = 0; i < in_Hi.size(); ++i) 
    {
        hamOps.emplace_back(std::make_pair(in_fi[i], GOAT_PulseOptim::constructMatrixFromPauliString(in_Hi[i], in_dimension)));
    }

    m_paramVals.resize(in_params.size());
    for (int i = 0; i < in_params.size(); ++i)
    {
        m_symbolTable.add_variable(in_params[i], m_paramVals[i]);
    }
    // Add time variable
    m_symbolTable.add_variable("t", m_time);

    for (int i = 0; i < hamOps.size(); ++i)
    {
        expression_t expression;
        expression.register_symbol_table(m_symbolTable);
        const bool compileOk = m_parser.compile(hamOps[i].first, expression);
        assert(compileOk);
        // Cache the compiled expressions
        m_exprs.emplace_back(std::move(expression));
    }

    params = in_params;
    dimension = in_dimension;
    hamiltonian = [this](double in_time, OptimParams in_paramVals) -> Matrix {      
        assert(in_paramVals.size() == params.size());
       
        Matrix hamMat = m_h0;
        for (int i = 0; i < hamOps.size(); ++i)
        {
            const auto& expression = m_exprs[i];
            // Set the variables before evaluation
            m_paramVals = in_paramVals;
            m_time = in_time;
            const auto evaled = expression.value();
            hamMat = hamMat + evaled * hamOps[i].second;
        }
        
        return hamMat;
    };


    for (int idx = 0; idx < params.size(); ++idx)
    {
        // Differential of H w.r.t. sigma parameter
        dHda.emplace_back([this, idx](double in_time, OptimParams in_paramVals) -> Matrix {
            assert(in_paramVals.size() == params.size());
            Matrix hamMat = Matrix::Zero(1 << dimension, 1 << dimension);
            for (int i = 0; i < hamOps.size(); ++i)
            {
                const auto& expression = m_exprs[i];
                // Set the variables before evaluation
                m_paramVals = in_paramVals;
                m_time = in_time;
                // Calculate the derivative w.r.t. the parameter
                const auto derivativeEvaled = exprtk::derivative(expression, params[idx]);
                hamMat = hamMat + derivativeEvaled * hamOps[i].second;
            }
            
            return hamMat;
        });
    }

    assert(dHda.size() == params.size());
}

GOAT_PulseOptim::GOAT_PulseOptim(const Matrix& in_targetU, const Hamiltonian& in_hamiltonian, const dHdalpha& in_dHda, 
    const OptimParams& in_initialParams, double in_maxTime, 
    std::unique_ptr<IIntegrator>&& io_integrator, std::unique_ptr<IGradientStepper>&& io_gradStepper):
    xacc::OptFunction([this](const std::vector<double>& in_x, std::vector<double>& out_dx){ return eval(in_x, out_dx); }, in_initialParams.size()),
    m_targetU(in_targetU),
    m_hamiltonian(in_hamiltonian),
    m_dHda(in_dHda),
    m_integrator(std::move(io_integrator)),
    m_gradStepper(std::move(io_gradStepper)),
    m_initialParams(in_initialParams),
    m_maxTime(in_maxTime),
    m_iterationCounter(0)
{
    // If not provided, use the default propagator (Runge-Kutta) and gradient-based stepper (L-BFGS)
    if (!m_integrator)
    {
        m_integrator = std::make_unique<DefaultIntegrator>(in_maxTime/DEFAULT_NUMBER_STEPS);
    }

    if (!m_gradStepper)
    {
        m_gradStepper =  std::make_unique<DefaultGradientStepper>();
    }
}


GOAT_PulseOptim::OptimizationResult GOAT_PulseOptim::optimize()
{
    m_iterationCounter = 0;
    // Invoke the Gradient stepper to drive optimization procedure
    m_gradStepper->optimize(this, m_initialParams);
    
    // Return the result cache (update during optimization run)
    return m_resultCache;
}

double GOAT_PulseOptim::eval(const OptimParams& in_params, std::vector<double>& out_grads) 
{
    double fx = 0.0;
    assert(m_dHda.size() == in_params.size());
    // Set-up initial parameters
    const OptimParams& params = in_params;
  
    // Hamiltonian matrix @ t = 0; params = init
    const auto initialH = m_hamiltonian(0, params);
    // Hamiltonian must be a square matrix
    assert(initialH.rows() == initialH.cols());
    const auto dimH = initialH.rows();
    
    const auto integrateResult = m_integrator->integrate(m_hamiltonian, m_dHda, params, m_maxTime);
    // We expect the result is a matrix that has (nb_params+1)*dimH rows and dimH columns
    // i.e. we have U(T) and a vector of dUda(T) (one for each param) stacked on each other.
    assert(integrateResult.cols() == dimH);
    assert(integrateResult.rows() == dimH * (1 + params.size()));

    const auto evalCostFn = [this](const Matrix& in_uMat) -> double {
        assert(m_targetU.rows() == in_uMat.rows() && m_targetU.cols() == in_uMat.cols());
        // Cost/goal function, i.e. Eq. (4)
        return 1.0 - (1.0/m_targetU.rows())*std::abs((m_targetU.adjoint() * in_uMat).trace());
    };

    const auto evalCostFnGradient = [this, dimH](const std::vector<Matrix>& in_duMats, const OptimParams& in_params, double in_costVal) -> std::vector<double> {
        assert(in_duMats.size() == in_params.size());
        std::vector<double> results;

        for (int i = 0; i < in_params.size(); ++i)
        {
            assert(in_duMats[i].rows() == m_targetU.rows() && m_targetU.cols() == in_duMats[i].cols());  
            // Cost function gradients, i.e., Eq. (6)
            const auto gradResult = -std::real((1.0/dimH)*(m_targetU.adjoint() * in_duMats[i]).trace());
            results.emplace_back(gradResult);
        }

        return results;
    }; 

    const auto uMat = integrateResult(Eigen::seqN(0, dimH), Eigen::all);
    std::vector<Matrix> duMats;
    for (int i = 0; i < params.size(); ++i)
    {
        // Vector of dUda (one for each param)
        duMats.emplace_back(integrateResult(Eigen::seqN(dimH + i*dimH, dimH), Eigen::all));
    }

    fx = evalCostFn(uMat);
    // Evaluate the gradients
    out_grads = evalCostFnGradient(duMats, params, fx);

    const auto vecToString = [](const std::vector<double>& in_vec){
        std::string result;
        for (const auto& elem : in_vec)
        {
            result += (std::to_string(elem) + ", ");
        }

        return result;
    };

    // SUMMARY:
    {
        std::cout << "        Iteration: " << m_iterationCounter << "\n";
        std::cout << "Params = " << vecToString(in_params) << "\n";
        std::cout << "Gradients = " << vecToString(out_grads) << "\n";
        std::cout << "Cost Function Val = " << fx << "\n";
        std::cout << "===========================================\n";
    }
  
    {
        m_resultCache.finalCost = fx;
        m_resultCache.optParams = params;
        m_resultCache.nbIters = m_iterationCounter;
    }

    m_iterationCounter++;
    return fx;
}

GOAT_PulseOptim::DefaultIntegrator::DefaultIntegrator(double in_dt):
    m_dt(in_dt)
{}

Matrix GOAT_PulseOptim::DefaultIntegrator::integrate(const Hamiltonian& in_hamiltonian, const dHdalpha& in_dHda, const OptimParams& in_params, double in_stopTime)
{
    const auto nbParams = in_dHda.size();
    const auto initialH = in_hamiltonian(0, in_params);
    const auto dimH = initialH.rows();
    
    // initial condition: U(0) = I, dUdalpha(0) = 0
    Matrix U = Matrix::Identity(dimH, dimH);
    std::vector<Matrix> dUs;
    for (int i = 0; i < nbParams; ++i)
    {
        dUs.emplace_back(Matrix::Zero(dimH, dimH));
    }
   
    for (int i = 0; i * m_dt < in_stopTime; ++i)
    {
        const double currentTime = m_dt*i;
        const auto currentU = U;
        
        std::function<Matrix(float, const Matrix&)> uIntegrateFunc = [&in_hamiltonian, &in_params](float in_time, const Matrix& in_u) -> Matrix {
            const std::complex<double> I(0.0, 1.0);
            return -I*in_hamiltonian(in_time, in_params)*in_u;
        };
        const RungeKutta uProgagator(uIntegrateFunc);

        std::vector<RungeKutta> dUPropagators;
        for (const auto& dHda :  in_dHda)
        {
            std::function<Matrix(float, const Matrix&)> duIntegrateFunc = [&in_hamiltonian, &in_params, &dHda, &currentU](float in_time, const Matrix& in_u) -> Matrix {
                const std::complex<double> I(0.0, 1.0);
                return -I*(dHda(in_time, in_params)*currentU + in_hamiltonian(in_time, in_params)*in_u);
            };
            const RungeKutta dUProgagator(duIntegrateFunc);

            dUPropagators.emplace_back(dUProgagator);
        }
   
        // Propagate U 
        U = uProgagator.step(currentTime, U, m_dt);
        // Propagate dUdalpha (params)
        for (int j = 0; j < nbParams; ++j)
        {
           dUs[j] = dUPropagators[j].step(currentTime, dUs[j], m_dt);
        }
    }
   
    Matrix result = U;     
    for (const auto& dUMat : dUs)
    {
        result = vstack(result, dUMat);
    }

    return result;
}

void GOAT_PulseOptim::DefaultGradientStepper::optimize(xacc::OptFunction* io_problem, const OptimParams& in_initialParams) 
{
    // TODO: get rid of this Default Stepper.
    // For now, keep this gradient stepper as the baseline since it's simple to debug.
    // Set up parameters
    LBFGSpp::LBFGSParam<double> param;
    param.epsilon = 1e-6;
    param.max_iterations = 50;
    LBFGSpp::LBFGSSolver<double> solver(param);
    
    double fx = 0.0;
    Eigen::VectorXd x = Eigen::VectorXd::Map(in_initialParams.data(), in_initialParams.size());
    
    // This LBFGSpp uses Eigen Vectors as params, hence we just need to convert b/w Eigen and std vectors.
    const auto optimProb = [&io_problem](const Eigen::VectorXd& in_params, Eigen::VectorXd& out_grads) -> double {
        OptimParams params;
        params.resize(in_params.size());
        Eigen::VectorXd::Map(&params[0], params.size()) = in_params;

        std::vector<double> grads(params.size(), 0.0);
        const auto result = (*io_problem)(params, grads);
        for (int i = 0; i < grads.size(); ++i)
        {
            out_grads[i] = grads[i];
        }

        return result;
    };

    solver.minimize(optimProb, x, fx);
}

void GOAT_PulseOptim::MLPackGradientStepper::optimize(xacc::OptFunction* io_problem, const OptimParams& in_initialParams) 
{
    auto optimizer = xacc::getService<xacc::Optimizer>("mlpack"); // ML-PACK optimizer;
    optimizer->setOptions(xacc::HeterogeneousMap {
        std::make_pair("nlopt-maxeval", 50),
        std::make_pair("mlpack-optimizer", "l-bfgs"),
        std::make_pair("initial-parameters", in_initialParams),
        // we don't want to waste time over-optimizing the params, hence set a reasonable min-step
        std::make_pair("bfgs-min-step", 0.001)
    });

    optimizer->optimize(*io_problem);
}

// ============= GOAT ==========================
// - Required: { "dimension" : int } : system dimension (number of qubits)
// 
// - Required: { "target-U" : string or EigenMatrix } : 
//   If string, represents a Pauli observable type string
//  e.g. "X", "X1Z2", etc.
//   If matrix, expecting a unitary matrix whose size matches the system dimension.
//
// - Optional: { "static-H" : string} : Pauli observable observable type string
//  describing time-indedependent (static) Hamiltonian
//  e.g. 5.1234 Z1Z2
//
// - Required: { "control-params" : vector<string> } : list of control parameters
//  e.g. { "gamma1", "gamma2", ... }
//
// - Required: { "control-funcs" : vector<string> }  : control functions 
//  e.g. { "sin(gamma1*t)", "exp(gamma2^2)", ... }
//  We support most basic functions (ExprTk) and "t" is an implicitly known param (time)
//
// - Required: { "control-H" : vector<string> } : list of Hamiltonian terms corresponding to the control functions.
//  Note: "control-H" and "control-funcs" must have the *same* number of elements
//
//  and "control-params" must include all possible params that may appear in those "control-funcs".
//
// - Required: { "initial-parameters" : vector<double> }: initial values for control parameters
//
// - Required: { "max-time" : double }: max control time horizon
//
// - Optional: { "optimizer" : string }: can be "ml-pack" or "default"
void PulseOptimGOAT::setOptions(const HeterogeneousMap& in_options)
{   
    const auto fatalError = [](const std::string& in_fieldName){
        xacc::error("Fatal: '" + in_fieldName + "' field is required.");
        return;
    };

    // Handle GOAT optimal control
    int dimension = 0;
    if (in_options.keyExists<int>("dimension")) 
    {
        dimension = in_options.get<int>("dimension");
    }
    else
    {
        return fatalError("dimension");
    }

    auto pauliMatrixUtil = xacc::getService<xacc::UnitaryMatrixUtil>("Pauli");
    const Matrix targetUmat = pauliMatrixUtil->fromHeterogeneousMap(in_options, dimension);

    // H0 is optional
    std::string H0;
    if (in_options.stringExists("static-H")) 
    {
        H0 = in_options.getString("static-H");
    }

    std::vector<std::string> controlParams;    
    if (in_options.keyExists<std::vector<std::string>>("control-params")) 
    {
        controlParams = in_options.get<std::vector<std::string>>("control-params");
    }
    else
    {
        return fatalError("control-params");
    }

    std::vector<std::string> controlFuncs;    
    if (in_options.keyExists<std::vector<std::string>>("control-funcs")) 
    {
        controlFuncs = in_options.get<std::vector<std::string>>("control-funcs");
    }
    else
    {
        return fatalError("control-funcs");
    }

    std::vector<std::string> controlOps;    
    if (in_options.keyExists<std::vector<std::string>>("control-H")) 
    {
        controlOps = in_options.get<std::vector<std::string>>("control-H");
    }
    else
    {
        return fatalError("control-H");
    }


    std::vector<double> initParams;    
    if (in_options.keyExists<std::vector<double>>("initial-parameters")) 
    {
        initParams = in_options.get<std::vector<double>>("initial-parameters");
    }
    else
    {
        return fatalError("initial-parameters");
    }

    double tMax = 0.0;
    if (in_options.keyExists<double>("max-time")) 
    {
        tMax = in_options.get<double>("max-time");
    }
    else
    {
        return fatalError("max-time");
    }

    std::string optimizer;
    if (in_options.stringExists("optimizer")) 
    {
        optimizer = in_options.getString("optimizer");
    }

    if (dimension < 1 || dimension > 10)
    {
        xacc::error("Invalid system dimension.");
        return;
    }

    if (controlFuncs.size() != controlOps.size())
    {
        xacc::error("The number of control functions must match the number of time-dependent Hamiltonian terms.");
        return;
    }

    if (controlFuncs.empty())
    {
        xacc::error("No control terms were specified.");
        return;
    }

    if (initParams.size() != controlParams.size())
    {
        xacc::error("The number of initial values must match the number control parameters.");
        return;
    }

    if (initParams.empty())
    {
        xacc::error("No control parameters were specified.");
        return;
    }

    if (tMax < 0.0)
    {
        xacc::error("Invalid max time parameter.");
        return;
    }

    if (!optimizer.empty() && optimizer != "default" && optimizer != "ml-pack")
    {
        xacc::error("Invalid optimizer.");
        return;
    }
    
    std::unique_ptr<IGradientStepper> gradientOptimizer = [](const std::string& in_optimizerName) -> std::unique_ptr<IGradientStepper> {
        if (in_optimizerName == "ml-pack") 
        {
            return std::make_unique<GOAT_PulseOptim::MLPackGradientStepper>();
        }
        
        return std::make_unique<GOAT_PulseOptim::DefaultGradientStepper>();
    }(optimizer);

    // We have a valid set of paramters here.
    m_hamiltonian = std::make_unique<GoatHamiltonian>();
    m_hamiltonian->construct(dimension, H0, controlOps, controlFuncs, controlParams);
    // All parameters have been validated: construct the GOAT pulse optimizer
    m_goatOptimizer = std::make_unique<GOAT_PulseOptim>(targetUmat, m_hamiltonian->hamiltonian, m_hamiltonian->dHda, initParams, tMax, nullptr, std::move(gradientOptimizer));
}

OptResult PulseOptimGOAT::optimize() 
{
    const auto result = m_goatOptimizer->optimize();
    return std::make_pair(result.finalCost, result.optParams);
}

Eigen::MatrixXcd PauliUnitaryMatrixUtil::fromHeterogeneousMap(const HeterogeneousMap& in_options, int in_dimension, const std::string& in_fieldName = "")
{
    const std::string fieldName = in_fieldName.empty() ? "target-U" : in_fieldName;
    const std::complex<double> I(0.0, 1.0);
    Matrix targetUmat = Matrix::Identity(1 << in_dimension, 1 << in_dimension);
   
    // If a string type "target-U" was provided:
    if (in_options.stringExists("target-U")) 
    {
        const std::string targetU = in_options.getString("target-U");
        targetUmat = fromString(targetU, in_dimension) * (-I);
    }
    else if (in_options.keyExists<Matrix>("target-U"))
    {
        targetUmat = in_options.get<Matrix>("target-U");
    }
    else if (in_options.keyExists<std::vector<std::complex<double>>>("target-U"))
    {
        const auto inMat = in_options.get<std::vector<std::complex<double>>>("target-U");
        const int mDim = static_cast<int>(std::sqrt(inMat.size()));
        targetUmat = Matrix::Zero(mDim, mDim);
        for (int i = 0; i < mDim; ++i)
        {
            for (int j = 0; j < mDim; ++j)
            {
                const auto idx = i*mDim + j;
                targetUmat(i, j) = inMat[idx];
            }
        }

        // Handle global phase (extra i multiplier in the U matrix): 
        // this has no implication in the unitary (global phase), but affects the gradient calculator.
        // For example, an Rx(theta) is defined with anti-diagonal terms of '-i*sin(theta/2)'
        // but X gate (theta = pi) is defined without that *i* term.
        // Hence, we check if the overall matrix don't have any imaginary elements,
        // then we need to add a global '-i' multiplier.
        const bool hasImagElem = [&inMat](){
            for (const auto& elem : inMat)
            {
                if (std::abs(elem.imag()) > 1.0e-12)
                {
                    return true;
                }
            }
            return false;
        }();

        if (!hasImagElem)
        {
            targetUmat *= (-I);
        }
    }

    return targetUmat;    
}
}