/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#include "InstructionIterator.hpp"
#include "IRProvider.hpp"
#include "CLIParser.hpp"
#include <signal.h>
#include <cstdlib>
#include <fstream>
#include "xacc_config.hpp"
#include "cxxopts.hpp"

#include "xacc_service.hpp"

#include <sys/types.h>
#include <sys/stat.h>

using namespace cxxopts;

namespace xacc {

bool isPyApi = false;
bool xaccFrameworkInitialized = false;
std::shared_ptr<CLIParser> xaccCLParser;
int argc = 0;
char **argv = NULL;
std::map<std::string, std::shared_ptr<CompositeInstruction>>
    compilation_database{};
std::string rootPathString = "";

int getArgc() { return argc; }
char **getArgv() { return argv; }
void Initialize(std::vector<std::string> argv) {
  std::vector<char *> cstrs;
  argv.insert(argv.begin(), "appExec");
  for (auto &s : argv) {
    cstrs.push_back(&s.front());
  }
  Initialize(argv.size(), cstrs.data());
}

void Initialize() { Initialize(std::vector<std::string>{}); }

bool isInitialized() { return xaccFrameworkInitialized; }

void ctrl_c_handler(int signal) {
  error("Caught CTRL-C, exiting the framework.");
  Finalize();
  exit(1);
}

std::vector<std::string> getIncludePaths() {
  return xaccCLParser->getIncludePaths();
}

void PyInitialize(const std::string rootPath) {
  std::vector<std::string> args{"--xacc-root-path", rootPath};
  Initialize(args);
}

void Initialize(int arc, char **arv) {

  if (!xaccFrameworkInitialized) {
    xaccCLParser = std::make_shared<CLIParser>();
    argc = arc;
    argv = arv;

    xacc::ServiceAPI_Initialize(argc, argv);

    // Parse any user-supplied command line options
    xaccCLParser->parse(argc, argv);
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    rootPathString = xacc::getRootPathString();
  }

  // We're good if we make it here, so indicate that we've been
  // initialized
  xacc::xaccFrameworkInitialized = true;

  if (!optionExists("queue-preamble")) {
    XACCLogger::instance()->dumpQueue();
  }
}

void setIsPyApi() { isPyApi = true; }

void setGlobalLoggerPredicate(MessagePredicate predicate) {
  XACCLogger::instance()->setGlobalLoggerPredicate(predicate);
  XACCLogger::instance()->dumpQueue();
}

void info(const std::string &msg, MessagePredicate predicate) {
  XACCLogger::instance()->info(msg, predicate);
}

void warning(const std::string &msg, MessagePredicate predicate) {
  XACCLogger::instance()->warning(msg, predicate);
}

void debug(const std::string &msg, MessagePredicate predicate) {
  XACCLogger::instance()->debug(msg, predicate);
}

void error(const std::string &msg, MessagePredicate predicate) {
  print_backtrace();
  if (isPyApi) {
    throw std::runtime_error(msg);
  } else {
    XACCLogger::instance()->error(msg, predicate);
    XACCLogger::instance()->error("Framework Exiting", predicate);
    xacc::Finalize();
    exit(-1);
  }
}

qbit qalloc(const int n) {
  qbit q(n);
  return q; // std::make_shared<xacc::AcceleratorBuffer>(n);
}
qbit qalloc() {
  qbit q;
  return q;
}

void addCommandLineOption(const std::string &optionName,
                          const std::string &optionDescription) {
  xaccCLParser->addStringOption(optionName, optionDescription);
}

void addCommandLineOptions(const std::string &category,
                           const std::map<std::string, std::string> &options) {
  xaccCLParser->addStringOptions(category, options);
}

void addCommandLineOptions(const std::map<std::string, std::string> &options) {
  xaccCLParser->addOptions(options);
}

bool optionExists(const std::string &optionKey) {
  return RuntimeOptions::instance()->exists(optionKey);
}

const std::string getOption(const std::string &optionKey) {
  if (!optionExists(optionKey)) {
    error("Invalid runtime option - " + optionKey);
  }
  return (*RuntimeOptions::instance())[optionKey];
}

void setOption(const std::string &optionKey, const std::string &value) {
  if (optionExists(optionKey)) {
    (*RuntimeOptions::instance())[optionKey] = value;
  } else {
    RuntimeOptions::instance()->insert(std::make_pair(optionKey, value));
  }
}
void unsetOption(const std::string &optionKey) {
  if (optionExists(optionKey)) {
    (*RuntimeOptions::instance()).erase(optionKey);
  }
  return;
}

void setCompiler(const std::string &compilerName) {
  setOption("compiler", compilerName);
}
void setAccelerator(const std::string &acceleratorName) {
  setOption("accelerator", acceleratorName);
}

std::shared_ptr<Accelerator> getAccelerator() {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use.\nPlease execute "
          "xacc::Initialize() before using API.");
  }
  if (!optionExists("accelerator")) {
    error("Invalid use of XACC API.\nxacc::getAccelerator() with no string "
          "argument\n"
          "requires that you set --accelerator at the command line.");
  }

  auto name = getOption("accelerator");
  HeterogeneousMap m;
  auto name_backend = split(name, ':');
  auto acc = xacc::getService<Accelerator>(name_backend[0], false);
  if (name_backend.size() > 1) {
    std::string b = name_backend[1];
    m.insert("backend", b);
    setOption(name_backend[0] + "-backend", name_backend[1]);
  }

  if (acc) {
    acc->initialize(m);
  } else {
    error("Invalid Accelerator.\nCould not find " + getOption("accelerator") +
          " in Accelerator Registry.");
  }
  return acc;
}
std::shared_ptr<Accelerator> getAccelerator(const std::string &name,
                                            std::shared_ptr<Client> client,
                                            const HeterogeneousMap &params) {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use.\nPlease execute "
          "xacc::Initialize() before using API.");
  }

  auto name_backend = split(name, ':');
  auto acc = xacc::getService<Accelerator>(name_backend[0]);
  if (name_backend.size() > 1) {
    setOption(name_backend[0] + "-backend", name_backend[1]);
  }

  if (acc) {
    auto remoteacc = std::dynamic_pointer_cast<RemoteAccelerator>(acc);
    if (remoteacc) {
      remoteacc->setClient(client);
    }
    acc->initialize(params);
  } else {
    error("Invalid Accelerator. Could not find " + name +
          " in Accelerator Registry.");
  }
  return acc;
}

std::shared_ptr<Accelerator> getAccelerator(const std::string &name,
                                            const HeterogeneousMap &params) {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }
  HeterogeneousMap m = params;
  auto name_backend = split(name, ':');
  auto acc = xacc::getService<Accelerator>(name_backend[0], false);
  if (name_backend.size() > 1) {
    std::string b = name_backend[1];
    m.insert("backend", b);
    setOption(name_backend[0] + "-backend", name_backend[1]);
  }

  if (acc) {
    //   std::stringstream ss;
    //   m.print<std::string,int>(ss);
    //   std::cout << "SS:\n" << ss.str() << "\n";
    acc->initialize(m);
  } else {

    if (xacc::hasContributedService<Accelerator>(name)) {

      acc = xacc::getContributedService<Accelerator>(name);
      if (acc)
        acc->initialize(params);

    } else {
      error("Invalid Accelerator. Could not find " + name +
            " in Accelerator Registry.");
    }
  }
  return acc;
}

bool hasAccelerator(const std::string &name) {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }
  return xacc::hasService<Accelerator>(name);
}

std::shared_ptr<Compiler> getCompiler(const std::string &name) {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }
  auto c = xacc::getService<Compiler>(name, false);
  if (!c) {
    if (xacc::hasContributedService<Compiler>(name)) {
      c = xacc::getContributedService<Compiler>(name);
    } else {
      error("Invalid Compiler. Could not find " + name +
            " in Service Registry.");
    }
  }
  return c;
}

std::shared_ptr<Algorithm> getAlgorithm(const std::string name) {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }
  return xacc::getService<Algorithm>(name);
}

std::shared_ptr<Algorithm> getAlgorithm(const std::string name,
                                        const xacc::HeterogeneousMap &params) {
  auto algo = xacc::getAlgorithm(name);
  if (!algo->initialize(params)) {
    error("Error initializing " + name + " algorithm.");
  }
  return algo;
}
std::shared_ptr<Algorithm> getAlgorithm(const std::string name,
                                        const xacc::HeterogeneousMap &&params) {
  return getAlgorithm(name, params);
}

std::shared_ptr<Optimizer> getOptimizer(const std::string name) {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }
  return xacc::getService<Optimizer>(name);
}

std::shared_ptr<Optimizer> getOptimizer(const std::string name,
                                        const HeterogeneousMap &opts) {
  auto opt = getOptimizer(name);
  opt->setOptions(opts);
  return opt;
}

std::shared_ptr<Optimizer> getOptimizer(const std::string name,
                                        const HeterogeneousMap &&opts) {
  return getOptimizer(name, opts);
}

std::shared_ptr<IRProvider> getIRProvider(const std::string &name) {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }

  auto irp = xacc::getService<IRProvider>(name);
  if (!irp) {
    error("Invalid IRProvider. Could not find " + name +
          " in Service Registry.");
  }
  return irp;
}

// std::shared_ptr<IRGenerator> getIRGenerator(const std::string &name) {
//   if (!xacc::xaccFrameworkInitialized) {
//     error("XACC not initialized before use. Please execute "
//           "xacc::Initialize() before using API.");
//   }

//   auto irp = xacc::getService<IRGenerator>(name, false);
//   if (!irp) {
//     if (xacc::hasContributedService<IRGenerator>(name)) {
//       irp = xacc::getContributedService<IRGenerator>(name);
//     } else {
//       error("Invalid IRProvicer. Could not find " + name +
//             " in Service Registry.");
//     }
//   }
//   return irp;
// }

std::shared_ptr<Compiler> getCompiler() {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }
  auto options = RuntimeOptions::instance();
  if (!optionExists("compiler")) {
    error("Invalid use of XACC API. getCompiler() with no string argument "
          "requires that you set --compiler at the command line.");
  }
  auto compiler = xacc::getService<Compiler>(getOption("compiler"));
  if (!compiler) {
    error("Invalid Compiler. Could not find " + (*options)["compiler"] +
          " in Compiler Registry.");
  }
  return compiler;
}

bool hasCompiler(const std::string &name) {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }

  return xacc::hasService<Compiler>(name);
}

std::shared_ptr<IRTransformation>
getIRTransformations(const std::string &name) {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }
  auto t = xacc::getService<IRTransformation>(name);
  if (!t) {
    error("Invalid IRTransformation. Could not find " + name +
          " in Service Registry.");
  }

  return t;
}
const std::string translate(std::shared_ptr<CompositeInstruction> ci,
                            const std::string toLanguage) {
  auto toLanguageCompiler = getCompiler(toLanguage);
  return toLanguageCompiler->translate(ci);
}

void clearOptions() { RuntimeOptions::instance()->clear(); }

std::shared_ptr<CompositeInstruction>
optimizeCompositeInstruction(const std::string optimizer,
                             std::shared_ptr<CompositeInstruction> inst) {
  auto ir = getService<IRProvider>("gate")->createIR();
  ir->addComposite(inst);
  auto opt = getService<IRTransformation>(optimizer);
  auto newir = opt->transform(ir);
  auto optF = newir->getComposites()[0];
  return optF->enabledView();
}

bool hasCache(const std::string fileName, const std::string subdirectory) {
  auto rootPathStr = xacc::getRootPathString();
  if (!subdirectory.empty()) {
    rootPathStr += "/" + subdirectory;
    if (!xacc::directoryExists(rootPathStr)) {
      return false;
    }
  }
  return xacc::fileExists(rootPathStr + "/" + fileName);
}

HeterogeneousMap getCache(const std::string fileName,
                          const std::string subdirectory) {
  std::string rootPathStr = xacc::getRootPathString();
  if (!subdirectory.empty()) {
    rootPathStr += "/" + subdirectory;
    if (!xacc::directoryExists(rootPathStr)) {
      error("Tried to get Cache at " + rootPathStr + "/" + fileName +
            ", and it does not exist.");
    }
  }

  std::ifstream t(rootPathStr + "/" + fileName);
  std::string json(std::istreambuf_iterator<char>{t}, {});
  std::istringstream s(json);
  auto buffer = std::make_shared<AcceleratorBuffer>();
  buffer->useAsCache();

  buffer->load(s);

  auto info = buffer->getInformation();
  HeterogeneousMap c;
  for (auto &kv : info) {
    std::string key = kv.first;
    ExtraInfoValue2HeterogeneousMap e2h(c, key);
    mpark::visit(e2h, kv.second);
  }
  std::stringstream ss;
  return c;
}

void appendCache(const std::string fileName, HeterogeneousMap &params,
                 const std::string subdirectory) {

  // This will over write the ip cache file

  auto rootPathStr = xacc::getRootPathString();

  if (!subdirectory.empty()) {
    rootPathStr += "/" + subdirectory;
    if (!xacc::directoryExists(rootPathStr)) {
      auto status =
          mkdir(rootPathStr.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
  }
  std::map<std::string, ExtraInfo> einfo;
  HeterogenousMap2ExtraInfo h2ei(einfo);
  params.visit(h2ei);
  //   InstructionParameter2ExtraInfo ip2e;
  AcceleratorBuffer b;
  b.useAsCache();

  for (auto &kv : einfo) {
    // auto extrainfo = mpark::visit(ip2e, kv.second);
    b.addExtraInfo(kv.first, kv.second);
  }

  std::ofstream out(rootPathStr + "/" + fileName);
  std::stringstream s;
  b.print(s);
  out << s.str();
  out.close();
}
const std::string getRootDirectory() { return xacc::getRootPathString(); }

void appendCompiled(std::shared_ptr<CompositeInstruction> composite,
                    bool _override) {
  if (!_override) {
    if (compilation_database.count(composite->name())) {
      xacc::error("Invalid CompositeInstruction name, already in compilation "
                  "database: " +
                  composite->name() + ".");
    }
  }

  compilation_database[composite->name()] = composite;
}
bool hasCompiled(const std::string name) {
  return compilation_database.count(name);
}

std::shared_ptr<CompositeInstruction> getCompiled(const std::string name) {
  if (!compilation_database.count(name)) {
    xacc::error(
        "Invalid CompositeInstruction requested. Not in compilation database " +
        name);
  }
  return compilation_database[name];
}

void qasm(const std::string &qasmString) {
  std::regex rgx(".compiler \\w+"), rgxx(".circuit \\w+"),
      buffrgx(".qbit \\w+");
  std::smatch match, match2, match3;
  std::map<std::string, std::string> function2code;

  if (!std::regex_search(qasmString.begin(), qasmString.end(), match, rgx)) {
    error("Cannot parse which compiler this qasm corresponds to.");
  }

  std::vector<std::string> variables;
  std::string bufferName = "b";
  auto compiler = split(match[0], ' ')[1];
  auto lines = split(qasmString, '\n');
  std::string currentFunctionName = "";
  for (auto &l : lines) {
    xacc::trim(l);
    if (l.find(".compiler") == std::string::npos &&
        l.find(".circuit") == std::string::npos &&
        l.find(".parameters") == std::string::npos &&
        l.find(".qbit") == std::string::npos && !l.empty()) {
      function2code[currentFunctionName] += l + "\n";
    }

    if (l.find(".circuit") != std::string::npos) {
      currentFunctionName = split(l, ' ')[1];
    }

    if (l.find(".parameters") != std::string::npos) {
      auto tmp = split(l, ' ');
      std::string varLine = tmp[1];
      for (auto i = 2; i < tmp.size(); i++) {
        varLine += tmp[i];
      }

      tmp = split(varLine, ',');
      if (tmp.size() > 1) {
        // we had t0,t1,t2,...
        for (auto &p : tmp) {
          variables.push_back(p);
        }
      } else {
        variables.push_back(tmp[0]);
      }
    }
    if (l.find(".qbit") != std::string::npos) {
      bufferName = split(l, ' ')[1];
    }
  }

  std::string variablesString = "";
  if (!variables.empty()) {
    for (auto &v : variables) {
      variablesString += ", double " + v;
    }
  }

  std::vector<std::string> newQasms;
  for (auto &kv : function2code) {
    newQasms.push_back("__qpu__ void " + kv.first + "(qbit " + bufferName +
                       variablesString + ") {\n" + kv.second + "\n}\n");
  }

  std::shared_ptr<IR> ir;
  for (auto &newQasm : newQasms) {
    if (optionExists("accelerator")) {
      ir = getCompiler(compiler)->compile(newQasm, getAccelerator());
    } else {
      ir = getCompiler(compiler)->compile(newQasm);
    }

    auto k = ir->getComposites()[0];
    appendCompiled(k, true);
  }

  for (auto &k : ir->getComposites())
    appendCompiled(k, true);
}

void Finalize() {
  XACCLogger::instance()->dumpQueue();
  if (xaccFrameworkInitialized) {
    xacc::xaccFrameworkInitialized = false;
    xacc::ServiceAPI_Finalize();
  }
}

} // namespace xacc
