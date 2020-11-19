/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Anthony Santana - initial API and implementation
 ******************************************************************************/

#pragma once
#include <string>
#include <fstream>
#include <streambuf>
#include <random>
#include "PauliOperator.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
#include "AlgorithmGradientStrategy.hpp"
#include "json.hpp"
#include <ctime>

using json = nlohmann::json;


class qaoa_from_file {

    protected:
        int m_steps;
        int m_max_iters;
        std::string m_acc_name;
        std::string m_opt_name;
        std::string m_opt_algo;
        std::string m_graph_file;
        std::string m_config_file;
        bool m_in_config;
        bool m_out_file;
        float m_step_size;
        
        json configs;

        void read_json();
        void read_hamiltonian(const std::string& graphFile, xacc::quantum::PauliOperator& H);
        xacc::HeterogeneousMap set_optimizer();

    public:
        qaoa_from_file(json& c) : configs(c) {
            m_in_config = true;
        }

        qaoa_from_file(const std::string& configFile, const std::string& graphFile, bool outFile, const std::string& optName, const std::string& optAlgo, const std::string& accName, bool inConfig, const int& nbSteps, const float& stepSize, const int& maxIters) : m_config_file(configFile), m_graph_file(graphFile), m_out_file(outFile), m_opt_name(optName), m_opt_algo(optAlgo), m_acc_name(accName), m_in_config(inConfig), m_steps(nbSteps), m_step_size(stepSize), m_max_iters(maxIters) {}

        void execute();

};