/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/

#include "aswap.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include <assert.h>

namespace {
    // Value to denote that an empty (not set) SZ value
    const double SZ_VALUE_NONE = std::numeric_limits<double>::max();
    const std::string PARAM_PREFIX = "t";
    
    template <typename T>
    std::string to_string_with_precision(const T in_val, int in_precision = 12)
    {
        std::ostringstream out;
        out.precision(in_precision);
        out << std::fixed << in_val;
        return out.str();
    }    

    template<typename... Args>
    std::string generateString(const Args&... args)
    {
        std::string result;
        int unpack[]{0, (result += to_string_with_precision(args), 0)...};
        static_cast<void>(unpack);
        return result;
    }

    int binomialCoefficient(int n, int k)  
    {  
        // Base Cases  
        if (k == 0 || k == n) 
        {
            return 1;  
        }            
    
        // Recursive  
        return binomialCoefficient(n - 1, k - 1) + binomialCoefficient(n - 1, k);  
    }  
}

namespace xacc {
namespace circuits {
    ASWAP::ASWAP():
        xacc::quantum::Circuit("ASWAP"),
        m_nbQubits(0),
        m_nbParticles(0),
        m_timeReversalSymmetry(true),
        m_szSymmetry(SZ_VALUE_NONE),
        // Note: don't try to get Quantum IR service in the constructor!!!
        m_gateRegistry(nullptr)
    {}

    bool ASWAP::expand(const xacc::HeterogeneousMap& runtimeOptions) 
    {
        m_gateRegistry = xacc::getService<IRProvider>("quantum");
        
        if (!m_gateRegistry)
        {
            return false;
        }

        // Params:
        // Check required params
        if (!runtimeOptions.keyExists<int>("nbQubits") || !runtimeOptions.keyExists<int>("nbParticles"))
        {
            xacc::error("Required params are missing!");
            return false;
        } 
        
        m_nbQubits = runtimeOptions.get<int>("nbQubits");
        m_nbParticles = runtimeOptions.get<int>("nbParticles");
        
        // Must have more than 2 qubits; number of particles must be positive and less than the number of qubits.
        if (m_nbQubits < 2 || m_nbParticles < 0 || m_nbParticles > m_nbQubits)
        {
            xacc::error("Invalid parameters! Please check your inputs...");
            return false;
        }

        // Always set default time reversal symmetry
        m_timeReversalSymmetry = true;
        if (runtimeOptions.keyExists<bool>("timeReversalSymmetry"))
        {
            m_timeReversalSymmetry = runtimeOptions.get<bool>("timeReversalSymmetry");
            xacc::info("Set 'timeReversalSymmetry' to " + std::to_string(m_timeReversalSymmetry));
        }
        else
        {
            xacc::warning("'timeReversalSymmetry' was not provided, default to " + std::to_string(m_timeReversalSymmetry));
        }
        
        if (runtimeOptions.keyExists<double>("szSymmetry"))
        {
            m_szSymmetry = runtimeOptions.get<double>("szSymmetry");
            xacc::info("Set 'szSymmetry' to " + std::to_string(m_szSymmetry));
        }
        else
        {
            xacc::warning("'szSymmetry' was not provided. Assume no symmetry.");
        }
       
        initializeParamList();
        constructCircuit();
        
        return true;
    }

    void ASWAP::initializeParamList()
    {
      const int nAblocks = (m_nbQubits == 2)
                               ? 1
                               : binomialCoefficient(m_nbQubits, m_nbParticles);

      const int numberParams = [&]() -> int {
        if (m_nbQubits == 2) {
          return m_timeReversalSymmetry ? 1 : 2;
        } else {
          // Minimum number of parameters: not include the ones that we can
          // fixed: For timeReversalSymmetry: all phi params are set to zero and
          // 1 extra theta param can be set to zero. For
          // non-timeReversalSymmetry: two phi params are *free* params.
          return m_timeReversalSymmetry
                     ? binomialCoefficient(m_nbQubits, m_nbParticles) - 1
                     : 2 * (binomialCoefficient(m_nbQubits, m_nbParticles) - 1);
        }
      }();

      // Compute the separate limits for theta and phi params:
      const int numThetaParams = m_timeReversalSymmetry ? numberParams : nAblocks;
      const int numPhiParams = numberParams - numThetaParams;
      int thetaCount = 0;
      int phiCount = 0;
      const int NB_ANGLES_PER_BLOCK = m_timeReversalSymmetry ? 1 : 2;
      // Named variables are provided
      if (!variables.empty()) {
        if (variables.size() != numberParams) {
          xacc::error("[ASwap] The number of variables doesn't match the "
                      "configuration. Provided " +
                      std::to_string(variables.size()) + "; expected " +
                      std::to_string(numberParams));
        }

        int varIdx = 0;
        for (int i = 0; i < nAblocks; ++i) {
          // Theta:
          if (thetaCount < numThetaParams) {
            m_paramList.emplace_back(variables[varIdx++]);
            thetaCount++;
          } else {
            // Fix the free theta paramter to zero:
            // Note: an A block with both theta and phi are zero
            // ==> CZ gate.
            // Just add a zero value to be resolved by expr_tk
            m_paramList.emplace_back("0.0");
          }
          // Only need phi if non-time reversal symmetry
          if (!m_timeReversalSymmetry) {
            // Phi
            if (phiCount < numPhiParams) {
              m_paramList.emplace_back(variables[varIdx++]);
              phiCount++;
            } else {
              m_paramList.emplace_back("0.0");
            }
          }
        }
      } else {
        // No named variables, create variables if needed.
        for (int i = 0; i < nAblocks; ++i) {
          // Theta:
          if (thetaCount < numThetaParams) {
            m_paramList.emplace_back(generateString("theta", i));
            addVariable(m_paramList.back());
            thetaCount++;
          } else {
            // Fix the free theta paramter to zero:
            // Note: an A block with both theta and phi are zero
            // ==> CZ gate.
            // Just add a zero value to be resolved by expr_tk
            m_paramList.emplace_back("0.0");
          }
          // Only need phi if non-time reversal symmetry
          if (!m_timeReversalSymmetry) {
            // Phi
            if (phiCount < numPhiParams) {
              m_paramList.emplace_back(generateString("phi", i));
              addVariable(m_paramList.back());
              phiCount++;
            } else {
              m_paramList.emplace_back("0.0");
            }
          }
        }
      }

      assert(phiCount == numPhiParams);
      assert(thetaCount == numThetaParams);
      assert(variables.size() == numberParams);
    }
    
    void ASWAP::constructCircuit()
    {
        // Note: we don't have a *generic* method to reduce param count for sz symmetry for arbitrary number of qubits.
        // The paper did provide some special-case handlers for 6 and 8 qubit cases.
        std::vector<std::pair<int, int>> controlTargetPairs;

        if (2 * m_nbParticles < m_nbQubits)
        {
            // Initialize *occupied* spin orbital
            // (alternating qubits)
            for (int i = 0; i < m_nbParticles; ++i)
            {
                X(2*i + 1);
            }

            for (int i = 0; i < m_nbParticles + 1; ++i)
            {
                for (int ii = 0; ii < m_nbParticles; ++ii)
                {
                    controlTargetPairs.emplace_back(2 * ii + 1, 2 * ii + 2);
                }

                for (int ii = 0; ii < m_nbParticles; ++ii)
                {
                    controlTargetPairs.emplace_back(2 * ii, 2 * ii + 1);
                }
            }
        }
        else if (2 * m_nbParticles == m_nbQubits)
        {
            for (int i = 0; i < m_nbParticles; ++i)
            {
                X(2*i);
            }

            for (int i = 0; i < (m_paramList.size()/(m_nbQubits-1)) + 1; ++i)
            {
                for (int ii = 0; ii < m_nbParticles; ++ii)
                {
                    controlTargetPairs.emplace_back(2 * ii, 2 * ii + 1);
                }

                for (int ii = 0; ii < m_nbParticles - 1; ++ii)
                {
                    controlTargetPairs.emplace_back(2 * ii + 1, 2 * ii + 2);
                }
            }    
        }
        else
        {
            for (int i = 0; i < m_nbParticles; ++i)
            {
                X(i + m_nbQubits - m_nbParticles);
            }
            
            for (int i = 0; i < m_nbQubits - m_nbParticles; ++i)
            {
                for (int ii = 0; ii < m_nbQubits - 1; ++ii)
                {
                    controlTargetPairs.emplace_back(ii, ii + 1);
                }            
            }
        }  

        int paramCounter = 0;
        if (!m_timeReversalSymmetry)      
        {  
            for (const auto& pair : controlTargetPairs)
            {
                addAGate(pair.first, pair.second, m_paramList.at(paramCounter), m_paramList.at(paramCounter + 1));
                if (paramCounter < m_paramList.size() - 2)
                {
                    paramCounter += 2;
                }
                else
                {
                   break;
                }
                
            }
        }
        else
        {
            for (const auto& pair : controlTargetPairs)
            {
                // Phi param is 0.0 (i.e. not a variational parameter)
                addAGate(pair.first, pair.second, m_paramList.at(paramCounter), "0.0");
                if (paramCounter < m_paramList.size() - 1)
                {
                    paramCounter += 1;
                }
                else
                {
                    break;
                }                
            }
        }        
    }

// Ignore warnings int->size_t conversions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
    void ASWAP::CNOT(int in_q1, int in_q2)
    {
        auto cnot = m_gateRegistry->createInstruction("CNOT", std::vector<std::size_t>{ in_q1, in_q2 });
        addInstruction(cnot);
    }

    void ASWAP::X(int in_qubit)
    {
        auto xGate = m_gateRegistry->createInstruction("X", std::vector<std::size_t>{ in_qubit });
        addInstruction(xGate);
    }

    void ASWAP::H(int in_qubit)
    {
        auto hGate = m_gateRegistry->createInstruction("H", std::vector<std::size_t>{ in_qubit });
        addInstruction(hGate);
    }

    void ASWAP::Rx(int in_qubit, const std::string& in_param)
    {
        auto rotationGate = m_gateRegistry->createInstruction("Rx", std::vector<std::size_t>{ in_qubit });
        InstructionParameter gateParam(in_param);
        rotationGate->setParameter(0, gateParam);
        addInstruction(rotationGate);
    }

    void ASWAP::Rx(int in_qubit, double in_angle)
    {
        auto rotationGate = m_gateRegistry->createInstruction("Rx", std::vector<std::size_t>{ in_qubit });
        InstructionParameter gateParam(in_angle);
        rotationGate->setParameter(0, gateParam);
        addInstruction(rotationGate);
    }

    void ASWAP::Ry(int in_qubit, const std::string& in_param)
    {
        auto rotationGate = m_gateRegistry->createInstruction("Ry", std::vector<std::size_t>{ in_qubit });
        InstructionParameter gateParam(in_param);
        rotationGate->setParameter(0, gateParam);
        addInstruction(rotationGate);
    }

    void ASWAP::Ry(int in_qubit, double in_angle)
    {
        auto rotationGate = m_gateRegistry->createInstruction("Ry", std::vector<std::size_t>{ in_qubit });
        InstructionParameter gateParam(in_angle);
        rotationGate->setParameter(0, gateParam);
        addInstruction(rotationGate);
    }

    void ASWAP::Rz(int in_qubit, const std::string& in_param)
    {
        auto rotationGate = m_gateRegistry->createInstruction("Rz", std::vector<std::size_t>{ in_qubit });
        InstructionParameter gateParam(in_param);
        rotationGate->setParameter(0, gateParam);
        addInstruction(rotationGate);
    }

    void ASWAP::Rz(int in_qubit, double in_angle)
    {
        auto rotationGate = m_gateRegistry->createInstruction("Rz", std::vector<std::size_t>{ in_qubit });
        InstructionParameter gateParam(in_angle);
        rotationGate->setParameter(0, gateParam);
        addInstruction(rotationGate);
    }
#pragma GCC diagnostic pop

    void ASWAP::addAGate(int in_q1, int in_q2, const std::string& in_thetaParam, const std::string& in_phiParam)
    {          
        CNOT(in_q2, in_q1);        
        
        // -(phi + pi)
        Rz(in_q2, generateString("-(", in_phiParam, " + ", M_PI, ")"));
        
        // -(theta + pi/2)
        Ry(in_q2, generateString("-(", in_thetaParam, " + ", M_PI_2, ")"));
        
        CNOT(in_q1, in_q2);
        
        // theta + pi/2
        Ry(in_q2, generateString(in_thetaParam,  " + ", M_PI_2));
        
        // phi + pi
        Rz(in_q2, generateString(in_phiParam,  " + ", M_PI));
        
        CNOT(in_q2, in_q1);
    }

    const std::vector<std::string> ASWAP::requiredKeys()
    {
        // This circuit requires qubit array is in the 0 initial state.
        // Note: szSymmetry requires known initial states anyway.
        static const std::vector<std::string> requiredParams {
            "nbQubits", // (type: int): number of qubits (i.e. spin orbitals)
            "nbParticles", // (type: int; constraint <= nbQubits): number of particles (i.e. electrons)
            "timeReversalSymmetry", // (type: bool; default = true): enforce time reversal symmetry which effectively restricts the states generated have only real coefficients
            "szSymmetry", // (type: double, optional): Fix and conserve the spin sz value (e.g. + or - 1/2) to a particular choice.
        };
        return requiredParams;
    }
}
}