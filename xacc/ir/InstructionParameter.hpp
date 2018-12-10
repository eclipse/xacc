/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#ifndef XACC_IR_INSTRUCTIONPARAMETER_HPP_
#define XACC_IR_INSTRUCTIONPARAMETER_HPP_
#include <vector>
#include <string>
#include <complex>

#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>

#include "Utils.hpp"

namespace xacc {

using VariantType =
    boost::variant<int, double, std::string, std::complex<double>,
                   std::vector<std::pair<int, int>>,
                   std::vector<std::pair<double, double>>>;

template <typename... Types> class Variant : public boost::variant<Types...> {

private:
  class ToStringVisitor : public boost::static_visitor<std::string> {
  public:
    template <typename T> std::string operator()(const T &t) const {
      std::stringstream ss;
      ss << t;
      return ss.str();
    }
  };
  class IsArithmeticVisitor : public boost::static_visitor<bool> {
  public:
    template <typename T> bool operator()(const T &t) const {
      return std::is_arithmetic<T>::value;
    }
  };

public:
  Variant() : boost::variant<Types...>() {}
  template <typename T>
  Variant(T &element) : boost::variant<Types...>(element) {}
  template <typename T>
  Variant(T &&element) : boost::variant<Types...>(element) {}
  Variant(const Variant &element) : boost::variant<Types...>(element) {}

  template <typename T> T as() const {
    try {
      // First off just try to get it
      return boost::get<T>(*this);
    } catch (std::exception &e) {
        XACCLogger::instance()->error("Cannot cast Variant:  " +
                                      std::string(e.what()));
    }
    return T();
  }

  bool isNumeric() const {
    IsArithmeticVisitor v;
    return boost::apply_visitor(v, *this);
  }

  bool isVariable() const {
    try {
      boost::get<std::string>(*this);
    } catch (std::exception &e) {
      return false;
    }
    return true;
  }

  const std::string toString() const {
    ToStringVisitor vis;
    return boost::apply_visitor(vis, *this);
  }

  bool operator==(const Variant<Types...> &v) const {
    return v.toString() == toString();
  }

  bool operator!=(const Variant<Types...> &v) const { return !operator==(v); }
};

using InstructionParameter =
    Variant<int, double, std::string, std::complex<double>,
            std::vector<std::pair<int, int>>,
            std::vector<std::pair<double, double>>>;

} // namespace xacc
#endif
