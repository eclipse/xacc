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
#ifndef XACC_XACC_HPP_
#define XACC_XACC_HPP_

#include "Compiler.hpp"
#include "RemoteAccelerator.hpp"
#include "IRProvider.hpp"

#include "Algorithm.hpp"
#include "Optimizer.hpp"
#include "IRTransformation.hpp"

#include "heterogeneous.hpp"

#include <memory>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

namespace xacc {

namespace constants {
static constexpr double pi = 3.141592653589793238;
}

extern bool xaccFrameworkInitialized;
extern bool isPyApi;
extern bool verbose;

class CLIParser;
extern std::shared_ptr<CLIParser> xaccCLParser;

extern int argc;
extern char **argv;

extern std::string rootPathString;

extern std::map<std::string, std::shared_ptr<CompositeInstruction>>
    compilation_database;
extern std::map<std::string, std::shared_ptr<AcceleratorBuffer>>
    allocated_buffers;

// The qbit type is critical to qcor
// We want it to be a shared_ptr, but we
// need it to have an operator[]() exposed
// so we can do things like H(q[0])
using AcceleratorBufferPtr = std::shared_ptr<xacc::AcceleratorBuffer>;
class qbit : public AcceleratorBufferPtr {
public:
  qbit() : AcceleratorBufferPtr(std::make_shared<xacc::AcceleratorBuffer>()) {}
  qbit(const int n)
      : AcceleratorBufferPtr(std::make_shared<xacc::AcceleratorBuffer>(n)) {}
  qbit(const AcceleratorBufferPtr &ab) : AcceleratorBufferPtr(ab) {}

  int operator[](const int &i) { return 0; }
  ExtraInfo operator[](const std::string &key) {
    return AcceleratorBufferPtr::get()->getInformation(key);
  }
  qbit &operator=(qbit &q) { return q; }
};
qbit qalloc(const int n);
qbit qalloc();

void Initialize(int argc, char **argv);

void Initialize(std::vector<std::string> argv);
void Initialize();
bool isInitialized();
void PyInitialize(const std::string rootPath);
int getArgc();
char **getArgv();

void set_verbose(bool verbose);

const std::string getRootDirectory();
std::vector<std::string> getIncludePaths();

void setIsPyApi();

void addCommandLineOption(const std::string &optionName,
                          const std::string &optionDescription = "");
void addCommandLineOptions(const std::string &category,
                           const std::map<std::string, std::string> &options);
void addCommandLineOptions(const std::map<std::string, std::string> &options);

void setGlobalLoggerPredicate(MessagePredicate predicate);
void logToFile(bool enable, const std::string &fileNamePrefix = "");
void setLoggingLevel(int level);
int getLoggingLevel();
void subscribeLoggingLevel(LoggingLevelNotification callback);

void info(const std::string &msg,
          MessagePredicate predicate = std::function<bool(void)>([]() {
            return true;
          }));
void warning(const std::string &msg,
             MessagePredicate predicate = std::function<bool(void)>([]() {
               return true;
             }));
void debug(const std::string &msg,
           MessagePredicate predicate = std::function<bool(void)>([]() {
             return true;
           }));
void error(const std::string &msg,
           MessagePredicate predicate = std::function<bool(void)>([]() {
             return true;
           }));

void clearOptions();
bool optionExists(const std::string &optionKey);
const std::string getOption(const std::string &optionKey);
void setOption(const std::string &optionKey, const std::string &value);
void unsetOption(const std::string &optionKey);

std::shared_ptr<IRProvider> getIRProvider(const std::string &name);

void storeBuffer(std::shared_ptr<AcceleratorBuffer> buffer);
void storeBuffer(const std::string name,
                 std::shared_ptr<AcceleratorBuffer> buffer);
std::shared_ptr<AcceleratorBuffer> getBuffer(const std::string &name);
bool hasBuffer(const std::string &name);
std::shared_ptr<AcceleratorBuffer>
getClassicalRegHostBuffer(const std::string &cRegName);

void setAccelerator(const std::string &acceleratorName);
std::shared_ptr<Accelerator>
getAccelerator(const std::string &name, const HeterogeneousMap &params = {});
std::shared_ptr<Accelerator>
getAccelerator(const std::string &name, std::shared_ptr<Client> client,
               const HeterogeneousMap &params = {});
std::shared_ptr<Accelerator> getAccelerator();
std::shared_ptr<Accelerator>
getAcceleratorDecorator(const std::string &decorator,
                        std::shared_ptr<Accelerator> acc,
                        const HeterogeneousMap &params = {});

bool hasAccelerator(const std::string &name);

void setCompiler(const std::string &compilerName);
std::shared_ptr<Compiler> getCompiler(const std::string &name, const xacc::HeterogeneousMap&& options);
std::shared_ptr<Compiler> getCompiler(const std::string &name);
std::shared_ptr<Compiler> getCompiler();
bool hasCompiler(const std::string &name);

std::shared_ptr<Algorithm> getAlgorithm(const std::string name,
                                        const xacc::HeterogeneousMap &params);
std::shared_ptr<Algorithm> getAlgorithm(const std::string name,
                                        const xacc::HeterogeneousMap &&params);
std::shared_ptr<Algorithm> getAlgorithm(const std::string name);

std::shared_ptr<Optimizer> getOptimizer(const std::string name);
std::shared_ptr<Optimizer> getOptimizer(const std::string name,
                                        const HeterogeneousMap &opts);
std::shared_ptr<Optimizer> getOptimizer(const std::string name,
                                        const HeterogeneousMap &&opts);

bool hasCache(const std::string fileName, const std::string subdirectory = "");

HeterogeneousMap getCache(const std::string fileName,
                          const std::string subdirectory = "");
void appendCache(const std::string fileName, HeterogeneousMap &params,
                 const std::string subdirectory = "");
template <typename T>
void appendCache(const std::string fileName, const std::string key, T &&param,
                 const std::string subdirectory = "") {
  auto rootPathStr = xacc::getRootDirectory();
  if (!subdirectory.empty()) {
    rootPathStr += "/" + subdirectory;
    if (!xacc::directoryExists(rootPathStr)) {
      auto status =
          mkdir(rootPathStr.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
  }
  // Check if file exists
  if (xacc::fileExists(rootPathStr + "/" + fileName)) {
    auto existingCache = getCache(fileName, subdirectory);
    existingCache.insert(key, param);

    appendCache(fileName, existingCache, subdirectory);
  } else {
    std::ofstream out(rootPathStr + "/" + fileName);
    std::stringstream s;
    AcceleratorBuffer b;
    b.useAsCache();

    HeterogeneousMap params(std::make_pair(key, param));
    std::map<std::string, ExtraInfo> einfo;
    HeterogenousMap2ExtraInfo h2ei(einfo);
    params.visit(h2ei);

    b.addExtraInfo(key, einfo[key]);

    b.print(s);

    out << s.str();
    out.close();
  }
}

template <typename T>
void appendCache(const std::string fileName, const std::string key, T &param,
                 const std::string subdirectory = "") {
  auto rootPathStr = xacc::getRootDirectory();
  if (!subdirectory.empty()) {
    rootPathStr += "/" + subdirectory;
    if (!xacc::directoryExists(rootPathStr)) {
      auto status =
          mkdir(rootPathStr.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
  }
  // Check if file exists
  if (xacc::fileExists(rootPathStr + "/" + fileName)) {
    auto existingCache = getCache(fileName, subdirectory);
    existingCache.insert(key, param);

    appendCache(fileName, existingCache, subdirectory);
  } else {
    std::ofstream out(rootPathStr + "/" + fileName);
    std::stringstream s;
    AcceleratorBuffer b;
    b.useAsCache();

    HeterogeneousMap params(std::make_pair(key, param));
    std::map<std::string, ExtraInfo> einfo;
    HeterogenousMap2ExtraInfo h2ei(einfo);
    params.visit(h2ei);

    b.addExtraInfo(key, einfo[key]);

    b.print(s);

    out << s.str();
    out.close();
  }
}

std::shared_ptr<IRTransformation> getIRTransformation(const std::string &name);

std::shared_ptr<CompositeInstruction>
createComposite(const std::string &name, HeterogeneousMap &&options);

const std::string
translate(std::shared_ptr<CompositeInstruction> CompositeInstruction,
          const std::string toLanguage);

void appendCompiled(std::shared_ptr<CompositeInstruction> composite,
                    bool _override = true);
std::shared_ptr<CompositeInstruction> getCompiled(const std::string name);
bool hasCompiled(const std::string name);

void qasm(const std::string &qasmString);
namespace external {
class ExternalLanguagePluginLoader : public Identifiable {
public:
  virtual bool load() = 0;
  virtual bool unload() = 0;
};
void load_external_language_plugins();
void unload_external_language_plugins();
} // namespace external

namespace ir {
std::shared_ptr<CompositeInstruction>
asComposite(std::shared_ptr<Instruction> inst);
std::shared_ptr<Instruction>
asInstruction(std::shared_ptr<CompositeInstruction> comp);
} // namespace ir
void Finalize();

} // namespace xacc

#endif
