#include "qalloc.hpp"
#include "xacc.hpp"
namespace xacc {
namespace internal_compiler {
template <typename T> struct empty_delete {
  empty_delete() {}
  void operator()(T *const) const {}
};
qreg::qreg(const int n) { buffer = xacc::qalloc(n).get(); }

int qreg::operator[](const int &i) { return 0; }
AcceleratorBuffer *qreg::results() { return buffer; }
std::map<std::string, int> qreg::counts() {
  return buffer->getMeasurementCounts();
}
double qreg::exp_val_z() { return buffer->getExpectationValueZ(); }
void qreg::reset() { buffer->resetBuffer(); }
void qreg::setName(const char *name) { buffer->setName(name); }
void qreg::store() {
  auto buffer_as_shared = std::shared_ptr<AcceleratorBuffer>(
      buffer, empty_delete<AcceleratorBuffer>());
  xacc::storeBuffer(buffer_as_shared);
}
} // namespace internal_compiler
} // namespace xacc