#include "qalloc.hpp"
#include "xacc.hpp"
#include <algorithm>

namespace xacc {
namespace internal_compiler {
template <typename T> struct empty_delete {
  empty_delete() {}
  void operator()(T *const) const {}
};

std::string qreg::random_string(std::size_t length) {
  auto randchar = []() -> char {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

qreg::qreg(const int n) {
  buffer = xacc::qalloc(n);
  auto name = "qreg_"+random_string(5);
  xacc::storeBuffer(name, buffer);
}

qreg::qreg(const qreg &other)
    : buffer(other.buffer), been_named_and_stored(other.been_named_and_stored) {
}

qubit qreg::operator[](const std::size_t i) {
  return std::make_pair(buffer->name(), i);
}
// qreg &qreg::operator=(const qreg &q) {
//   buffer = q.buffer;
//   return *this;
// }

AcceleratorBuffer *qreg::results() { return buffer.get(); }
std::map<std::string, int> qreg::counts() {
  return buffer->getMeasurementCounts();
}
std::string qreg::name() { return buffer->name(); }

double qreg::exp_val_z() { return buffer->getExpectationValueZ(); }
void qreg::reset() { buffer->resetBuffer(); }
void qreg::setName(const char *name) { buffer->setName(name); }
void qreg::setNameAndStore(const char *name) {
  if (!been_named_and_stored) {
    setName(name);
    store();
    been_named_and_stored = true;
  }
}
void qreg::store() {
  xacc::storeBuffer(buffer);
}
int qreg::size() { return buffer->size(); }
void qreg::addChild(qreg &q) {
  for (auto &child : q.buffer->getChildren()) {
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

xacc::internal_compiler::qreg qalloc(const int n) {
  return xacc::internal_compiler::qreg(n);
}
