#include "xacc_internal_compiler.hpp"
#include "xacc.hpp"
#include "InstructionIterator.hpp"

namespace xacc {
namespace internal_compiler {
const char *internal_qpu_name = "local-ibm";

void compiler_InitializeXACC() {
  if (!xacc::isInitialized())
    xacc::Initialize();
}

void setAccelerator(const char *_qpu_name) { internal_qpu_name = _qpu_name; }

// Map kernel source string representing a single
// kernel function to a single CompositeInstruction (src to IR)
CompositeInstruction *compile(const char *compiler_name, const char *kernel_src,
                              const char *qpu_name) {
  auto qpu = xacc::getAccelerator(qpu_name);
  auto compiler = xacc::getCompiler(compiler_name);
  auto IR = compiler->compile(kernel_src, qpu);
  auto program = IR->getComposites()[0];
  return program.get();
}

CompositeInstruction *compile(const char *compiler_name,
                              const char *kernel_src) {
  return compile(compiler_name, kernel_src, internal_qpu_name);
}

CompositeInstruction *getCompiled(const char *kernel_name) {
  return xacc::hasCompiled(kernel_name) ? xacc::getCompiled(kernel_name).get()
                                        : nullptr;
}

// Run quantum compilation routines on IR
void optimize(CompositeInstruction *program, const char *qpu_name,
              const OptLevel opt) {
  auto qpu = xacc::getAccelerator(qpu_name);
  auto as_shared = std::shared_ptr<CompositeInstruction>(
      program, empty_delete<CompositeInstruction>());
  if (opt == DEFAULT) {
    auto optimizer = xacc::getIRTransformation("circuit-optimizer");
    optimizer->apply(as_shared, qpu);
  } else {
    xacc::error("Other Optimization Levels not yet supported.");
  }
}
void optimize(CompositeInstruction *program, const OptLevel opt) {
  optimize(program, internal_qpu_name, opt);
}

// Execute on the specified QPU, persisting results to
// the provided buffer.
void execute(AcceleratorBuffer *buffer, const char *qpu_name,
             CompositeInstruction *program, double *parameters) {

  std::shared_ptr<CompositeInstruction> program_as_shared;
  if (parameters) {
    std::vector<double> values(parameters, parameters + program->nVariables());
    program_as_shared = program->operator()(values);
  } else {
    program_as_shared = std::shared_ptr<CompositeInstruction>(
        program, empty_delete<CompositeInstruction>());
  }
  auto qpu = xacc::getAccelerator(qpu_name);
  auto buffer_as_shared = std::shared_ptr<AcceleratorBuffer>(
      buffer, empty_delete<AcceleratorBuffer>());
  qpu->execute(buffer_as_shared, program_as_shared);
}

void execute(AcceleratorBuffer *buffer, CompositeInstruction *program,
             double *parameters) {
  execute(buffer, internal_qpu_name, program, parameters);
}

void execute(AcceleratorBuffer **buffers, const int nBuffers, const char *qpu_name,
             CompositeInstruction *program, double *parameters) {

  // FIXME Should take vector of buffers, and we collapse them
  //       into a single unified buffer for execution, then set the
  //       measurement counts accordingly in postprocessing.

  std::vector<AcceleratorBuffer *> bvec(
      buffers, buffers + nBuffers);

  auto program_as_shared = std::shared_ptr<CompositeInstruction>(
      program, empty_delete<CompositeInstruction>());

  // Merge the buffers. Keep track of buffer_name to shift in
  // all bit indices operating on that buffer_name, a map of
  // buffer names to the buffer ptr, and start a map to collect
  // buffer names to measurement counts
  int global_reg_size = 0;
  std::map<std::string, int> shift_map;
  std::map<std::string, AcceleratorBuffer*> buf_map;
  std::map<std::string, std::map<std::string, int>> buf_counts;
  for (auto &b : bvec) {
    shift_map.insert({b->name(), global_reg_size});
    buf_map.insert({b->name(), b});
    buf_counts.insert({b->name(), {}});
    global_reg_size += b->size();
  }
  auto tmp = xacc::qalloc(global_reg_size);

  // Update Program bit indices based on new global
  // qubit register
  InstructionIterator iter(program_as_shared);
  while (iter.hasNext()) {
    auto &next = *iter.next();
    std::vector<std::size_t> newBits;
    for (auto& b : next.bits()) {
        auto buffer_name = next.getBufferName(b);
        auto shift = shift_map[buffer_name];
        newBits.push_back(b+shift);
    }
    next.setBits(newBits);
    // FIXME Update buffer_names here too
  }

  // Now execute using the global merged register
  execute(tmp.get(), qpu_name, program, parameters);

  // Take bit strings and map to buffer individual bit strings
  for (auto &kv : tmp->getMeasurementCounts()) {
    auto bitstring = kv.first;
    for (auto& buff_names_shift : shift_map) {
        auto shift = buff_names_shift.second;
        auto buff_name = buff_names_shift.first;
        auto buffer = buf_map[buff_name];
        auto buffer_bitstring = bitstring.substr(shift, shift+buffer->size());
        if(buf_counts[buff_name].count(buffer_bitstring)) {
            buf_counts[buff_name][buffer_bitstring] += kv.second;
        } else {
            buf_counts[buff_name].insert({buffer_bitstring, kv.second});
        }
    }
  }

  for (auto& b : bvec) {
      b->setMeasurements(buf_counts[b->name()]);
  }
}

void execute(AcceleratorBuffer **buffers, const int nBuffers, CompositeInstruction *program,
             double *parameters) {
  execute(buffers, nBuffers, internal_qpu_name, program, parameters);
}
} // namespace internal_compiler
} // namespace xacc