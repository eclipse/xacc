#include "PulseScheduler.hpp"
#include "InstructionIterator.hpp"
#include "Pulse.hpp"

#include "xacc.hpp"

namespace xacc {
namespace quantum {

void PulseScheduler::schedule(std::shared_ptr<CompositeInstruction> program) {

  // Remember that sub-composites have timings relative to each other internally
  std::map<std::string, std::size_t> channel2times;

   InstructionIterator it(program);
   while (it.hasNext()) {
     auto nextInst = it.next();
     if (nextInst->isEnabled() && !nextInst->isComposite()) {
         auto pulse = std::dynamic_pointer_cast<Pulse>(nextInst);
         if (!pulse) {
             xacc::error("Invalid instruction in pulse program.");
         }
         if (!channel2times.count(pulse->channel())) {
             channel2times.insert({pulse->channel(), 0});
         }
     }
   }

     InstructionIterator it2(program);
   while (it2.hasNext()) {
     auto nextInst = it2.next();
     if (nextInst->isEnabled() && !nextInst->isComposite()) {
         auto pulse = std::dynamic_pointer_cast<Pulse>(nextInst);
         auto duration = pulse->duration();
         auto currentTimeOnChannel = channel2times[pulse->channel()];
         pulse->setStart(currentTimeOnChannel+duration);
         channel2times[pulse->channel()] += duration;
     } 
   }

}

}
}