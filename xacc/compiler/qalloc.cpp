#include "qalloc.hpp"
#include "xacc.hpp"
#include <algorithm>

namespace xacc {
namespace internal_compiler {
template <typename T> struct empty_delete {
  empty_delete() {}
  void operator()(T *const) const {}
};

std::ostream &operator<<(std::ostream &os, qreg &q) {
  q.results()->print(os);
  return os;
}
void qreg::write_file(const std::string &file_name) {
  std::ofstream os(file_name);
  os << *this;
  return;
}

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
  auto name = "qrg_" + random_string(5);
  xacc::storeBuffer(name, buffer);
  cReg classicalReg(buffer);
  creg = classicalReg;

  for (std::size_t i = 0; i < n; i++) {
    internal_qubits.push_back(qubit{name, i, buffer.get()});
  }
}

qreg::qreg(const qreg &other)
    : buffer(other.buffer), been_named_and_stored(other.been_named_and_stored),
      creg(buffer), internal_qubits(other.internal_qubits) {}

qubit qreg::operator[](const std::size_t i) { return internal_qubits[i]; }

qreg::qreg(std::vector<qubit> &qubits) {
  buffer = xacc::qalloc(qubits.size());
  auto name = "qrg_" + random_string(5);
  xacc::storeBuffer(name, buffer);
  cReg classicalReg(buffer);
  creg = classicalReg;
  internal_qubits = qubits;
}

qreg qreg::extract_range(const std::size_t &start, const std::size_t &end) {
  return extract_range({start, end});
}

qreg qreg::head(const std::size_t n_qubits) {
  return extract_range({0, n_qubits});
}

qubit qreg::head() { return internal_qubits[0]; }

qubit qreg::tail() { return internal_qubits[size() - 1]; }

qreg qreg::tail(const std::size_t n_qubits) {
  return extract_range({size() - n_qubits, internal_qubits.size()});
}

qreg qreg::extract_range(const Range &&range) {
  assert(range.end <= size() &&
         "qreg::extract_range - you have set Range::end > qreg::size()");
  std::vector<qubit> new_qubits;
  for (int i = range.start; i < range.end; i += range.step) {
    new_qubits.push_back(internal_qubits[i]);
  }
  return qreg(new_qubits);
}

qreg qreg::extract_qubits(const std::initializer_list<std::size_t> &&qbits) {
  std::vector<std::size_t> v(qbits);
  std::size_t max_element = *std::max_element(v.begin(), v.end());
  assert(max_element < size() && "qreg::extract_qubits - you have requested a "
                                 "qubit idx outside the size() of this qreg.");
  std::vector<qubit> new_qubits;
  for (auto vv : v) {
    new_qubits.push_back(internal_qubits[vv]);
  }
  return qreg(new_qubits);
}

cReg::cReg(std::shared_ptr<AcceleratorBuffer> in_buffer) : buffer(in_buffer) {}
bool cReg::operator[](std::size_t i) {
  // Throw if this qubit hasn't been measured.
  return (*buffer)[i];
}
std::shared_ptr<AcceleratorBuffer> qreg::results_shared() { return buffer; }
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
void qreg::store() { xacc::storeBuffer(buffer); }
int qreg::size() { return internal_qubits.size(); }
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

static AllocEventListener *global_alloc_tracker = nullptr;
void setGlobalQubitManager(AllocEventListener *in_listener) {
  global_alloc_tracker = in_listener;
}

// Dummy one:
struct DummyListener : AllocEventListener {
  virtual void onAllocate(qubit *qubit) override {
    xacc::debug("Allocate: " + qubit->first + "[" +
                std::to_string(qubit->second) + "]");
  }
  virtual void onDealloc(qubit *qubit) override {
    xacc::debug("Deallocate: " + qubit->first + "[" +
                std::to_string(qubit->second) + "]");
  }
  static AllocEventListener *getInstance() {
    static DummyListener dummy;
    return &dummy;
  }
};

AllocEventListener *getGlobalQubitManager() {
  return global_alloc_tracker ? global_alloc_tracker
                              : DummyListener::getInstance();
}
} // namespace internal_compiler
} // namespace xacc

xacc::internal_compiler::qreg
qalloc(const int n, xacc::internal_compiler::QubitAllocator *allocator) {
  if (allocator) {
    std::vector<xacc::internal_compiler::qubit> qubits;
    for (int i = 0; i < n; ++i) {
      qubits.emplace_back(allocator->allocate());
    }
    return xacc::internal_compiler::qreg(qubits);
  } else {
    return xacc::internal_compiler::qreg(n);
  }
}
