#pragma once
#include <string>
#include <vector>

namespace xacc {
namespace aer {
std::string runPulseSim(const std::string &hamJsonStr, double dt,
                        const std::vector<double> &freqEst,
                        const std::vector<int> &uChanLoRefs,
                        const std::string &qObjJson);
std::string
noiseModelFromBackendProperties(const std::string &backendPropertiesJson);
} // namespace AER
} // namespace xacc