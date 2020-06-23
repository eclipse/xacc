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

#include "xacc.hpp"
#include <gtest/gtest.h>
#include "Circuit.hpp"
#include "Optimizer.hpp"
#include "xacc_observable.hpp"
#include "xacc_service.hpp"

using namespace xacc;

TEST(QFastTester, checkSimple) 
{
    auto tmp = xacc::getService<Instruction>("QFAST");
    auto qfast = std::dynamic_pointer_cast<quantum::Circuit>(tmp);
    qfast->expand({ });
}


int main(int argc, char **argv) {
    xacc::Initialize(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    xacc::set_verbose(true);
    auto ret = RUN_ALL_TESTS();
    xacc::Finalize();
    return ret;
}