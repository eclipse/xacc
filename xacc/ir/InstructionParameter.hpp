/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_IR_INSTRUCTIONPARAMETER_HPP_
#define XACC_IR_INSTRUCTIONPARAMETER_HPP_
#include <vector>
#include <string>
#include <complex>
#include <map>

#include "variant.hpp"

#include "Utils.hpp"

namespace xacc {

template <typename... Types> class Variant : public mpark::variant<Types...> {

private:
  class ToStringVisitor {
  public:
    template <typename T> std::string operator()(const T &t) const {
      std::stringstream ss;
      ss << t;
      return ss.str();
    }
  };
  class IsArithmeticVisitor {
  public:
    template <typename T> bool operator()(const T &t) const {
      return std::is_arithmetic<T>::value;
    }
  };

  std::map<int, std::string> whichType{{0,"int"}, {1,"double"},
                                        {2,"string"}, {3,"complex"},
                                        {4,"vector<pair<int>>"},
                                        {5,"vector<pair<double>>"},
                                        {6,"vector<int>"},
                                        {7,"vector<double>"},
                                        {8,"vector<string>"}};
public:
  Variant() : mpark::variant<Types...>() {}
  template <typename T>
  Variant(T &element) : mpark::variant<Types...>(element) {}
  template <typename T>
  Variant(T &&element) : mpark::variant<Types...>(element) {}
  Variant(const Variant &element) : mpark::variant<Types...>(element) {}

  template <typename T> T as() const {
    try {
      // First off just try to get it
      return mpark::get<T>(*this);
    } catch (std::exception &e) {
        std::stringstream s;
        s << "InstructionParameter.toString() = " << toString() << "\n";
        s << "This InstructionParameter type id is " << this->which() << "\nAllowed Ids to Type\n";
        for (auto& kv : whichType) {
            s << kv.first << ": " << kv.second << "\n";
        }
        XACCLogger::instance()->error("Cannot cast Variant:\n" + s.str());
        print_backtrace();
        exit(0);
    }
    return T();
  }
  int which() const {
      return this->index();
  }
  bool isNumeric() const {
    IsArithmeticVisitor v;
    return mpark::visit(v, *this);
  }

  bool isComplex() const {
      try {
          mpark::get<std::complex<double>>(*this);
      } catch(std::exception& e) {
          return false;
      }
      return true;
  }
  bool isVariable() const {
    try {
      mpark::get<std::string>(*this);
    } catch (std::exception &e) {
      return false;
    }
    return true;
  }

  const std::string toString() const {
    ToStringVisitor vis;
    return mpark::visit(vis, *this);
  }

  bool operator==(const Variant<Types...> &v) const {
    return v.toString() == toString();
  }

  bool operator!=(const Variant<Types...> &v) const { return !operator==(v); }

};

using InstructionParameter =
    Variant<int, double, std::string, std::complex<double>,
            std::vector<std::pair<int, int>>,
            std::vector<std::pair<double, double>>,
            std::vector<int>, std::vector<double>, std::vector<std::string>>;

} // namespace xacc
#endif
