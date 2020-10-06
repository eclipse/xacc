#pragma once
#include "Identifiable.hpp"

namespace xacc {
namespace quantum {

class QObjGenerator : public Identifiable {
public:
  virtual std::string
  getQObjJsonStr(std::vector<std::shared_ptr<CompositeInstruction>> composites,
                 const int &shots, const nlohmann::json &backend,
                 const std::string getBackendPropsResponse,
                 std::vector<std::pair<int, int>> &connectivity,
                 const nlohmann::json &backendDefaults) = 0;
};
} // namespace quantum
} // namespace xacc