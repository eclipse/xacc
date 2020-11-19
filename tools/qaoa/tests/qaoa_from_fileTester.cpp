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


#include <gtest/gtest.h>
#include <memory>

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

#include "qaoa_from_file.hpp"
#include "xacc_config.hpp"
#include <regex>
using json = nlohmann::json;

// TODO: Write a test for command line as well

TEST(qaoa_from_file_tester, checkSimple) {

    std::stringstream ss;
    ss << "{\n  \"graph\": \"" << XACC_QAOA_DIR;
    std::string configs = R"(/examples/graph_input_example.txt",
        "outputfile": true,
        "xacc":{
            "accelerator": "qpp",
            "optimizer": "mlpack"
        },
        "optimizer-params":{
            "algorithm": "adagrad",
            "step-size": 0.1,
            "max-iter": 30
        },
        "qaoa-params":{
            "p": 1
        }
    })";

    ss << configs;
    configs = ss.str();
    auto config = json::parse(configs);
    qaoa_from_file QAOA(config);
    QAOA.execute();
}


int main(int argc, char **argv) {
    xacc::Initialize(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    auto ret = RUN_ALL_TESTS();
    xacc::Finalize();
    return ret;
}