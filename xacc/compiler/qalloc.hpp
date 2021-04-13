/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_QALLOC_HPP_
#define XACC_QALLOC_HPP_

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <cassert>

namespace xacc {
class AcceleratorBuffer;
class Observable;
namespace internal_compiler {
// We treat a qubit as a pair { QREG_VAR_NAME, QREG_IDX }
// for use with qcor QRT API we also need to keep track of the buffer
struct qubit {
  std::string first;
  std::size_t second;
  xacc::AcceleratorBuffer *buffer;
  xacc::AcceleratorBuffer *results() { return buffer; }
};

class qreg;
// Classical register associated with a qubit register to store single-shot
// measurement results. i.e. after Measure(q[0]) => q.creg[0] will contain the
// boolean (true/false) result of the measurement.
class cReg {
public:
  cReg() = default;
  cReg(std::shared_ptr<AcceleratorBuffer> in_buffer);
  bool operator[](std::size_t i);

private:
  std::shared_ptr<AcceleratorBuffer> buffer;
};

// A qreg is a vector of qubits
class qreg {
private:
  std::vector<qubit> internal_qubits;
  std::string random_string(std::size_t length);

protected:
  std::shared_ptr<AcceleratorBuffer> buffer;
  bool been_named_and_stored = false;

public:
  struct Range {
    std::size_t start;
    std::size_t end;
    std::size_t step = 1;
    Range(std::vector<std::size_t> &s) {
      assert(
          s.size() > 1 &&
          "qreg::Range error - you must provide {start, end, optional step=1}");
      start = s[0];
      end = s[1];
      if (s.size() > 2) {
        step = s[2];
      }
    }
    Range(std::initializer_list<std::size_t> &&s) {
      assert(
          s.size() > 1 &&
          "qreg::Range error - you must provide {start, end, optional step=1}");
      std::vector<std::size_t> v(s);
      start = v[0];
      end = v[1];
      if (v.size() > 2) {
        step = v[2];
      }
    }
  };

  qreg() = default;
  qreg(const int n);
  qreg(const qreg &other);
  qreg(std::vector<qubit> &qubits);
  qubit operator[](const std::size_t i);
  qreg extract_range(const std::size_t& start, const std::size_t& end);
  qreg extract_range(const Range &&range);
  qreg extract_qubits(const std::initializer_list<std::size_t> &&qbits);
  qreg head(const std::size_t n_qubits);
  qubit head();
  qubit tail();
  qreg tail(const std::size_t n_qubits);
  AcceleratorBuffer *results();
  std::shared_ptr<AcceleratorBuffer> results_shared();
  std::map<std::string, int> counts();
  double exp_val_z();
  void reset();
  int size();
  void addChild(qreg &q);
  void setName(const char *name);
  void setNameAndStore(const char *name);
  std::string name();
  void store();
  void print();
  double weighted_sum(Observable *obs);
  void write_file(const std::string &file_name);
  // friend std::ostream& operator<<(std::ostream& os, qreg& q);
  // Public member var to simplify the single measurement syntax:
  // i.e. we can access the single measurement results via the syntax:
  // q.creg[i] vs. (*q.results())[i]
  cReg creg;
};
// std::ostream& operator<<(std::ostream& os, qreg& q);
} // namespace internal_compiler
} // namespace xacc

xacc::internal_compiler::qreg qalloc(const int n);

// __qpu__ indicates this functions is for the QCOR Clang Syntax Handler
// and annotated with quantum for the LLVM IR CodeGen
#define __qpu__ [[clang::syntax(qcor)]] __attribute__((annotate("quantum")))

#endif