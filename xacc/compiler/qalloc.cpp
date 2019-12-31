#include "qalloc.hpp"
#include "xacc.hpp"
namespace xacc {
namespace internal_compiler {

qreg::qreg(const int n) { buffer = xacc::qalloc(n).get(); }

int qreg::operator[](const int &i) { return 0; }
AcceleratorBuffer *qreg::results() { return buffer; }
std::map<std::string, int> qreg::counts() {
  return buffer->getMeasurementCounts();
}
double qreg::exp_val_z() { return buffer->getExpectationValueZ(); }
void qreg::reset() { buffer->resetBuffer(); }
} // namespace internal_compiler
} // namespace xacc