/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#ifndef QUANTUM_GATE_ACCELERATORS_IBMACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_IBMACCELERATOR_HPP_

#include "InstructionIterator.hpp"
#include "RemoteAccelerator.hpp"
#include <bitset>
#include <type_traits>

using namespace xacc;

namespace xacc {
namespace quantum {

#define IS_INTEGRAL(T)                                                         \
  typename std::enable_if<std::is_integral<T>::value>::type * = 0

template <class T>
std::string integral_to_binary_string(T byte, IS_INTEGRAL(T)) {
  std::bitset<sizeof(T) * 8> bs(byte);
  return bs.to_string();
}

std::string hex_string_to_binary_string(std::string hex);
/**
 * Wrapper for information related to the remote
 * IBM Backend.
 */
struct IBMBackend {
  std::string name;
  std::string description;
  int nQubits;
  std::vector<std::pair<int, int>> couplers;
  bool status = true;
  bool isSimulator = true;
  std::string gateSet = "";
  std::string basisGates = "";
  std::vector<double> readoutErrors;
  std::vector<double> gateErrors;
  std::vector<std::string> multiQubitGates;
  std::vector<double> multiQubitGateErrors;
  std::vector<double> T1s;
  std::vector<double> T2s;
  std::vector<double> frequencies;
};

/**
 * The IBMAccelerator is a QPUGate Accelerator that
 * provides an execute implementation that maps XACC IR
 * to an equivalent OpenQasm string, and executes it on the
 * IBM QPU.
 *
 */
class IBMAccelerator : public RemoteAccelerator {
public:

  void cancel() override;

  std::map<std::string, std::map<int, int>> name2QubitMap;
  /**
   * Create, store, and return an AcceleratorBuffer with the given
   * variable id string and of the given number of bits.
   * The string id serves as a unique identifier
   * for future lookups and reuse of the AcceleratorBuffer.
   *
   * @param varId
   * @param size
   * @return
   */
  std::shared_ptr<AcceleratorBuffer> createBuffer(const std::string &varId,
                                                  const int size) override;

  /**
   * Create, store, and return an AcceleratorBuffer with the given
   * variable id string. This method returns all available
   * qubits for this Accelerator. The string id serves as a unique identifier
   * for future lookups and reuse of the AcceleratorBuffer.
   *
   * @param varId The variable name of the created buffer
   * @return buffer The buffer instance created.
   */
  std::shared_ptr<AcceleratorBuffer>
  createBuffer(const std::string &varId) override;
  /**
   * Initialize this Accelerator. This method is called
   * by the XACC framework after an Accelerator has been
   * requested and created. Perform any work you need
   * done before execution here.
   *
   */
  void initialize(AcceleratorParameters params = {}) override;


  /**
   * Return the graph structure for this Accelerator.
   *
   * @return connectivityGraph The graph structure of this Accelerator
   */
  std::vector<std::pair<int, int>> getAcceleratorConnectivity() override;

  /**
   * Return true if this Accelerator can allocated
   * NBits number of bits.
   * @param NBits
   * @return
   */
  bool isValidBufferSize(const int NBits) override;

  /**
   * This Accelerator models QPU Gate accelerators.
   * @return
   */
  AcceleratorType getType() override { return AcceleratorType::qpu_gate; }

  /**
   * We have no need to transform the IR for this Accelerator,
   * so return an empty list, for now.
   * @return
   */
  std::vector<std::shared_ptr<IRTransformation>>
  getIRTransformations() override;

  /**
   * Return all relevant IBMAccelerator runtime options.
   * Users can set the api-key, execution type, and number of triels
   * from the command line with these options.
   */
  OptionPairs getOptions() override {
    OptionPairs desc{
        {"ibm-api-key", "Provide the IBM API key. This is used if "
                        "$HOME/.ibm_config is not found"},
        {"ibm-api-url",
         "Specify the IBM Quantum Experience URL, overrides $HOME/.ibm_config "
         "."},
        {"ibm-backend", "Provide the backend name."},
        {"ibm-shots", "Provide the number of shots to execute."},
        {"ibm-list-backends",
         "List the available backends at the IBM Quantum Experience URL."},
        {"ibm-print-queue",
         "Print the status of the queue for the given backend"}};
    return desc;
  }

  /**
   * Given user-input command line options, perform
   * some operation. Returns true if runtime should exit,
   * false otherwise.
   *
   * @param map The mapping of options to values
   * @return exit True if exit, false otherwise
   */
  bool handleOptions(const std::map<std::string, std::string> &map) override {
    if (map.count("ibm-list-backends")) {
      initialize();
      XACCLogger::instance()->enqueueLog("");
      for (auto s : availableBackends) {
        XACCLogger::instance()->enqueueLog(
            "Available IBM Backend: " + std::string(s.first) + " [" +
            (s.second.status ? "on" : "off") + "]");
      }
      return true;
    } else if (map.count("ibm-print-queue")) {
      initialize();
      auto backend = map.at("ibm-print-queue");
      XACCLogger::instance()->enqueueLog("");
      XACCLogger::instance()->enqueueLog(
          "Queue Status for " + backend + ": " +
          handleExceptionRestClientGet(remoteUrl, "/api/Backends/" + backend +
                                                      "/queue/status"));
      return true;
    }
    return false;
  }

  const std::vector<double> getOneBitErrorRates() override;
  const std::vector<std::pair<std::pair<int, int>, double>>
  getTwoBitErrorRates() override;

  /**
   * Return the name of this instance.
   *
   * @return name The string name
   */
  const std::string name() const override { return "ibm"; }

  /**
   * Return the description of this instance
   * @return description The description of this object.
   */
  const std::string description() const override {
    return "The IBM Accelerator interacts with the remote IBM "
           "Quantum Experience to launch XACC quantum kernels.";
  }

  /**
   * take ir, generate json post string
   */
  const std::string
  processInput(std::shared_ptr<AcceleratorBuffer> buffer,
               std::vector<std::shared_ptr<Function>> functions) override;

  /**
   * take response and create
   */
  std::vector<std::shared_ptr<AcceleratorBuffer>>
  processResponse(std::shared_ptr<AcceleratorBuffer> buffer,
                  const std::string &response) override;

  IBMAccelerator() : RemoteAccelerator() {}

  IBMAccelerator(std::shared_ptr<Client> client) : RemoteAccelerator(client) {}

  virtual bool isPhysical();

  /**
   * The destructor
   */
  virtual ~IBMAccelerator() {}

private:

  /**
   * Private utility to search for the IBM
   * API key in $HOME/.ibm_config, $IBM_CONFIG,
   * or --ibm-api-key command line arg
   */
  void searchAPIKey(std::string &key, std::string &url, std::string &hub,
                    std::string &group, std::string &project);

  /**
   * Private utility to search for key in the config
   * file.
   */
  void findApiKeyInFile(std::string &key, std::string &url, std::string &hub,
                        std::string &group, std::string &project,
                        const std::string &p);

  /**
   * Reference to the temporary API Token for
   * this IBM Quantum Experience session.
   */
  std::string currentApiToken;

  /**
   * The IBM Quantum Experience URL
   */
  std::string url;

  std::string hub;
  std::string group;
  std::string project;

  bool jobIsRunning = false;
  std::string currentJobId = "";
  /**
   * Mapping of available backend name to an actual
   * IBMBackend struct data structure.
   */
  std::map<std::string, IBMBackend> availableBackends;

  IBMBackend chosenBackend;

  bool initialized = false;
};

} // namespace quantum
} // namespace xacc

#endif
