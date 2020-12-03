#include "aer_python_adapter.hpp"
#include <iostream>
#include <numeric>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <dlfcn.h>
#include "xacc_config.hpp"

namespace xacc {
namespace aer {
std::string runPulseSim(const std::string &hamJsonStr, double dt,
                        const std::vector<double> &freqEst,
                        const std::vector<int> &uChanLoRefs,
                        const std::string &qObjJson) {
  static bool PythonInit = false;
  if (!PythonInit) {
    if (!XACC_IS_APPLE) {
      // If not MacOs, preload Python lib to the address space.
      // Note: we don't need to dlclose, just need to load the lib to prevent
      // linking issue on Linux.
      auto libPythonPreload =
          dlopen("@PYTHON_LIB_NAME@", RTLD_LAZY | RTLD_GLOBAL);
    }
    pybind11::initialize_interpreter();
    PythonInit = true;
  }
  auto py_src = R"#(
import json, warnings
import numpy as np
from qiskit.providers.aer.pulse.system_models.hamiltonian_model import HamiltonianModel
from qiskit.providers.aer.pulse.system_models.pulse_system_model import PulseSystemModel
from qiskit.providers.aer.backends import PulseSimulator
from qiskit import qobj, pulse
from qiskit.compiler import assemble
from qiskit.qobj.converters import QobjToInstructionConverter 
from qiskit.providers.models.backendconfiguration import UchannelLO
warnings.filterwarnings("ignore", category=RuntimeWarning, module="qiskit")
# Parse inputs to construct PulseSystemModel
ham_dict = json.loads(locals()["ham_json"])
ham_model = HamiltonianModel.from_dict(ham_dict)
u_channel_lo = []
for u_chan_ref in locals()["u_channel_lo_ref"]:  
    u_channel_lo.append([UchannelLO(int(u_chan_ref), 1.0+0.0j)])

system_model = PulseSystemModel(hamiltonian=ham_model,
                                u_channel_lo=u_channel_lo,
                                subsystem_list=locals()["subsystem_list"],
                                dt=locals()["dt"],
                                qubit_freq_est=locals()["qubit_freq_est"])
backend_sim = PulseSimulator()
qobjDict = json.loads(locals()["qobj_json"])
pulseQobj = qobj.PulseQobj.from_dict(qobjDict)
converter = QobjToInstructionConverter(pulseQobj.config.pulse_library)
run_config = pulseQobj.config.to_dict()
run_config.pop('pulse_library')
qubit_lo_freq = run_config.get('qubit_lo_freq')
if qubit_lo_freq:
    run_config['qubit_lo_freq'] = [freq*1e9 for freq in qubit_lo_freq]
meas_lo_freq = run_config.get('meas_lo_freq')
if meas_lo_freq:
    run_config['meas_lo_freq'] = [freq*1e9 for freq in meas_lo_freq] 
programs = []
for program in pulseQobj.experiments:
    insts = []
    for inst in program.instructions:        
        insts.append(converter(inst))
    schedule = pulse.Schedule(*insts)
    programs.append(schedule)

# Reconstruct Pulse Obj for the AER simulator
pulse_qobj = assemble(programs,
                        backend=backend_sim,
                        meas_level=run_config['meas_level'],
                        meas_return=run_config['meas_return'],
                        memory_slots=run_config['memory_slots'],
                        shots=run_config['shots'],
                        meas_lo_freq=run_config['meas_lo_freq'])
result = backend_sim.run(pulse_qobj, system_model=system_model).result().to_dict()
# Set output (JSON of bitstring -> count map)
hex_to_count = result["results"][0]["data"]["counts"]
for hex_val in hex_to_count:
    hex_to_count[hex_val] = int(hex_to_count[hex_val])
count_json = json.dumps(hex_to_count)
)#";
  // Check if Qiskit present.
  try {
    pybind11::module::import("qiskit");
  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
    std::cerr << "Qiskit is not installed. Please install Qiskit to use the "
                 "Aer Pulse simulator.\n";
    throw;
  }

  // Set variables:
  auto locals = pybind11::dict();
  locals["ham_json"] = hamJsonStr;
  locals["dt"] = dt;
  locals["qubit_freq_est"] = freqEst;
  locals["u_channel_lo_ref"] = uChanLoRefs;

  std::vector<int> subSystemList(freqEst.size());
  std::iota(subSystemList.begin(), subSystemList.end(), 0);
  locals["subsystem_list"] = subSystemList;
  locals["qobj_json"] = qObjJson;
  // Run the simulator:
  pybind11::exec(py_src, pybind11::globals(), locals);
  const auto result = locals["count_json"].cast<std::string>();
  return result;
}
} // namespace aer
} // namespace xacc
