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
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "hwe.hpp"
#include "exp.hpp"
#include "range.hpp"
#include "QFT.hpp"
#include "InverseQFT.hpp"

#include "uccsd.hpp"
#include "ucc1.hpp"
#include "ucc3.hpp"
#include "aswap.hpp"
#include "qfast.hpp"
#include "kak.hpp"
#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

using namespace cppmicroservices;

class US_ABI_LOCAL GeneratorsActivator : public BundleActivator {
public:
  GeneratorsActivator() {}

  void Start(BundleContext context) {
    auto hwe = std::make_shared<xacc::circuits::HWE>();
    auto expit = std::make_shared<xacc::circuits::Exp>();
    auto r = std::make_shared<xacc::circuits::Range>();
    auto q = std::make_shared<xacc::circuits::QFT>();
    auto iq = std::make_shared<xacc::circuits::InverseQFT>();
    auto u = std::make_shared<xacc::circuits::UCCSD>();
    auto u1 = std::make_shared<xacc::circuits::UCC1>();
    auto u3 = std::make_shared<xacc::circuits::UCC3>();
    auto aswap = std::make_shared<xacc::circuits::ASWAP>();
    auto qfast = std::make_shared<xacc::circuits::QFAST>();
    auto kak = std::make_shared<xacc::circuits::KAK>();
    auto zyz = std::make_shared<xacc::circuits::ZYZ>();

    context.RegisterService<xacc::Instruction>(hwe);
    context.RegisterService<xacc::Instruction>(expit);
    context.RegisterService<xacc::Instruction>(r);
    context.RegisterService<xacc::Instruction>(q);
    context.RegisterService<xacc::Instruction>(iq);
    context.RegisterService<xacc::Instruction>(u);
    context.RegisterService<xacc::Instruction>(u1);
    context.RegisterService<xacc::Instruction>(u3);
    context.RegisterService<xacc::Instruction>(aswap);
    context.RegisterService<xacc::Instruction>(qfast);
    context.RegisterService<xacc::Instruction>(kak);
    context.RegisterService<xacc::Instruction>(zyz);
  }

  void Stop(BundleContext context) {}
};

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(GeneratorsActivator)
