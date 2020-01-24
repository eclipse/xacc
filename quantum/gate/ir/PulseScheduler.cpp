#include "PulseScheduler.hpp"
#include "InstructionIterator.hpp"
#include "Pulse.hpp"

#include "xacc.hpp"

namespace {
  // Note: this is a *SEQUENTIAL* pulse scheduler, i.e. it will respect the ordering of the composites (command defs),
  // e.g. if we do a "pulse::cx_0_1; pulse::cx_4_5"; pulse sequence of the second one (pulse::cx_4_5) will be scheduled
  // *AFTER* that of the first one "pulse::cx_0_1".
  // TODO: we can implement a smarter scheduler which will keep track of the *set* of channels related to a composite,
  // and in case that the two sets of channels are completely disjoint (for two consecutive composites), we can schedule them in parallel 
  // (i.e. same start time for both composites)
  void processComposite(std::shared_ptr<xacc::CompositeInstruction> composite, size_t compositeStartTime, std::map<std::string, std::size_t>& io_channel2times) {
    // Process children of a composite instructions
    for (auto& inst: composite->getInstructions()) {
      if (inst->isEnabled() && !inst->isComposite()) {
        auto pulse = std::dynamic_pointer_cast<xacc::quantum::Pulse>(inst);
        if (!pulse) {
            xacc::error("Invalid instruction in pulse program.");
        }
        if (io_channel2times.find(pulse->channel()) == io_channel2times.end()) {
          io_channel2times.insert({pulse->channel(), compositeStartTime});
        }
        auto& currentTimeOnChannel = io_channel2times[pulse->channel()];
        // The expected start time of a pulse is shifted by that of the parent composite.
        auto pulseExpectedStart = pulse->start() + compositeStartTime;
        
        // Normally, in a well-formed pulse composite instruction (e.g. those from IBM json)
        // individual pulses within a composite (command def) already have their start time set correctly 
        // (i.e. pulses on the same channels are spaced correctly)
        // hence, the following condition should be passing
        if (pulseExpectedStart >= currentTimeOnChannel) {
          // Well-designed pulses, just set the start time to the expected.
          pulse->setStart(pulseExpectedStart);
        } else {
          // If the expected start time is less than the current time on the channel,
          // i.e. the end time of the previous pulse on this channel.
          // This may be a result of manually constructing the composite using IR's,
          // whereby we just appending pulse IR's without spacing pulses according to their durations.
          // In this case, we just gracefully shift the start time to the end time of the previous pulse on the channel
          pulse->setStart(currentTimeOnChannel);
        }
        // Update the current time on channel after the pulse has been scheduled.
        currentTimeOnChannel = pulse->start() + pulse->duration();
      } else if (inst->isEnabled() && inst->isComposite()) {
        // Composite nested within a composite
        // The start time of this nested composite must be the latest time of all channels
        size_t nextCompositeStartTime = 0;
        for (const auto& kv : io_channel2times) {
          nextCompositeStartTime = kv.second > nextCompositeStartTime ? kv.second  : nextCompositeStartTime;
        }

        auto compositeInstPtr = std::dynamic_pointer_cast<xacc::CompositeInstruction>(inst);
        if (!compositeInstPtr) {
            xacc::error("Invalid instruction in pulse program.");
        }
        // Recursive
        processComposite(compositeInstPtr, nextCompositeStartTime, io_channel2times);
      }
    } 
  }
}


namespace xacc {
namespace quantum {

void PulseScheduler::schedule(std::shared_ptr<CompositeInstruction> program) {

  // Remember that sub-composites have timings relative to each other internally
  std::map<std::string, std::size_t> channel2times;
  // Run the recursive scheduler, starting at this root composite at time 0:
  processComposite(program, 0, channel2times);
}

}
}