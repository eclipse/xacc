#include "machine.hpp"

namespace xacc {
class NoiseModel;
// Override TriQ's Machine class
class BackendMachine : public Machine {
public:
  BackendMachine(const NoiseModel &backendNoiseModel);
};
} // namespace xacc