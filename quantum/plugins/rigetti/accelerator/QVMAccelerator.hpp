#ifndef QUANTUM_GATE_ACCELERATORS_QVMACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_QVMACCELERATOR_HPP_

#include "RemoteAccelerator.hpp"
#include "InstructionIterator.hpp"
#include "QuilVisitor.hpp"
#include "xacc.hpp"

using namespace xacc;

namespace xacc {
namespace quantum {

/**
 * The QVMAccelerator is a QPUGate Accelerator that
 * provides an execute implementation that maps XACC IR
 * to an equivalent Quil string, and executes it on the
 * Rigetti QVM at 127.0.0.1:5000/qvm
 *
 *
 */
class QVMAccelerator : virtual public RemoteAccelerator {
public:
  QVMAccelerator() : RemoteAccelerator() {}

  QVMAccelerator(std::shared_ptr<Client> client)
      : RemoteAccelerator(client) {}

  void
  execute(std::shared_ptr<AcceleratorBuffer> buffer,
          const std::vector<std::shared_ptr<CompositeInstruction>> functions) override {
    int counter = 0;
    std::vector<std::shared_ptr<AcceleratorBuffer>> tmpBuffers;
    for (auto f : functions) {
      xacc::info("QVM Executing kernel = " + f->name());
      auto tmpBuffer = std::make_shared<AcceleratorBuffer>(buffer->name() + std::to_string(counter),
                                    buffer->size());
      RemoteAccelerator::execute(tmpBuffer, f);
      tmpBuffers.push_back(tmpBuffer);
      buffer->appendChild(tmpBuffer->name(), tmpBuffer);
      counter++;
    }

    return;
  }

  void initialize(const HeterogeneousMap& params = {}) override {}
  const std::vector<std::string> configurationKeys() override {
      return {};
  }
  void updateConfiguration(const HeterogeneousMap &config) override {
  }


  const std::string
  processInput(std::shared_ptr<AcceleratorBuffer> buffer,
               std::vector<std::shared_ptr<CompositeInstruction>> functions) override;

  void
  processResponse(std::shared_ptr<AcceleratorBuffer> buffer,
                  const std::string &response) override;

  std::vector<std::shared_ptr<IRTransformation>>
  getIRTransformations() override {
    std::vector<std::shared_ptr<IRTransformation>> v;
    return v;
  }

  /**
   * Return all relevant QVMAccelerator runtime options.
   * Users can set the api-key, execution type, and number of triels
   * from the command line with these options.
   */
  OptionPairs getOptions() override {
    OptionPairs desc {{"rigetti-shots",
        "Provide the number of shots to execute on the QVM."}};
    return desc;
  }

  const std::string name() const override { return "rigetti-qvm"; }

  const std::string description() const override {
    return "The Rigetti QVM Accelerator interacts with "
           "the Forest QVM to execute XACC quantum IR.";
  }

  virtual ~QVMAccelerator() {}

private:
  std::vector<int> currentMeasurementSupports;

};

} // namespace quantum
} // namespace xacc

#endif
