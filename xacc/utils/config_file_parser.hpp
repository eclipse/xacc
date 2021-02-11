#pragma once

#include "heterogeneous.hpp"
#include "Identifiable.hpp"

namespace xacc {
// Parse a config file to a HeterogeneousMap
class ConfigFileParsingUtil : public Identifiable {
public:
  virtual HeterogeneousMap parse(const std::string &in_fileName) = 0;
};
} // namespace xacc