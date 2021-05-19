/*******************************************************************************
 * Copyright (c) 2021 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   A.M. Santana - initial API and implementation
 *******************************************************************************/

#include "qaoa_maxcut.hpp"

#include "xacc.hpp"
#include "Circuit.hpp"
#include "xacc_service.hpp"
#include "PauliOperator.hpp"
#include "xacc_observable.hpp"
#include "CompositeInstruction.hpp"
#include "AlgorithmGradientStrategy.hpp"

#include <cassert>
#include <iomanip>
#include <random>


// WARM-START FUNCTIONS:
auto random_vector(const double l_range, const double r_range, const std::size_t size) {
    // Generate a random initial parameter set 
    // WARNING: This function multiplies each output in the range
    // by 2*pi
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()}; // Generates random integers
    std::uniform_real_distribution<double> dist{l_range, r_range};
    auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };
    std::vector<double> vec(size);
    std::generate(vec.begin(), vec.end(), gen);
    for (int i=0; i < vec.size(); ++i){vec.at(i) *= 2 * M_PI;}
    return vec;
}

double cutValue2D(xacc::Graph* const graph, std::vector<double> positions)
{
    double value = 0.0;
    double w, ux, uy, vx, vy;
    for (int i = 0; i < graph->order(); ++i) {
        auto neighbors = graph->getNeighborList(i);
        for (const auto &neighborId : neighbors) {
            w = graph->getEdgeWeight(i,neighborId) ? graph->getEdgeWeight(i,neighborId) : 1.0;
            ux = std::cos(positions[i]);
            uy = std::sin(positions[i]);
            vx = std::cos(positions[neighborId]);
            vy = std::sin(positions[neighborId]);
            value += w * (std::pow(ux-vx,2) + std::pow(uy-vy,2));
        }
    }
    return (value / 4);
}

std::pair<double,double> regularize(double in_phi, double in_theta)
{
    auto theta = std::fmod(in_theta, 2 * M_PI);
    auto phi = in_phi;
    if (theta > M_PI)
    {
        theta = (2*M_PI) - theta;
        phi = std::fmod(in_phi + M_PI, 2 * M_PI);
    }
    return std::make_pair(phi,theta);
}

std::tuple<double,double,double> polartocart(double phi, double theta) 
{
    auto x = std::sin(theta) * std::cos(phi);
    auto y = std::sin(phi) * std::sin(theta);
    auto z = std::cos(theta);
    return std::make_tuple(x,y,z);
}

std::pair<double,double> carttopolar(double x, double y, double z)
{
    double phi = std::atan2(y,x);
    double theta = std::acos(z / std::sqrt(x*x+y*y+z*z));
    return std::make_pair(phi,theta);
}

std::pair<std::vector<double>, std::vector<double>> getSDPSolution(xacc::Graph* const graph)
{
    int n_nodes = graph->order();
    float W = 0;
    int trials = 1;

    float maxDelta = 10;
    std::vector<double> values, positions;
    double cutvalue, dTheta;
    // Setting best value to -inf.
    const int bestValue = std::numeric_limits<int>::min();
    for (int j=0; j < trials; ++j){
        positions = random_vector(0, 1, n_nodes);
        while (values.size() <= 100) {
            for (int i=0; i < n_nodes; ++i){
                cutvalue = cutValue2D(graph, positions);
                dTheta = (std::rand() - 0.5) / maxDelta;
                positions[i] += dTheta;
                if (cutValue2D(graph, positions) < cutvalue){
                    positions[i] -= dTheta;
                }
            }
            values.push_back(cutvalue);
        }
    }
    std::pair<std::vector<double>, std::vector<double>> solution = std::make_pair(positions, values);
    return solution;
}

std::pair<std::vector<double>, std::vector<double>> rotatesolution(std::pair<std::vector<double>, std::vector<double>> solution, int top)
{   
    std::vector<double> positions = std::get<0>(solution);
    std::vector<double> values = std::get<1>(solution);
    auto angles = regularize(positions[top-1], values[top-1]);
    auto phi = std::get<0>(angles);
    auto theta = std::get<1>(angles);
    theta *= -1.;
    std::vector<double> newPositions, newValues;
    std::tuple<double,double,double> xyz;
    std::pair<double,double> polar;
    double newX, newY, newZ;
    int i = 0;
    for (auto position : positions)
    {
        position = std::fmod(position - phi, 2 * M_PI);
        auto newvals = regularize(position, values[i]);
        position = std::get<0>(newvals);
        values[i] = std::get<1>(newvals);
        xyz = polartocart(position, values[i]);
        newX = std::cos(theta) * std::get<0>(xyz) * std::sin(theta) * std::get<2>(xyz);
        newY = std::get<1>(xyz);
        newZ = -1. * std::sin(theta) * std::get<0>(xyz) * std::cos(theta) * std::get<2>(xyz);
        polar = carttopolar(newX,newY,newZ);
        newPositions.push_back(std::get<0>(polar));
        newValues.push_back(std::get<1>(polar));
        i += 1;
    }
    phi = 2 * M_PI * std::rand();
    for (int i=0; i < newPositions.size(); ++i)
    {
        newPositions[i] = std::fmod(newPositions[i] + phi, 2 * M_PI);
    }
    std::pair<std::vector<double>, std::vector<double>> rotated_solution = std::make_pair(newPositions, newValues);
    return rotated_solution;
} 
// end warm_start functions

namespace xacc {
namespace algorithm {
    // Construct the MAX-CUT Hamiltonian as an XACC observable
    // based on input graphs edges
    std::shared_ptr<Observable> maxcut_qaoa::constructMaxCutHam(xacc::Graph *in_graph) const {
        xacc::quantum::PauliOperator H;
        for (int i = 0; i < in_graph->order(); ++i) {
            auto neighbors = in_graph->getNeighborList(i);
            for (const auto &neighborId : neighbors) {
                if (in_graph->getEdgeWeight(i,neighborId)){
                    auto weight = in_graph->getEdgeWeight(i,neighborId);
                    H += 0.5 * weight * (xacc::quantum::PauliOperator(1.) - xacc::quantum::PauliOperator({{i, "Z"}, {neighborId, "Z"}}));
                } else {
                H += 0.5 * (xacc::quantum::PauliOperator(1.) - xacc::quantum::PauliOperator({{i, "Z"}, {neighborId, "Z"}})); 
                }
            }
        }
        return xacc::quantum::getObservable("pauli", H.toString());
    }

    // Take a graph as an input and output a series of Rx/Rz instructions on 
    // each qubit for Warm-Start procedure (https://arxiv.org/abs/2010.14021)
    auto warm_start(xacc::Graph* m_graph) {
        auto provider = getIRProvider("quantum");
        auto initial_state = provider->createComposite("initial_state");
        auto solution1 = getSDPSolution(m_graph);
        int sol_size = std::get<0>(solution1).size() + std::get<1>(solution1).size();
        int rand = std::rand() % sol_size;
        auto rotated = rotatesolution(solution1, rand);
        std::vector<double> x_rotations = std::get<1>(rotated);
        std::vector<double> z_rotations = std::get<0>(rotated);
        for (size_t i = 0; i < m_graph->order(); ++i) {
            auto xrot = x_rotations[i];
            auto zrot = z_rotations[i] + (M_PI / 2);
            initial_state->addInstruction(provider->createInstruction("Rx", { i }, {xrot}));
            initial_state->addInstruction(provider->createInstruction("Rz", { i }, {zrot}));
        }
        return initial_state;
    }
    
    bool maxcut_qaoa::initialize(const HeterogeneousMap &parameters) {
        bool initializeOk = true;
        // Hyper-parameters for QAOA:
        // (1) Accelerator (QPU)
        if (!parameters.pointerLikeExists<Accelerator>("accelerator")) {
            std::cout << "'accelerator' is required.\n";
            // We check all required params; hence don't early return on failure.
            initializeOk = false;
        }

        // (2) Classical optimizer
        if (!parameters.pointerLikeExists<Optimizer>("optimizer")) {
            std::cout << "'optimizer' is required.\n";
            initializeOk = false;
        }

        // (3) Number of mixing and cost function steps to use (default = 1)
        m_nbSteps = 1;
        if (parameters.keyExists<int>("steps")) {
            m_nbSteps = parameters.get<int>("steps");
        }

        // (4) Cost Hamiltonian or a graph to construct the max-cut cost Hamiltonian
        // from.
        bool graphInput = false;
        if (parameters.pointerLikeExists<Graph>("graph")) {
            graphInput = true;
            m_graph = parameters.getPointerLike<Graph>("graph");
        } else {
            std::cout << "graph' is required.\n";
            initializeOk = false;
        } 
        
        // Default is Extended ParameterizedMode (less steps, more params)
        m_parameterizedMode = "Extended";
        if (parameters.stringExists("parameter-scheme")) {
            m_parameterizedMode = parameters.getString("parameter-scheme");
        }

        if (initializeOk) {
            m_costHamObs = constructMaxCutHam(parameters.getPointerLike<Graph>("graph"));
            m_qpu = parameters.getPointerLike<Accelerator>("accelerator");
            m_optimizer = parameters.getPointerLike<Optimizer>("optimizer");
            // Optional ref-hamiltonian
            m_refHamObs = nullptr;
            if (parameters.pointerLikeExists<Observable>("ref-ham")) {
            m_refHamObs = parameters.getPointerLike<Observable>("ref-ham");
            }
        }

        // Check if a parameter initialization routine has been specified
        // Current options: random (default), warm-starts, (eventually) Fourier 
        if (parameters.stringExists("initialization")) {
            m_initializationMode = parameters.getString("initialization");
        }

        if (parameters.keyExists<bool>("maximize")) {
            m_maximize = parameters.get<bool>("maximize");
        }
        m_shuffleTerms = false;
        if (parameters.keyExists<bool>("shuffle-terms")) {
          m_shuffleTerms = parameters.get<bool>("shuffle-terms");
        }
        if (m_optimizer && m_optimizer->isGradientBased() &&
            gradientStrategy == nullptr) {
            // No gradient strategy was provided, just use autodiff.
            gradientStrategy = xacc::getService<AlgorithmGradientStrategy>("autodiff");
            gradientStrategy->initialize(
                {{"observable", m_costHamObs}});
        }

        return initializeOk;
        }

        const std::vector<std::string> maxcut_qaoa::requiredParameters() const {
        return {"accelerator", "optimizer", "observable"};
        }
    
    void maxcut_qaoa::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {
        HeterogeneousMap m; 
        m.insert("accelerator", m_qpu);
        m.insert("optimizer", m_optimizer);
        m.insert("observable", m_costHamObs);
        m.insert("steps", m_nbSteps);
        m.insert("parameter-scheme", "Standard");
        m.insert("shuffle-terms", m_shuffleTerms);

        if (m_initializationMode == "warm-start") {
            m.insert("initial-state", warm_start(m_graph));
        } else if (m_initial_state){
            m.insert("initial-state", m_initial_state);
        }
        // Initialize QAOA
        auto qaoa = xacc::getAlgorithm("QAOA", m);
        // Allocate some qubits and execute
        qaoa->execute(buffer);
    }

    std::vector<double> maxcut_qaoa::execute(const std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<double> &x) {
        // Build up heterogeneous map of algorithm options to pass off to "QAOA" algorithm
        HeterogeneousMap m; 
        m.insert("accelerator", m_qpu);
        m.insert("optimizer", m_optimizer);
        m.insert("observable", m_costHamObs);
        m.insert("steps", m_nbSteps);
        m.insert("parameter-scheme", "Standard");
        m.insert("shuffle-terms", m_shuffleTerms);

        // If: warm-start selected, call the warm start function
        // and use as initial-state.
        // Else if: custom initial-state given, pass that to the
        // algorithm call.
        // Else: "QAOA" algorithm defaults to Hadamards on each qubit.
        if (m_initializationMode == "warm-start") {
            m.insert("initial-state", warm_start(m_graph));
        } else if (m_initial_state){
            m.insert("initial-state", m_initial_state);
        }

        // Initialize QAOA
        auto qaoa = xacc::getAlgorithm("QAOA", m);
        // Allocate some qubits and execute
        auto energy = qaoa->execute(buffer, x);
        return energy;
    }

} // namespace quantum
} // namespace xacc