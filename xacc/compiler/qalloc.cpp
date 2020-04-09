#include "qalloc.hpp"
#include "xacc.hpp"

namespace xacc {
namespace internal_compiler {
template <typename T> struct empty_delete {
  empty_delete() {}
  void operator()(T *const) const {}
};
qreg::qreg(const int n) { buffer = xacc::qalloc(n).get(); }
qreg::qreg(const qreg &other) : buffer(other.buffer) {}

int qreg::operator[](const int &i) { return 0; }
AcceleratorBuffer *qreg::results() { return buffer; }
std::map<std::string, int> qreg::counts() {
  return buffer->getMeasurementCounts();
}
double qreg::exp_val_z() { return buffer->getExpectationValueZ(); }
void qreg::reset() { buffer->resetBuffer(); }
void qreg::setName(const char *name) { buffer->setName(name); }
void qreg::setNameAndStore(const char *name) {
  setName(name);
  store();
}
void qreg::store() {
  auto buffer_as_shared = std::shared_ptr<AcceleratorBuffer>(
      buffer, empty_delete<AcceleratorBuffer>());
  xacc::storeBuffer(buffer_as_shared);
}

double qreg::weighted_sum(Observable *obs) {
  auto terms = obs->getNonIdentitySubTerms();
  auto id = obs->getIdentitySubTerm();

  auto children = buffer->getChildren();
  double sum = 0.0;
  for (int i = 0; i < children.size(); i++) {
    //   std::cout << children[i]->name() << ", "
    //             << children[i]->getExpectationValueZ() << ", "
    //             << terms[i]->coefficient() << "\n";

      sum += children[i]->getExpectationValueZ() *
             std::real(terms[i]->coefficient());
  }

  if (id) {
      sum += std::real(id->coefficient());
  }
  return sum;
}

} // namespace internal_compiler
} // namespace xacc