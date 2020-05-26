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

qubit qreg::operator[](const std::size_t &i) {
  return std::make_pair(buffer->name(), i);
}
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
int qreg::size() { return buffer->size(); }
void qreg::addChild(qreg &q) {
  for (auto &child : buffer->getChildren()) {
    results()->appendChild(child->name(), child);
  }
}

void qreg::print() { buffer->print(); }

double qreg::weighted_sum(Observable *obs) {
  auto terms = obs->getNonIdentitySubTerms();
  auto id = obs->getIdentitySubTerm();

  auto children = buffer->getChildren();

  double sum = 0.0;

  if (terms.size() != children.size()) {
    if (terms.size() + 1 == children.size()) {
      // find the I term in children and remove it
      // we will add it at the end
      children.erase(std::remove_if(children.begin(), children.end(),
                                    [](const auto &child) {
                                      return child->name() == "I";
                                    }),
                     children.end());
    } else {
      xacc::error("[qreg::weighted_sum()] error, number of observable terms != "
                  "number of children buffers.");
    }
  }

  for (int i = 0; i < children.size(); i++) {
    // std::cout << children[i]->name() << ", ";
    // std::cout << children[i]->getExpectationValueZ() << ", "
    //           << terms[i]->coefficient() << "\n";

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