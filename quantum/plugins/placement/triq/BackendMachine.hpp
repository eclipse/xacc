#include "machine.hpp"

namespace xacc {
class NoiseModel;
// Override TriQ's Machine class
class BackendMachine : Machine {
  BackendMachine(const NoiseModel &backendNoiseModel);
};
} // namespace xacc