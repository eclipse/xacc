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
#include <memory>
#include <signal.h>
#include <cstdlib>
#include <fstream>
#include "xacc_config.hpp"
#include "cxxopts.hpp"
#include "AcceleratorDecorator.hpp"

#include "xacc_service.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include "TearDown.hpp"

using namespace cxxopts;

namespace xacc {

bool verbose = false;
bool isPyApi = false;
bool xaccFrameworkInitialized = false;
std::shared_ptr<CLIParser> xaccCLParser;
int argc = 0;
char **argv = NULL;
std::map<std::string, std::shared_ptr<CompositeInstruction>>
    compilation_database{};
std::map<std::string, std::shared_ptr<AcceleratorBuffer>> allocated_buffers{};

std::string rootPathString = "";

void set_verbose(bool v) { verbose = v; }

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

void logToFile(bool enable, const std::string &fileNamePrefix) {
  XACCLogger::instance()->logToFile(enable, fileNamePrefix);
}

void setLoggingLevel(int level) {
  XACCLogger::instance()->setLoggingLevel(level);
}

int getLoggingLevel() { return XACCLogger::instance()->getLoggingLevel(); }

void subscribeLoggingLevel(LoggingLevelNotification callback) {
  XACCLogger::instance()->subscribeLoggingLevel(callback);
}

void info(const std::string &msg, MessagePredicate predicate) {
  if (verbose)
    XACCLogger::instance()->info(msg, predicate);
}

void warning(const std::string &msg, MessagePredicate predicate) {
  if (verbose)
    XACCLogger::instance()->warning(msg, predicate);
}

void debug(const std::string &msg, MessagePredicate predicate) {
#ifdef _XACC_DEBUG
  if (verbose)
    XACCLogger::instance()->debug(msg, predicate);
#endif
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
  std::stringstream ss;
  ss << "qreg_" << q;
  q->setName(ss.str());
  allocated_buffers.insert({ss.str(), q});
  return q;
}
qbit qalloc() {
  qbit q;
  std::stringstream ss;
  ss << "qreg_" << q;
  q->setName(ss.str());
  allocated_buffers.insert({ss.str(), q});
  return q;
}

void storeBuffer(std::shared_ptr<AcceleratorBuffer> buffer) {
  auto name = buffer->name();
  if (allocated_buffers.count(name)) {
    // error("Invalid buffer name to store: " + name);
    allocated_buffers[name] = buffer;
  } else {
    allocated_buffers.insert({name, buffer});
  }
}

void storeBuffer(const std::string name,
                 std::shared_ptr<AcceleratorBuffer> buffer) {
  if (allocated_buffers.count(name)) {
    error("Invalid buffer name to store: " + name);
  }
  // if this buffer is in here already before we
  // set its new name, we should remove it from the allocation
  if (allocated_buffers.count(buffer->name())) {
    allocated_buffers.erase(buffer->name());
  }

  buffer->setName(name);
  allocated_buffers.insert({name, buffer});
}

std::shared_ptr<AcceleratorBuffer> getBuffer(const std::string &name) {
  if (!allocated_buffers.count(name)) {
    error("Invalid buffer name: " + name);
  }
  return allocated_buffers[name];
}
bool hasBuffer(const std::string &name) {
  return allocated_buffers.count(name);
}
std::shared_ptr<AcceleratorBuffer>
getClassicalRegHostBuffer(const std::string &cRegName) {
  if (hasBuffer(cRegName)) {
    return getBuffer(cRegName);
  }

  for (auto &[bufferName, allocated_buffer] : allocated_buffers) {
    if (xacc::container::contains(allocated_buffer->getClassicalRegs(),
                                  cRegName)) {
      return allocated_buffer;
    }
  }

  error("Invalid classical register name: " + cRegName);
  return nullptr;
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

std::shared_ptr<Accelerator>
getAcceleratorDecorator(const std::string &decorator,
                        std::shared_ptr<Accelerator> acc,
                        const HeterogeneousMap &params) {
  std::shared_ptr<AcceleratorDecorator> accd;
  if (xacc::hasService<AcceleratorDecorator>(decorator)) {
    accd = xacc::getService<AcceleratorDecorator>(decorator, false);
  } else if (xacc::hasContributedService<AcceleratorDecorator>(decorator)) {
    accd = xacc::getContributedService<AcceleratorDecorator>(decorator, false);
  }

  // It may have been stored as an Accelerator
  if (xacc::hasService<Accelerator>(decorator)) {
    auto tmp_accd = xacc::getService<Accelerator>(decorator, false);

    // accd will be null if can't cast
    accd = std::dynamic_pointer_cast<AcceleratorDecorator>(tmp_accd);
  } else if (xacc::hasContributedService<Accelerator>(decorator)) {
    auto tmp_accd = xacc::getContributedService<Accelerator>(decorator, false);
    accd = std::dynamic_pointer_cast<AcceleratorDecorator>(tmp_accd);
  }

  if (!accd) {
    xacc::error("Cannot find AcceleratorDecorator with name " + decorator);
  }

  accd->setDecorated(acc);
  accd->initialize(params);
  return accd;
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

  std::shared_ptr<Accelerator> acc;
  if (xacc::hasService<Accelerator>(name_backend[0])) {
    acc = xacc::getService<Accelerator>(name_backend[0], false);
  } else if (xacc::hasContributedService<Accelerator>(name_backend[0])) {
    acc = xacc::getContributedService<Accelerator>(name_backend[0], false);
  }

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

  std::shared_ptr<Accelerator> acc;
  if (xacc::hasService<Accelerator>(name_backend[0])) {
    acc = xacc::getService<Accelerator>(name_backend[0], false);
  } else if (xacc::hasContributedService<Accelerator>(name_backend[0])) {
    acc = xacc::getContributedService<Accelerator>(name_backend[0], false);
  }

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

  std::shared_ptr<Accelerator> acc;
  if (xacc::hasService<Accelerator>(name_backend[0])) {
    acc = xacc::getService<Accelerator>(name_backend[0], false);
  } else if (xacc::hasContributedService<Accelerator>(name_backend[0])) {
    acc = xacc::getContributedService<Accelerator>(name_backend[0], false);
  }

  if (name_backend.size() > 1) {
    std::string b = name_backend[1];
    m.insert("backend", b);
    setOption(name_backend[0] + "-backend", name_backend[1]);
  }

  if (acc) {
    acc->initialize(m);
  } else {

    if (xacc::hasContributedService<Accelerator>(name_backend[0])) {

      acc = xacc::getContributedService<Accelerator>(name_backend[0]);
      if (acc)
        acc->initialize(m);

    } else {
      error("Invalid Accelerator. Could not find " + name_backend[0] +
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
  return xacc::hasService<Accelerator>(name) ||
         xacc::hasContributedService<Accelerator>(name);
}

std::shared_ptr<Compiler> getCompiler(const std::string &name,
                                      const xacc::HeterogeneousMap &&options) {
  auto compiler = getCompiler(name);
  compiler->setExtraOptions(options);
  return compiler;
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

  auto a = xacc::getService<Algorithm>(name, false);

  if (!a) {
    if (xacc::hasContributedService<Algorithm>(name)) {
      a = xacc::getContributedService<Algorithm>(name);

    } else {
      error("Invalid Algorithm. Could not find " + name +
            " in Service Registry.");
    }
  }
  return a;
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
  std::shared_ptr<Optimizer> t;
  if (xacc::hasService<Optimizer>(name)) {
    t = xacc::getService<Optimizer>(name, false);
  } else if (xacc::hasContributedService<Optimizer>(name)) {
    t = xacc::getContributedService<Optimizer>(name, false);
  }

  if (!t) {
    xacc::error("Invalid Optimizer name, not in service registry - " + name);
  }
  return t;
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

std::shared_ptr<IRTransformation> getIRTransformation(const std::string &name) {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }

  std::shared_ptr<IRTransformation> t;
  if (xacc::hasService<IRTransformation>(name)) {
    t = xacc::getService<IRTransformation>(name, false);
  } else if (xacc::hasContributedService<IRTransformation>(name)) {
    t = xacc::getContributedService<IRTransformation>(name, false);
  }

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

std::shared_ptr<CompositeInstruction>
createComposite(const std::string &name, HeterogeneousMap &&options) {
  auto comp = std::dynamic_pointer_cast<xacc::CompositeInstruction>(
      xacc::getService<xacc::Instruction>(name));
  const bool success = comp->expand(options);
  if (!success) {
    xacc::error("Error in creating " + name +
                " composite instruction. Perhaps invalid parameter set.");
  }
  return comp;
}
namespace ir {
std::shared_ptr<CompositeInstruction>
asComposite(std::shared_ptr<Instruction> inst) {
  auto comp = std::dynamic_pointer_cast<CompositeInstruction>(inst);
  if (!comp) {
    error("Invalid conversion of Instruction to CompositeInstruction.");
  }
  return comp;
}
std::shared_ptr<Instruction>
asInstruction(std::shared_ptr<CompositeInstruction> comp) {
  auto inst = std::dynamic_pointer_cast<Instruction>(comp);
  if (!inst) {
    error("Invalid conversion of CompositeInstruction to Instruction.");
  }
  return inst;
}
} // namespace ir

namespace external {
void load_external_language_plugins() {
  auto loaders = xacc::getServices<ExternalLanguagePluginLoader>();
  for (auto &loader : loaders) {
    if (!loader->load()) {
      xacc::warning("[xacc::external] Warning, could not load " +
                    loader->name() + " external language plugin.");
    }
  }
}
void unload_external_language_plugins() {
  auto loaders = xacc::getServices<ExternalLanguagePluginLoader>();
  for (auto &loader : loaders) {
    if (!loader->unload()) {
      xacc::warning(
          "[xacc::external] Warning (maybe an error), could not unload " +
          loader->name() + " external language plugin.");
    }
  }
}
} // namespace external

void Finalize() {
  XACCLogger::instance()->dumpQueue();
  if (xaccFrameworkInitialized) {
    // Execute tearDown() for all registered TearDown services.
    auto tearDowns = xacc::getServices<TearDown>();
    debug("Tearing down " + std::to_string(tearDowns.size()) +
          " registered TearDown services..");
    std::vector<std::shared_ptr<TearDown>> frameworkTearDowns;
    for (auto &td : tearDowns) {
      try {
        // If this is XACC HPC TearDown,
        // add it to the list of XACC internal TearDown services to be executed
        // after all plugins' TearDowns.
        if (td->name() == "xacc-hpc-virt") {
          frameworkTearDowns.emplace_back(td);
        } else {
          td->tearDown();
        }
      } catch (int exception) {
        xacc::error("Error while tearing down a service. Code: " +
                    std::to_string(exception));
      }
    }
    // Runs XACC internal TearDowns
    for (auto &td : frameworkTearDowns) {
      td->tearDown();
    }

    xacc::xaccFrameworkInitialized = false;
    compilation_database.clear();
    allocated_buffers.clear();
    xacc::ServiceAPI_Finalize();
  }
}

} // namespace xacc
