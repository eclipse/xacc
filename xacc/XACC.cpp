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
#include "XACC.hpp"
#include "InstructionIterator.hpp"
#include "IRProvider.hpp"
#include "IRGenerator.hpp"
#include <signal.h>
#include <cstdlib>

namespace xacc {

bool isPyApi = false;
bool xaccFrameworkInitialized = false;
std::shared_ptr<CLIParser> xaccCLParser = std::make_shared<CLIParser>();
std::shared_ptr<ServiceRegistry> serviceRegistry =
    std::make_shared<ServiceRegistry>();

int argc = 0;
char **argv = NULL;

int getArgc() { return argc; }
char **getArgv() { return argv; }
void Initialize(std::vector<std::string> argv) {
  XACCLogger::instance()->info("Initializing the XACC Framework");
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

void Initialize(int arc, char **arv) {

  if (!xaccFrameworkInitialized) {
    argc = arc;
    argv = arv;

    // Do a little preprocessing on the
    // command line args to see if we have
    // been given a custom internal plugin path
    using namespace boost::program_options;
    variables_map vm;
    options_description desc;
    desc.add_options()("internal-plugin-path", value<std::string>(), "");
    store(command_line_parser(argc, argv)
              .options(desc)
              .allow_unregistered()
              .run(),
          vm);
    notify(vm);

    XACCLogger::instance()->enqueueLog("Creating XACC ServiceRegistry");
    try {
      XACCLogger::instance()->enqueueLog("Initializing the ServiceRegistry");
      if (vm.count("internal-plugin-path")) {
        serviceRegistry->initialize(
            vm["internal-plugin-path"].as<std::string>());
      } else {
        serviceRegistry->initialize();
      }
    } catch (std::exception &e) {
      XACCLogger::instance()->error(
          "Failure initializing XACC Plugin Registry - " +
          std::string(e.what()));
      Finalize();
      exit(0);
    }

    // Parse any user-supplied command line options
    xaccCLParser->parse(argc, argv, *serviceRegistry);

    XACCLogger::instance()->enqueueLog("[xacc] Initializing XACC Framework.");

    // Check that we have some
    auto s = serviceRegistry->getServices<Compiler>().size();
    auto a = serviceRegistry->getServices<Accelerator>().size();

    XACCLogger::instance()->enqueueLog(
        "[xacc::plugins] XACC has " + std::to_string(s) + " Compiler" +
        ((s == 1 || s == 0) ? "" : "s") + " available.");
    XACCLogger::instance()->enqueueLog(
        "[xacc::plugins] XACC has " + std::to_string(a) + " Accelerator" +
        ((s == 0 || s == 1) ? "" : "s") + " available.");

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
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
  if (isPyApi) {
    throw std::runtime_error(msg);
  } else {
    XACCLogger::instance()->error(msg, predicate);
    XACCLogger::instance()->error("Framework Exiting", predicate);
    xacc::Finalize();
    exit(-1);
  }
}

void addCommandLineOption(const std::string &optionName,
                          const std::string &optionDescription) {
  xaccCLParser->addStringOption(optionName, optionDescription);
}

void addCommandLineOptions(const std::string &category,
                           const std::map<std::string, std::string> &options) {
  xaccCLParser->addStringOptions(category, options);
}

void addCommandLineOptions(std::shared_ptr<options_description> options) {
  xaccCLParser->addOptionsDescription(options);
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
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }
  if (!optionExists("accelerator")) {
    error("Invalid use of XACC API. getAccelerator() with no string argument "
          "requires that you set --accelerator at the command line.");
  }

  auto acc = serviceRegistry->getService<Accelerator>(getOption("accelerator"));
  if (acc) {
    acc->initialize();
  } else {
    error("Invalid Accelerator. Could not find " + getOption("accelerator") +
          " in Accelerator Registry.");
  }
  return acc;
}

std::shared_ptr<Accelerator> getAccelerator(const std::string &name) {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }
  auto acc = serviceRegistry->getService<Accelerator>(name);
  if (acc) {
    acc->initialize();
  } else {
    error("Invalid Accelerator. Could not find " + name +
          " in Accelerator Registry.");
  }
  return acc;
}

bool hasAccelerator(const std::string &name) {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }
  return serviceRegistry->hasService<Accelerator>(name);
}

std::shared_ptr<Compiler> getCompiler(const std::string &name) {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }
  auto c = serviceRegistry->getService<Compiler>(name);
  if (!c) {
    error("Invalid Compiler. Could not find " + name + " in Service Registry.");
  }
  return c;
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
  auto compiler = serviceRegistry->getService<Compiler>(getOption("compiler"));
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

  return serviceRegistry->hasService<Compiler>(name);
}

std::shared_ptr<IRTransformation>
getIRTransformations(const std::string &name) {
  if (!xacc::xaccFrameworkInitialized) {
    error("XACC not initialized before use. Please execute "
          "xacc::Initialize() before using API.");
  }
  auto t = serviceRegistry->getService<IRTransformation>(name);
  if (!t) {
    error("Invalid IRTransformation. Could not find " + name +
          " in Service Registry.");
  }

  return t;
}
const std::string translate(std::shared_ptr<Function> function,
                            const std::string toLanguage) {
  auto toLanguageCompiler = getCompiler(toLanguage);
  return toLanguageCompiler->translate("", function);
}

const std::string translateWithVisitor(const std::string &originalSource,
                                       const std::string &originalLanguage,
                                       const std::string &visitorMappingName,
                                       const std::string &accelerator,
                                       const int kernel) {

  auto acc = getAccelerator(accelerator);
  auto originalCompiler = getCompiler(originalLanguage);
  auto ir = originalCompiler->compile(originalSource, acc);
  auto visitor =
      serviceRegistry->getService<BaseInstructionVisitor>(visitorMappingName);

  std::vector<std::string> previouslySeenKernels;

  InstructionIterator it(ir->getKernels()[kernel]);
  while (it.hasNext()) {
    // Get the next node in the tree
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
    }
  }

  return visitor->toString();
}

void analyzeBuffer(std::shared_ptr<AcceleratorBuffer> buffer) {
  if (!buffer->hasExtraInfoKey("ir-generator")) {
    error("xacc::analyzeBuffer is for use with codes generated with an "
          "IRGenerator.");
  }

  auto gen = getService<IRGenerator>(
      boost::get<std::string>(buffer->getInformation("ir-generator")));
  gen->analyzeResults(buffer);
}

void clearOptions() { RuntimeOptions::instance()->clear(); }

std::shared_ptr<Function> optimizeFunction(const std::string optimizer,
                                           std::shared_ptr<Function> function) {
  auto ir = getService<IRProvider>("gate")->createIR();
  ir->addKernel(function);
  auto opt = getService<IRTransformation>(optimizer);
  auto newir = opt->transform(ir);
  auto optF = newir->getKernels()[0];
  return optF->enabledView();
}

/**
 * This method should be called by clients to
 * clean up and finalize the XACC framework. It should
 * be called after using the XACC API.
 */
void Finalize() {
  XACCLogger::instance()->dumpQueue();
  info("");
  info("[xacc::plugins] Cleaning up Plugin Registry.");
  //	xacc::serviceRegistry->destroy();
  xacc::xaccFrameworkInitialized = false;
  info("[xacc] Finalizing XACC Framework.");
}
} // namespace xacc
