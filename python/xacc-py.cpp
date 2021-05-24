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
#include "xacc.hpp"
#include "xacc_service.hpp"

#include "InstructionIterator.hpp"
#include "AcceleratorDecorator.hpp"

#include "py_heterogeneous_map.hpp"
#include "py_ir.hpp"
#include "py_accelerator.hpp"
#include "py_compiler.hpp"
#include "py_algorithm.hpp"
#include "py_optimizer.hpp"
#include "py_observable.hpp"
#include "py_graph.hpp"
#include "xacc-quantum-py.hpp"

namespace py = pybind11;
using namespace xacc;

PYBIND11_MODULE(_pyxacc, m) {
  m.doc() =
      "Python bindings for XACC. XACC provides a plugin infrastructure for "
      "programming, compiling, and executing quantum kernels in a language and "
      "hardware agnostic manner.";

  bind_heterogeneous_map(m);
  bind_ir(m);
  bind_accelerator(m);
  bind_compiler(m);
  bind_algorithm(m);
  bind_optimizer(m);
  bind_observable(m);
  bind_quantum(m);
  bind_graph(m);
  // Expose XACC API functions
  m.def("Initialize", (void (*)(std::vector<std::string>)) & xacc::Initialize,
        "Initialize the framework. Can provide a list of strings to model "
        "command line arguments. For instance, "
        "to print the XACC help message, pass ['--help'] to this function, or "
        "to set the compiler to use, ['--compiler','scaffold'].");
  m.def("Initialize", (void (*)()) & xacc::Initialize,
        "Initialize the framework. Use this if there are no command line "
        "arguments to pass.");
  m.def("PyInitialize", &xacc::PyInitialize,
        "Initialize the framework from Python.");
  m.def("appendPluginPath", &xacc::addPluginSearchPath, "");
  m.def(
      "getAccelerator",
      [](const std::string &name, const PyHeterogeneousMap &p = {}) {
        HeterogeneousMap m;
        for (auto &item : p) {
          PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
          mpark::visit(vis, item.second);
        }
        return xacc::getAccelerator(name, m);
      },
      py::arg("name"), py::arg("p") = PyHeterogeneousMap(),
      py::return_value_policy::reference,
      "Return the accelerator with given name.");
  m.def("getCompiler",
        (std::shared_ptr<xacc::Compiler>(*)(const std::string &)) &
            xacc::getCompiler,
        py::return_value_policy::reference,
        "Return the Compiler of given name.");
  m.def("getIRTransformation", &xacc::getIRTransformation,
        py::return_value_policy::reference,
        "Return the IRTransformation of given name.");
  m.def("getConnectivity",
        [](const std::string acc) -> std::vector<std::pair<int, int>> {
          auto a = xacc::getAccelerator(acc);
          return a->getConnectivity();
        });
  m.def("translate", &xacc::translate,
        "Translate the provided IR Function to the given language.");
  m.def("setOption", [](const std::string s, InstructionParameter p) {
    xacc::setOption(s, p.toString());
  });
  m.def("qasm", &xacc::qasm, "");
  m.def("getCompiled", &xacc::getCompiled, "");
  m.def(
      "internal_qalloc_with_size",
      [](const int n) -> std::shared_ptr<xacc::AcceleratorBuffer> {
        return xacc::qalloc(n);
      },
      "");
  m.def(
      "internal_qalloc_no_size",
      []() -> std::shared_ptr<xacc::AcceleratorBuffer> {
        return xacc::qalloc();
      },
      "");
  m.def(
      "info", [](const std::string s) { xacc::info(s); }, "");
  m.def(
      "debug", [](const std::string s) { xacc::debug(s); }, "");
  m.def(
      "error", [](const std::string s) { xacc::error(s); }, "");
  m.def("setOption", &xacc::setOption, "Set an XACC framework option.");
  m.def(
      "setOptions",
      [](std::map<std::string, std::string> options) {
        for (auto &kv : options)
          xacc::setOption(kv.first, kv.second);
      },
      "Set a number of options at once.");
  m.def("getAcceleratorDecorator",
        [](const std::string name,
           std::shared_ptr<Accelerator> acc) -> std::shared_ptr<Accelerator> {
          auto accd = xacc::getAcceleratorDecorator(name, acc);
          return accd;
        });
  m.def("getAcceleratorDecorator",
        [](const std::string name, std::shared_ptr<Accelerator> acc,
           const PyHeterogeneousMap &options) -> std::shared_ptr<Accelerator> {
          HeterogeneousMap m;
          for (auto &item : options) {
            PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
            mpark::visit(vis, item.second);
          }
          auto accd = xacc::getAcceleratorDecorator(name, acc, m);
          return accd;
        });
  m.def("asComposite", &xacc::ir::asComposite, "");
  m.def("asInstruction", &xacc::ir::asInstruction, "");
  m.def(
      "setOptions",
      [](std::map<std::string, InstructionParameter> options) {
        for (auto &kv : options)
          xacc::setOption(kv.first, kv.second.toString());
      },
      "Set a number of options at once.");
  m.def("unsetOption", &xacc::unsetOption, "");
  m.def("getOption",
        (const std::string (*)(const std::string &)) & xacc::getOption,
        "Get an XACC framework option.");
  m.def("hasBuffer", &xacc::hasBuffer, "");
  m.def("hasAccelerator", &xacc::hasAccelerator,
        "Does XACC have the given Accelerator installed?");
  m.def("hasCompiler", &xacc::hasCompiler,
        "Does XACC have the given Accelerator installed?");
  m.def("optionExists", &xacc::optionExists, "Set an XACC framework option.");
  m.def("setIsPyApi", &xacc::setIsPyApi,
        "Indicate that this is using XACC via the Python API.");
  m.def("getCache", &xacc::getCache, "");
  m.def("getIRProvider", &xacc::getIRProvider, "");
  m.def("Finalize", &xacc::Finalize, "Finalize the framework");
  m.def(
      "loadBuffer",
      [](const std::string json) {
        std::istringstream s(json);
        auto buffer = std::make_shared<AcceleratorBuffer>();
        buffer->load(s);
        return buffer;
      },
      "");
  m.def("getAlgorithm",
        (std::shared_ptr<xacc::Algorithm>(*)(const std::string)) &
            xacc::getAlgorithm,
        "");
  m.def(
      "getAlgorithm",
      [](const std::string algo, const PyHeterogeneousMap &options) {
        HeterogeneousMap m;
        for (auto &item : options) {
          PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
          mpark::visit(vis, item.second);
        }
        return xacc::getAlgorithm(algo, m);
      },
      "");
  m.def(
      "getCompiler",
      [](const std::string c, const PyHeterogeneousMap &options) {
        HeterogeneousMap m;
        for (auto &item : options) {
          PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
          mpark::visit(vis, item.second);
        }
        auto compiler = xacc::getCompiler(c);
        compiler->setExtraOptions(m);
        return compiler;
      },
      "");
  m.def(
      "storeBuffer",
      [](std::shared_ptr<AcceleratorBuffer> buffer) {
        xacc::storeBuffer(buffer);
      },
      "");
  m.def("getOptimizer",
        (std::shared_ptr<xacc::Optimizer>(*)(const std::string)) &
            xacc::getOptimizer,
        "");
  m.def("set_verbose", &xacc::set_verbose, "");
  m.def(
      "getOptimizer",
      [](const std::string optName, const PyHeterogeneousMap &options) {
        HeterogeneousMap m;
        for (auto &item : options) {
          PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
          mpark::visit(vis, item.second);
        }
        return xacc::getOptimizer(optName, m);
      },

      "");
  m.def("getComposite", [](const std::string &name) {
    auto c = xacc::getService<Instruction>(name);
    return std::dynamic_pointer_cast<CompositeInstruction>(c);
  });
  m.def(
      "createCompositeInstruction",
      [](const std::string &name, const PyHeterogeneousMap &options) {
        HeterogeneousMap m;
        for (auto &item : options) {
          PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
          mpark::visit(vis, item.second);
        }
        auto inst = xacc::getService<Instruction>(name);
        auto comp = std::dynamic_pointer_cast<CompositeInstruction>(inst);
        if (comp) {
          comp->expand(m);
        } else {
          xacc::error("Cannot cast to a Composite Instruction");
        }
        return comp;
      },
      "");
  m.def(
      "contributeService",
      [](const std::string name, ContributableService &service) {
        xacc::contributeService(name, service);
      },
      "");

  m.def(
      "compileKernel",
      [](std::shared_ptr<Accelerator> acc, const std::string &src,
         const std::string &compilerName =
             "") -> std::shared_ptr<CompositeInstruction> {
        if (!compilerName.empty())
          xacc::setOption("compiler", compilerName);

        auto compiler = xacc::getCompiler();
        auto ir = compiler->compile(src, acc);
        return ir->getComposites()[0];
      },
      py::arg("acc"), py::arg("src"), py::arg("compilerName") = std::string(""),
      py::return_value_policy::move, "");

  m.def("createComposite", [](const std::string name) {
    std::shared_ptr<CompositeInstruction> inst;
    auto tmp = xacc::getIRProvider("quantum")->createInstruction(name, {});
    inst = std::dynamic_pointer_cast<CompositeInstruction>(tmp);
    if (!inst) {
      xacc::error("Not a valid composite instruction");
    }
    return inst;
  });
  m.def(
      "compositeToInstruction",
      [](std::shared_ptr<CompositeInstruction> f) {
        return std::dynamic_pointer_cast<Instruction>(f);
      },
      py::return_value_policy::copy);

  py::module gatesub =
      m.def_submodule("gate", "Gate model quantum computing data structures.");
  gatesub.def(
      "create",
      [](const std::string &name, std::vector<std::size_t> qbits,
         std::vector<InstructionParameter> params =
             std::vector<InstructionParameter>{})
          -> std::shared_ptr<Instruction> {
        return xacc::getService<IRProvider>("quantum")->createInstruction(
            name, qbits, params);
      },
      "", py::arg("name"), py::arg("qbits"),
      py::arg("params") = std::vector<InstructionParameter>{});
  gatesub.def(
      "createComposite",
      [](const std::string &name,
         std::vector<std::string> variables = std::vector<std::string>{})
          -> std::shared_ptr<CompositeInstruction> {
        return xacc::getService<IRProvider>("quantum")->createComposite(
            name, variables);
      },
      "", py::arg("name"), py::arg("variables") = std::vector<std::string>{});
  gatesub.def(
      "createIR",
      []() -> std::shared_ptr<IR> {
        return xacc::getService<IRProvider>("quantum")->createIR();
      },
      "");

  gatesub.def(
      "getState",
      [](std::shared_ptr<Accelerator> acc,
         std::shared_ptr<CompositeInstruction> f) {
        auto results = acc->getAcceleratorState(f);
        Eigen::VectorXcd ret =
            Eigen::Map<Eigen::VectorXcd>(results.data(), results.size());
        return ret;
      },
      "Compute and return the state after execution of the given program on "
      "the given accelerator.");

  py::module annealingsub = m.def_submodule(
      "annealing", "Annealing model quantum computing data structures.");
  annealingsub.def(
      "create_composite_from_qubo",
      [](std::map<std::pair<int, int>, double> Q) {
        auto prov = xacc::getIRProvider("quantum");
        auto comp = prov->createComposite("composite_from_qubo", {}, "anneal");
        for (auto &kv : Q) {
          if (kv.first.first == kv.first.second) {
            std::size_t f = kv.first.first;
            auto qmi = prov->createInstruction("dwqmi", {f, f}, {kv.second});
            comp->addInstruction(qmi);
          } else {
            std::size_t f = kv.first.first;
            std::size_t g = kv.first.second;
            auto qmi = prov->createInstruction("dwqmi", {f, g}, {kv.second});
            comp->addInstruction(qmi);
          }
        }
        return comp;
      },
      "");
}
