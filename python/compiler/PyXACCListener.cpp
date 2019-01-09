/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#include <iostream>
#include <IRProvider.hpp>
#include "PyXACCListener.hpp"
#include "XACC.hpp"
#include "GateFunction.hpp"
#include "IRGenerator.hpp"

using namespace pyxacc;

namespace xacc {
namespace quantum {

void DWorGateListener::enterGate(PyXACCIRParser::GateContext *ctx) {
  // Get the name of the gate
  auto gate = ctx->getText();

  // Until we see a qmi or anneal instruction, keep checking
  // the incoming gate name
  if (!isDW)
    isDW = boost::contains(gate, "qmi") || boost::contains(gate, "anneal");

  // Once we have seen qmi or anneal, ensure that we
  // don't see any gate model instructions
  if (isDW && (gate != "qmi" && gate != "anneal"))
    xacc::error("Cannot mix gate and dwave instructions.");
}

PyXACCListener::PyXACCListener(std::shared_ptr<Accelerator> acc)
    : accelerator(acc) {
  provider = xacc::getService<IRProvider>("gate");
}

PyXACCListener::PyXACCListener(bool _useDw, std::shared_ptr<Accelerator> acc)
    : useDW(_useDw), accelerator(acc) {
  if (_useDw)
    provider = xacc::getService<IRProvider>("dwave");
  else
    provider = xacc::getService<IRProvider>("gate");
}

std::shared_ptr<Function> PyXACCListener::getKernel() { return f; }

void PyXACCListener::enterXacckernel(PyXACCIRParser::XacckernelContext *ctx) {
  // Note here we expect to have a function with possible nParameters,
  // The first parameter must always be the AcceleratorBuffer
  if (ctx->param().empty()) {
    xacc::error("XACC Python kernels must have at least one argument - the "
                "AcceleratorBuffer.");
  }
  bufferName = ctx->param(0)->getText();

  std::vector<InstructionParameter> params;
  for (int i = 1; i < ctx->param().size(); i++) {
    if (!boost::contains(ctx->param(i)->getText(), "*")) {
      params.push_back(InstructionParameter(ctx->param(i)->getText()));
      functionVariableNames.push_back(ctx->param(i)->getText());
    }
  }
  f = provider->createFunction(ctx->kernelname->getText(), {}, params);
}

void PyXACCListener::enterUop(PyXACCIRParser::UopContext *ctx) {
  // Note, if the user has specified something like
  // H(...) or H(0...2), then we have a different handle
  if (ctx->allbitsOp() != nullptr)
     return;

  auto is_double = [](const std::string &s) -> bool {
    try {
      std::stod(s);
    } catch (std::exception &e) {
      return false;
    }
    return true;
  };
  auto is_int = [](const std::string &s) -> bool {
    try {
      std::stoi(s);
    } catch (std::exception &e) {
      return false;
    }
    return true;
  };

  auto gateName = ctx->gatename->getText();
  boost::trim(gateName);

  if (gateName == "CX") {
    gateName = "CNOT";
  } else if (gateName == "MEASURE") {
    gateName = "Measure";
  }

  if (xacc::hasService<GateInstruction>(gateName)) {

    // We have to accept GATE(params..., qbits..., opt=val,...)
    auto tmpInst = provider->createInstruction(gateName, std::vector<int>{});
    auto nBits = tmpInst->nRequiredBits();

    auto nArgs = ctx->explist()->exp().size();
    int nOptions = 0;
    // how many options are there?
    for (int i = 0; i < nArgs; i++) {
      if (ctx->explist()->exp(i)->key() != nullptr) {
        nOptions++;
      }
    }

    // Parameters may be optional sometimes
    auto nParams = nArgs - nOptions - nBits;

    // get all InstructionParameters first
    std::vector<InstructionParameter> params;
    for (int i = 0; i < nParams; i++) {
      auto tmp = ctx->explist()->exp(i);
      if (gateName == "Measure") {
        InstructionParameter param(std::stoi(tmp->getText()));
        params.push_back(param);
      } else {
        if (tmp->real() != nullptr || tmp->id() != nullptr) {
          auto str = tmp->getText();
          auto param = is_double(str) ? InstructionParameter(std::stod(str))
                                      : InstructionParameter(str);
          params.push_back(param);
        } else if (tmp->exp().size() == 1 && tmp->exp(0)->real() != nullptr) {
          // this is a neg double
          params.push_back(InstructionParameter(std::stod(tmp->getText())));
        } else {
          params.push_back(InstructionParameter(tmp->getText()));
        }
      }
    }

    // Get qubit indices
    std::vector<int> qubits;
    for (int i = nParams; i < nBits + nParams; i++) {
      qubits.push_back(std::stoi(ctx->explist()->exp(i)->INT()->getText()));
    }

    auto inst = provider->createInstruction(gateName, qubits);
    int counter = 0;
    for (auto &p : params) {
      inst->setParameter(counter, p);
      counter++;
    }

    // See if we have any options
    for (int i = nBits + nParams; i < nArgs; i++) {
      auto key = ctx->explist()->exp(i)->key()->getText();
      auto valStr = ctx->explist()->exp(i)->exp(0)->getText();

      if (is_int(valStr)) {
        InstructionParameter p(std::stoi(valStr));
        inst->setOption(key, p);
      } else if (is_double(valStr)) {
        InstructionParameter p(std::stod(valStr));
        inst->setOption(key, p);
      } else {
        InstructionParameter p(valStr);
        inst->setOption(key, p);
      }

      // FIXME HANDLE OTHER TYPES ???
    }
    f->addInstruction(inst);

  } else if (xacc::hasService<IRGenerator>(gateName)) {
    // HERE we have something like uccsd(n_qubits=4, n_electrons=2)
    auto generatorName = gateName;
    std::map<std::string, InstructionParameter> params;
    for (int i = 0; i < ctx->explist()->exp().size(); i++) {
      auto key = ctx->explist()->exp(i)->key()->getText();
      auto valStr = ctx->explist()->exp(i)->exp(0)->getText();
      if (is_int(valStr)) {
        params.insert({key, InstructionParameter(std::stoi(valStr))});
      } else if (is_double(valStr)) {
        params.insert({key, InstructionParameter(std::stod(valStr))});
      } else {
        params.insert({key, InstructionParameter(valStr)});
      }

      // FIXME HANDLE OTHER TYPES
    }

    // Add the Function's InstructionParameters
    for (int i = 0; i < f->nParameters(); i++) {
      params.insert({"param_" + std::to_string(i), f->getParameter(i)});
    }

    auto generator = xacc::getService<xacc::IRGenerator>(generatorName);
    auto genF = generator->generate(params);

    // We may have a IRGenerator that produces d-wave functions,
    // if so, we will not have set to correct provider
    if (!std::dynamic_pointer_cast<GateFunction>(genF)) {
      if (!xacc::hasCompiler("dwave-qmi")) {
        xacc::error(
            "Cannot run decorated code for d-wave without d-wave plugins.");
      }
      f = xacc::getService<IRProvider>("dwave")->createFunction(
          f->name(), {}, f->getParameters());
      auto dwcompiler = xacc::getCompiler("dwave-qmi");
      //   auto acc = xacc::getAccelerator("dwave");
      auto buff = accelerator->getBuffer(bufferName);
      buff->addExtraInfo("ir-generator", ExtraInfo(generator->name()));

      auto xaccKernelSrcStr = dwcompiler->translate("", genF);
      auto embeddedCode = dwcompiler->compile(xaccKernelSrcStr, accelerator);
      genF = embeddedCode->getKernels()[0];
    }

    for (auto i : genF->getInstructions()) {
      f->addInstruction(i);
    }

  } else if (gateName == "qmi") {
    std::vector<int> qubits;
    std::vector<InstructionParameter> params;
    qubits.push_back(std::stoi(ctx->explist()->exp(0)->INT()->getText()));
    qubits.push_back(std::stoi(ctx->explist()->exp(1)->INT()->getText()));
    auto str = ctx->explist()->exp(2)->getText();
    auto param = is_double(str) ? InstructionParameter(std::stod(str))
                                : InstructionParameter(str);
    params.push_back(param);
    auto gate = provider->createInstruction(gateName, qubits, params);
    f->addInstruction(gate);

  } /*else if (gateName == "xacc") {

    // SOON TO BE DEPRECATED IN FAVOR OR IRGENERATOR()
    auto generatorName = ctx->explist()->exp(0)->getText();
    std::map<std::string, InstructionParameter> params;
    for (int i = 1; i < ctx->explist()->exp().size(); i++) {
      auto key = ctx->explist()->exp(i)->id()->getText();
      auto valStr = ctx->explist()->exp(i)->exp(0)->getText();
      if (is_int(valStr)) {
        params.insert({key, InstructionParameter(std::stoi(valStr))});
      } else if (is_double(valStr)) {
        params.insert({key, InstructionParameter(std::stod(valStr))});
      } else {
        params.insert({key, InstructionParameter(valStr)});
      }
    }

    // Add the Function's InstructionParameters
    for (int i = 0; i < f->nParameters(); i++) {
      params.insert({"param_" + std::to_string(i), f->getParameter(i)});
    }

    auto generator = xacc::getService<xacc::IRGenerator>(generatorName);
    auto genF = generator->generate(params);

    // We may have a IRGenerator that produces d-wave functions,
    // if so, we will not have set to correct provider
    if (!std::dynamic_pointer_cast<GateFunction>(genF)) {
      if (!xacc::hasCompiler("dwave-qmi")) {
        xacc::error(
            "Cannot run decorated code for d-wave without d-wave plugins.");
      }
      f = xacc::getService<IRProvider>("dwave")->createFunction(
          f->name(), {}, f->getParameters());
      auto dwcompiler = xacc::getCompiler("dwave-qmi");
      //   auto acc = xacc::getAccelerator("dwave");
      auto buff = accelerator->getBuffer(bufferName);
      buff->addExtraInfo("ir-generator", ExtraInfo(generator->name()));

      auto xaccKernelSrcStr = dwcompiler->translate("", genF);
      auto embeddedCode = dwcompiler->compile(xaccKernelSrcStr, accelerator);
      genF = embeddedCode->getKernels()[0];
    }

    for (auto i : genF->getInstructions()) {
      f->addInstruction(i);
    }
  }*/
  else {
      xacc::error(gateName + " is an invalid instruction for the PyXACC IR compiler.");
  }
}

void PyXACCListener::enterAllbitsOp(PyXACCIRParser::AllbitsOpContext *ctx) {
  auto gateName = ctx->gatename->getText();
  boost::trim(gateName);

  if (gateName == "CX") {
    gateName = "CNOT";
  } else if (gateName == "MEASURE") {
    gateName = "Measure";
  }

  auto isRotation = [](const std::string &name) {
    return name == "Rx" || name == "Ry" || name == "Rz";
  };

  if (gateName == "CNOT" || gateName == "CZ" || gateName == "Measure" ||
      isRotation(gateName)) {
    xacc::error("Cannot use ellipses '...' for 2 qubit gates, Measure, or "
                "rotation gates.");
  }

  auto buff = accelerator->getBuffer(bufferName);
  auto nQubits = buff->size();
  int start = 0, end = nQubits - 1;
  if (ctx->INT().size() == 1) {
    xacc::error("You cannot specify GATE(INT ...) or GATE(...INT). Only "
                "GATE(INT...INT).");
  } else if (ctx->INT().size() == 2) {
    start = std::stoi(ctx->INT(0)->getText());
    end = std::stoi(ctx->INT(1)->getText());
    if (end >= nQubits)
      xacc::error("Invalid qubit indices for ellipses. Must be [start,end] "
                  "(end inclusive)");
  } else if (!ctx->INT().empty()) {
    xacc::error("Invalid use of ellipses: " + ctx->getText());
  }

  for (int i = start; i <= end; i++) {
    auto inst = provider->createInstruction(gateName, {i});
    f->addInstruction(inst);
  }
}

void PyXACCListener::exitXacckernel(PyXACCIRParser::XacckernelContext *ctx) {

  // If this is a D-Wave kernel, then we need to check
  // and see if an embedding has been set, if not, then
  // lets run the D-Wave compiler to set one.
  if (!std::dynamic_pointer_cast<GateFunction>(f)) {
    if (!xacc::hasCompiler("dwave-qmi")) {
      xacc::error(
          "Cannot run decorated code for d-wave without d-wave plugins.");
    }

    auto acc = xacc::getAccelerator("dwave");
    auto buff = acc->getBuffer(bufferName);

    if (!buff->hasExtraInfoKey("embedding")) {
      xacc::info("Embedding not found for this python kernel, computing "
                 "embedding now.");
      auto dwcompiler = xacc::getCompiler("dwave-qmi");
      auto xaccKernelSrcStr = dwcompiler->translate("", f);
      auto embeddedCode = dwcompiler->compile(xaccKernelSrcStr, acc);
    }
  }
}
} // namespace quantum
} // namespace xacc
