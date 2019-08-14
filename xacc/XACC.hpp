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
#include "IRGenerator.hpp"

#include "Algorithm.hpp"
#include "Optimizer.hpp"

namespace xacc {

namespace constants {
static constexpr double pi = 3.141592653589793238;
}

extern bool xaccFrameworkInitialized;
extern bool isPyApi;

class CLIParser;
extern std::shared_ptr<CLIParser> xaccCLParser;

extern int argc;
extern char **argv;

extern std::map<std::string, std::shared_ptr<Function>> compilation_database;

using AcceleratorBufferPtr = std::shared_ptr<xacc::AcceleratorBuffer>;
class qbit : public AcceleratorBufferPtr{
public:
  qbit(const int n) : AcceleratorBufferPtr(std::make_shared<xacc::AcceleratorBuffer>(n)) {}
  void operator[](const int& i) {}
};
qbit qalloc(const int n);

void Initialize(int argc, char **argv);

void Initialize(std::vector<std::string> argv);
void Initialize();
bool isInitialized();
void PyInitialize(const std::string rootPath);
int getArgc();
char **getArgv();

std::vector<std::string> getIncludePaths();

void setIsPyApi();

void addCommandLineOption(const std::string &optionName,
                          const std::string &optionDescription = "");
void addCommandLineOptions(const std::string &category,
                           const std::map<std::string, std::string> &options);
void addCommandLineOptions(const std::map<std::string, std::string> &options);

void setGlobalLoggerPredicate(MessagePredicate predicate);
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

std::shared_ptr<IRGenerator> getIRGenerator(const std::string &name);

void setAccelerator(const std::string &acceleratorName);
std::shared_ptr<Accelerator> getAccelerator(const std::string &name,
                                            AcceleratorParameters params = {});
std::shared_ptr<Accelerator> getAccelerator(const std::string &name,
                                            std::shared_ptr<Client> client,
                                            AcceleratorParameters params = {});
std::shared_ptr<Accelerator> getAccelerator(AcceleratorParameters params = {});
bool hasAccelerator(const std::string &name);

void setCompiler(const std::string &compilerName);
std::shared_ptr<Compiler> getCompiler(const std::string &name);
std::shared_ptr<Compiler> getCompiler();
bool hasCompiler(const std::string &name);

std::shared_ptr<Algorithm> getAlgorithm(const std::string name,
                                        xacc::AlgorithmParameters &params);
std::shared_ptr<Algorithm> getAlgorithm(const std::string name,
                                        xacc::AlgorithmParameters &&params);
std::shared_ptr<Algorithm> getAlgorithm(const std::string name);

std::shared_ptr<Optimizer> getOptimizer(const std::string name);
std::shared_ptr<Optimizer>
getOptimizer(const std::string name,
             const std::map<std::string, xacc::InstructionParameter> &opts);
std::shared_ptr<Optimizer>
getOptimizer(const std::string name,
             const std::map<std::string, xacc::InstructionParameter> &&opts);

bool hasCache(const std::string fileName, const std::string subdirectory = "");
std::map<std::string, InstructionParameter>
getCache(const std::string fileName, const std::string subdirectory = "");
void appendCache(const std::string fileName, const std::string key,
                 InstructionParameter &param,
                 const std::string subdirectory = "");
void appendCache(const std::string fileName, const std::string key,
                 InstructionParameter &&param,
                 const std::string subdirectory = "");
void appendCache(const std::string fileName,
                 std::map<std::string, InstructionParameter> &params,
                 const std::string subdirectory = "");
const std::string getRootDirectory();

std::shared_ptr<Function> optimizeFunction(const std::string optimizer,
                                           std::shared_ptr<Function> function);

std::shared_ptr<IRTransformation> getIRTransformation(const std::string &name);

const std::string translate(std::shared_ptr<Function> function,
                            const std::string toLanguage);
const std::string translateWithVisitor(const std::string &originalSource,
                                       const std::string &originalLanguage,
                                       const std::string &visitorMapping,
                                       const std::string &accelerator,
                                       const int kernel);

void appendCompiled(std::shared_ptr<Function> function);
std::shared_ptr<Function> getCompiled(const std::string name);
bool hasCompiled(const std::string name);

void qasm(const std::string &qasmString);

void Finalize();

} // namespace xacc

#endif
