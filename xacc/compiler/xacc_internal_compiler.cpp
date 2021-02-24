#include "xacc_internal_compiler.hpp"
#include "Instruction.hpp"
#include "Utils.hpp"
#include "heterogeneous.hpp"
#include "config_file_parser.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "InstructionIterator.hpp"
#include <CompositeInstruction.hpp>
#include <stdlib.h>

namespace xacc {
namespace internal_compiler {
std::shared_ptr<Accelerator> qpu = nullptr;
std::shared_ptr<CompositeInstruction> lastCompiled = nullptr;
bool __execute = true;
std::vector<HeterogeneousMap> current_runtime_arguments = {};

void __set_verbose(bool v) { xacc::set_verbose(v); }
void compiler_InitializeXACC(const char *qpu_backend,
                             const std::vector<std::string> cmd_line_args) {
  if (!xacc::isInitialized()) {
    xacc::Initialize(cmd_line_args);
    xacc::external::load_external_language_plugins();
    auto at_exit = []() { xacc::Finalize(); };
    atexit(at_exit);
  }
  setAccelerator(qpu_backend);
}

void compiler_InitializeXACC(const char *qpu_backend, int shots) {
  if (!xacc::isInitialized()) {
    xacc::Initialize();
    xacc::external::load_external_language_plugins();
    auto at_exit = []() { xacc::Finalize(); };
    atexit(at_exit);
  }

  setAccelerator(qpu_backend, shots);
}
auto process_qpu_backend_str = [](const std::string &qpu_backend_str)
    -> std::pair<std::string, HeterogeneousMap> {
  bool has_extra_config = qpu_backend_str.find("[") != std::string::npos;
  auto qpu_name =
      (has_extra_config)
          ? qpu_backend_str.substr(0, qpu_backend_str.find_first_of("["))
          : qpu_backend_str;

  HeterogeneousMap options;
  if (has_extra_config) {
    auto first = qpu_backend_str.find_first_of("[");
    auto second = qpu_backend_str.find_first_of("]");
    auto qpu_config = qpu_backend_str.substr(first + 1, second - first - 1);

    auto key_values = split(qpu_config, ',');
    for (auto key_value : key_values) {
      auto tmp = split(key_value, ':');
      auto key = tmp[0];
      auto value = tmp[1];
      if (key == "qcor_qpu_config") {
        // If the config is provided in a file:
        // We could check for file extension here to
        // determine a parsing plugin.
        // Currently, we only support a simple key-value format (INI like).
        // e.g.
        // String like config:
        // name=XACC
        // Boolean configs
        // true_val=true
        // false_val=false
        // Array/vector configs
        // array=[1,2,3]
        // array_double=[1.0,2.0,3.0]
        auto parser = xacc::getService<ConfigFileParsingUtil>("ini");
        options = parser->parse(value);
      } else {
        // check if int first, then double,
        // finally just throw it in as a string
        try {
          auto i = std::stoi(value);
          options.insert(key, i);
        } catch (std::exception &e) {
          try {
            auto d = std::stod(value);
            options.insert(key, d);
          } catch (std::exception &e) {
            options.insert(key, value);
          }
        }
      }
    }
  }

  return std::make_pair(qpu_name, options);
};

void setAccelerator(const char *qpu_backend) {

  if (qpu) {
    if (qpu_backend != qpu->name()) {
      const auto [qpu_name, config] = process_qpu_backend_str(qpu_backend);
      qpu = xacc::getAccelerator(qpu_name, config);
    }
  } else {
    const auto [qpu_name, config] = process_qpu_backend_str(qpu_backend);
    qpu = xacc::getAccelerator(qpu_name, config);
  }
}

void setAccelerator(const char *qpu_backend, int shots) {
  if (qpu) {
    if (qpu_backend != qpu->name()) {
      auto [qpu_name, config] = process_qpu_backend_str(qpu_backend);
      config.insert("shots", shots);
      qpu = xacc::getAccelerator(qpu_backend, config);
    }
  } else {
    auto [qpu_name, config] = process_qpu_backend_str(qpu_backend);
    config.insert("shots", shots);
    qpu = xacc::getAccelerator(qpu_backend, {std::make_pair("shots", shots)});
  }
}

std::shared_ptr<Accelerator> get_qpu() { return qpu; }

std::shared_ptr<CompositeInstruction> getLastCompiled() { return lastCompiled; }

// Map kernel source string representing a single
// kernel function to a single CompositeInstruction (src to IR)
std::shared_ptr<CompositeInstruction> compile(const char *compiler_name,
                                              const char *kernel_src) {
  auto compiler = xacc::getCompiler(compiler_name);
  auto IR = compiler->compile(kernel_src, qpu);
  auto program = IR->getComposites()[0];
  lastCompiled = program;
  return program;
}

std::shared_ptr<CompositeInstruction> getCompiled(const char *kernel_name) {
  return xacc::hasCompiled(kernel_name) ? xacc::getCompiled(kernel_name)
                                        : nullptr;
}

// Run quantum compilation routines on IR
void optimize(std::shared_ptr<CompositeInstruction> program,
              const OptLevel opt) {

  xacc::info("[InternalCompiler] Pre-optimization, circuit has " +
             std::to_string(program->nInstructions()) + " instructions.");

  if (opt == DEFAULT) {
    auto optimizer = xacc::getIRTransformation("circuit-optimizer");
    optimizer->apply(program, qpu);
  } else {
    xacc::error("Other Optimization Levels not yet supported.");
  }

  xacc::info("[InternalCompiler] Post-optimization, circuit has " +
             std::to_string(program->nInstructions()) + " instructions.");
}

void execute(AcceleratorBuffer *buffer,
             std::vector<std::shared_ptr<CompositeInstruction>> programs) {

  qpu->execute(xacc::as_shared_ptr(buffer), programs);
}

// Execute on the specified QPU, persisting results to
// the provided buffer.
void execute(AcceleratorBuffer *buffer,
             std::shared_ptr<CompositeInstruction> program,
             double *parameters) {

  std::shared_ptr<CompositeInstruction> program_as_shared;
  if (parameters) {
    std::vector<double> values(parameters, parameters + program->nVariables());
    program = program->operator()(values);
  }
  auto buffer_as_shared = std::shared_ptr<AcceleratorBuffer>(
      buffer, xacc::empty_delete<AcceleratorBuffer>());

  qpu->execute(buffer_as_shared, program);
}

void execute(AcceleratorBuffer **buffers, const int nBuffers,
             std::shared_ptr<CompositeInstruction> program,
             double *parameters) {

  //  Should take vector of buffers, and we collapse them
  //  into a single unified buffer for execution, then set the
  //  measurement counts accordingly in postprocessing.

  std::vector<AcceleratorBuffer *> bvec(buffers, buffers + nBuffers);
  std::vector<std::string> buffer_names;
  for (auto &a : bvec)
    buffer_names.push_back(a->name());

  // Do we have any unknown ancilla bits?
  std::vector<std::string> possible_extra_buffers;
  int possible_size = -1;
  InstructionIterator it(program);
  while (it.hasNext()) {
    auto &next = *it.next();
    auto bnames = next.getBufferNames();
    for (auto &bb : bnames) {
      if (!xacc::container::contains(buffer_names, bb) &&
          !xacc::container::contains(possible_extra_buffers, bb)) {
        // we have an unknown buffer with name bb, need to figure out its size
        // too
        possible_extra_buffers.push_back(bb);
      }
    }
  }

  for (auto &possible_buffer : possible_extra_buffers) {
    std::set<std::size_t> sizes;
    InstructionIterator it2(program);
    while (it2.hasNext()) {
      auto &next = *it2.next();
      for (auto &bit : next.bits()) {
        sizes.insert(bit);
      }
    }
    auto size = *std::max_element(sizes.begin(), sizes.end());
    auto extra = qalloc(size);
    extra->setName(possible_buffer);
    xacc::debug("[xacc_internal_compiler] Adding extra buffer " +
                possible_buffer + " of size " + std::to_string(size));
    bvec.push_back(extra.get());
  }

  // Merge the buffers. Keep track of buffer_name to shift in
  // all bit indices operating on that buffer_name, a map of
  // buffer names to the buffer ptr, and start a map to collect
  // buffer names to measurement counts
  int global_reg_size = 0;
  std::map<std::string, int> shift_map;
  std::vector<std::string> shift_map_names;
  std::vector<int> shift_map_shifts;
  std::map<std::string, AcceleratorBuffer *> buf_map;
  std::map<std::string, std::map<std::string, int>> buf_counts;
  for (auto &b : bvec) {
    shift_map.insert({b->name(), global_reg_size});
    shift_map_names.push_back(b->name());
    shift_map_shifts.push_back(global_reg_size);
    buf_map.insert({b->name(), b});
    buf_counts.insert({b->name(), {}});
    global_reg_size += b->size();
  }

  xacc::debug("[xacc_internal_compiler] Creating register of size " +
              std::to_string(global_reg_size));
  auto tmp = xacc::qalloc(global_reg_size);

  // Update Program bit indices based on new global
  // qubit register
  InstructionIterator iter(program);
  while (iter.hasNext()) {
    auto &next = *iter.next();
    std::vector<std::size_t> newBits;
    int counter = 0;
    for (auto &b : next.bits()) {
      auto buffer_name = next.getBufferName(counter);
      auto shift = shift_map[buffer_name];
      newBits.push_back(b + shift);
      counter++;
    }
    next.setBits(newBits);
    // FIXME Update buffer_names here too
    std::vector<std::string> unified_buf_names;
    for (int j = 0; j < next.nRequiredBits(); j++) {
      unified_buf_names.push_back("q");
    }
    next.setBufferNames(unified_buf_names);
  }

  std::vector<std::size_t> measure_idxs;
  InstructionIterator iter2(program);
  while (iter2.hasNext()) {
    auto &next = *iter2.next();
    if (next.name() == "Measure") {
      measure_idxs.push_back(next.bits()[0]);
    }
  }

  // Now execute using the global merged register
  execute(tmp.get(), program, parameters);

  // Take bit strings and map to buffer individual bit strings
  for (auto &kv : tmp->getMeasurementCounts()) {
    auto bitstring = kv.first;

    // Some backends return bitstring of size = number of measures
    // instead of size = global_reg_size, adjust if so
    if (bitstring.size() == measure_idxs.size()) {
      std::string tmps = "";
      for (int j = 0; j < global_reg_size; j++)
        tmps += "0";

      for (int j = 0; j < measure_idxs.size(); j++) {
        tmps[measure_idxs[j]] = bitstring[j];
      }

      bitstring = tmps;
    }

    // The following processing the bit string assuming LSB
    if (qpu->getBitOrder() == Accelerator::BitOrder::MSB) {
      std::reverse(bitstring.begin(), bitstring.end());
    }

    for (int j = 0; j < shift_map_names.size(); j++) {

      auto shift = shift_map_shifts[j];
      auto buff_name = shift_map_names[j];
      auto buffer = buf_map[buff_name];

      auto buffer_bitstring = bitstring.substr(shift, buffer->size());

      if (qpu->getBitOrder() == Accelerator::BitOrder::MSB) {
        std::reverse(buffer_bitstring.begin(), buffer_bitstring.end());
      }

      if (buf_counts[buff_name].count(buffer_bitstring)) {
        buf_counts[buff_name][buffer_bitstring] += kv.second;
      } else {
        buf_counts[buff_name].insert({buffer_bitstring, kv.second});
      }
    }
  }

  for (auto &b : bvec) {
    b->setMeasurements(buf_counts[b->name()]);
    b->addExtraInfo("endianness",
                    qpu->getBitOrder() == Accelerator::BitOrder::LSB ? "lsb"
                                                                     : "msb");
  }
}

} // namespace internal_compiler
} // namespace xacc