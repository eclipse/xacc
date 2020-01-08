#include "xacc_internal_compiler.hpp"
#include "xacc.hpp"
#include "InstructionIterator.hpp"

namespace xacc {
namespace internal_compiler {
Accelerator *qpu = nullptr;

void compiler_InitializeXACC(const char *qpu_backend) {
  if (!xacc::isInitialized())
    xacc::Initialize();

  setAccelerator(qpu_backend);

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

// Map kernel source string representing a single
// kernel function to a single CompositeInstruction (src to IR)
CompositeInstruction *compile(const char *compiler_name,
                              const char *kernel_src) {
  auto qpu_as_shared =
      std::shared_ptr<Accelerator>(qpu, empty_delete<Accelerator>());
  auto compiler = xacc::getCompiler(compiler_name);
  auto IR = compiler->compile(kernel_src, qpu_as_shared);
  auto program = IR->getComposites()[0];
  return program.get();
}

CompositeInstruction *getCompiled(const char *kernel_name) {
  return xacc::hasCompiled(kernel_name) ? xacc::getCompiled(kernel_name).get()
                                        : nullptr;
}

// Run quantum compilation routines on IR
void optimize(CompositeInstruction *program, const OptLevel opt) {

  // We don't own this ptr, so create shared_ptr with empty deleter
  auto as_shared = std::shared_ptr<CompositeInstruction>(
      program, empty_delete<CompositeInstruction>());

  auto qpu_as_shared =
      std::shared_ptr<Accelerator>(qpu, empty_delete<Accelerator>());
  if (opt == DEFAULT) {
    auto optimizer = xacc::getIRTransformation("circuit-optimizer");
    optimizer->apply(as_shared, qpu_as_shared);
  } else {
    xacc::error("Other Optimization Levels not yet supported.");
  }
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
        program, empty_delete<CompositeInstruction>());
  }
  auto buffer_as_shared = std::shared_ptr<AcceleratorBuffer>(
      buffer, empty_delete<AcceleratorBuffer>());
  qpu->execute(buffer_as_shared, program_as_shared);
}

void execute(AcceleratorBuffer **buffers, const int nBuffers,
             CompositeInstruction *program, double *parameters) {

  // FIXME Should take vector of buffers, and we collapse them
  //       into a single unified buffer for execution, then set the
  //       measurement counts accordingly in postprocessing.

  std::vector<AcceleratorBuffer *> bvec(buffers, buffers + nBuffers);

  // We don't own this ptr, so create shared_ptr with empty deleter
  auto program_as_shared = std::shared_ptr<CompositeInstruction>(
      program, empty_delete<CompositeInstruction>());

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

  xacc::debug("[xacc_internal_compiler] Creating register of size " + std::to_string(global_reg_size));
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

  // Now execute using the global merged register
  execute(tmp.get(), program, parameters);

  // Take bit strings and map to buffer individual bit strings
  for (auto &kv : tmp->getMeasurementCounts()) {
    auto bitstring = kv.first;
    if (qpu->getBitOrder() == Accelerator::BitOrder::MSB) {
        std::reverse(bitstring.begin(), bitstring.end());
    }

    for (int j = 0; j < shift_map_names.size(); j++) {

      auto shift = shift_map_shifts[j];
      auto buff_name = shift_map_names[j];
      auto buffer = buf_map[buff_name];

      auto buffer_bitstring = bitstring.substr(shift,  buffer->size());

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
  }
}

} // namespace internal_compiler
} // namespace xacc