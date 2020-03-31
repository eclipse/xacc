#include "xacc_internal_compiler.hpp"
#include "Utils.hpp"
#include "xacc.hpp"
#include "InstructionIterator.hpp"

namespace xacc {
namespace internal_compiler {
Accelerator *qpu = nullptr;
CompositeInstruction *lastCompiled = nullptr;
bool __execute = true;

void __set_verbose(bool v) { xacc::set_verbose(v); }
void compiler_InitializeXACC(const char *qpu_backend) {
  if (!xacc::isInitialized())
    xacc::Initialize();

  xacc::external::load_external_language_plugins();
  setAccelerator(qpu_backend);
}

void compiler_InitializeXACC(const char *qpu_backend, const int shots) {
  if (!xacc::isInitialized())
    xacc::Initialize();

  xacc::external::load_external_language_plugins();
  setAccelerator(qpu_backend, shots);
}

void setAccelerator(const char *qpu_backend) {
  if (qpu) {
    if (qpu_backend != qpu->name()) {
      qpu = xacc::getAccelerator(qpu_backend).get();
    }
  } else {
    qpu = xacc::getAccelerator(qpu_backend).get();
  }
}

void setAccelerator(const char *qpu_backend, const int shots) {
  if (qpu) {
    if (qpu_backend != qpu->name()) {
      qpu = xacc::getAccelerator(qpu_backend, {std::make_pair("shots", shots)})
                .get();
    }
  } else {
    qpu = xacc::getAccelerator(qpu_backend, {std::make_pair("shots", shots)})
              .get();
  }
}

Accelerator *get_qpu() { return qpu; }

CompositeInstruction *getLastCompiled() { return lastCompiled; }

// Map kernel source string representing a single
// kernel function to a single CompositeInstruction (src to IR)
CompositeInstruction *compile(const char *compiler_name,
                              const char *kernel_src) {
  auto qpu_as_shared =
      std::shared_ptr<Accelerator>(qpu, xacc::empty_delete<Accelerator>());
  auto compiler = xacc::getCompiler(compiler_name);
  auto IR = compiler->compile(kernel_src, qpu_as_shared);
  auto program = IR->getComposites()[0];
  lastCompiled = program.get();
  return program.get();
}

CompositeInstruction *getCompiled(const char *kernel_name) {
  return xacc::hasCompiled(kernel_name) ? xacc::getCompiled(kernel_name).get()
                                        : nullptr;
}

// Run quantum compilation routines on IR
void optimize(CompositeInstruction *program, const OptLevel opt) {

  xacc::info("[InternalCompiler] Pre-optimization, circuit has " +
             std::to_string(program->nInstructions()) + " instructions.");

  // We don't own this ptr, so create shared_ptr with empty deleter
  auto as_shared = std::shared_ptr<CompositeInstruction>(
      program, xacc::empty_delete<CompositeInstruction>());

  auto qpu_as_shared =
      std::shared_ptr<Accelerator>(qpu, xacc::empty_delete<Accelerator>());
  if (opt == DEFAULT) {
    auto optimizer = xacc::getIRTransformation("circuit-optimizer");
    optimizer->apply(as_shared, qpu_as_shared);
  } else {
    xacc::error("Other Optimization Levels not yet supported.");
  }

  xacc::info("[InternalCompiler] Post-optimization, circuit has " +
             std::to_string(program->nInstructions()) + " instructions.");
}

// Execute on the specified QPU, persisting results to
// the provided buffer.
void execute(AcceleratorBuffer *buffer, CompositeInstruction *program,
             double *parameters) {

  std::shared_ptr<CompositeInstruction> program_as_shared;
  if (parameters) {
    std::vector<double> values(parameters, parameters + program->nVariables());
    program_as_shared = program->operator()(values);
  } else {
    // We don't own this ptr, so create shared_ptr with empty deleter
    program_as_shared = std::shared_ptr<CompositeInstruction>(
        program, xacc::empty_delete<CompositeInstruction>());
  }
  auto buffer_as_shared = std::shared_ptr<AcceleratorBuffer>(
      buffer, xacc::empty_delete<AcceleratorBuffer>());

  optimize(program);

  qpu->execute(buffer_as_shared, program_as_shared);
}

void execute(AcceleratorBuffer **buffers, const int nBuffers,
             CompositeInstruction *program, double *parameters) {

  //  Should take vector of buffers, and we collapse them
  //  into a single unified buffer for execution, then set the
  //  measurement counts accordingly in postprocessing.

  std::vector<AcceleratorBuffer *> bvec(buffers, buffers + nBuffers);
  std::vector<std::string> buffer_names;
  for (auto &a : bvec)
    buffer_names.push_back(a->name());

  // We don't own this ptr, so create shared_ptr with empty deleter
  auto program_as_shared = std::shared_ptr<CompositeInstruction>(
      program, xacc::empty_delete<CompositeInstruction>());

  // Do we have any unknown ancilla bits?
  std::vector<std::string> possible_extra_buffers;
  int possible_size = -1;
  InstructionIterator it(program_as_shared);
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
    InstructionIterator it2(program_as_shared);
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
  InstructionIterator iter(program_as_shared);
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
  InstructionIterator iter2(program_as_shared);
  while (iter2.hasNext()) {
    auto &next = *iter2.next();
    if (next.name() == "Measure") {
      measure_idxs.push_back(next.bits()[0]);
    }
  }

  optimize(program);

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