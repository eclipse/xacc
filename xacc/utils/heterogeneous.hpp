/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
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
#ifndef XACC_HETEROGENEOUS_HPP_
#define XACC_HETEROGENEOUS_HPP_

#include <initializer_list>
#include <map>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <experimental/type_traits>

// mpark variant
#include "variant.hpp"

#include "Utils.hpp"
#include <complex>
#include <any>

namespace xacc {

class HeterogeneousMap;

template <class...> struct is_heterogeneous_map : std::false_type {};

template <> struct is_heterogeneous_map<HeterogeneousMap> : std::true_type {};

template <class...> struct type_list {};

template <class... TYPES> struct visitor_base {
  using types = xacc::type_list<TYPES...>;
};

class HeterogeneousMap {
public:
  HeterogeneousMap() = default;
  HeterogeneousMap(const HeterogeneousMap &_other) { *this = _other; }
  HeterogeneousMap(HeterogeneousMap &_other) { *this = _other; }

  HeterogeneousMap &operator=(const HeterogeneousMap &_other) {
    clear();
    items = _other.items;
    return *this;
  }

  template <typename T> void loop_pairs(T value) {
    insert(value.first, value.second);
  }

  template <typename First, typename... Rest>
  void loop_pairs(First firstValue, Rest... rest) {
    loop_pairs(firstValue);
    loop_pairs(rest...);
  }

  template <typename... TYPES,
            typename = std::enable_if_t<!is_heterogeneous_map<
                std::remove_cv_t<std::remove_reference_t<TYPES>>...>::value>>
  HeterogeneousMap(TYPES &&... list) {
    loop_pairs(list...);
  }

  // Better constructor, just provide key values without std::make_pair()
  HeterogeneousMap(
      std::initializer_list<std::pair<std::string, std::any>> &&list) {
    for (auto &l : list) {
      insert(l.first, l.second);
    }
  }

  template <typename... Ts> void print(std::ostream &os) const {
    _internal_print_visitor<Ts...> v(os);
    visit(v);
  }

  template <class T> void insert(const std::string key, const T &_t) {
    if (items.count(key)) {
      items.at(key) = _t;
    } else {
      items.insert({key, _t});
    }
  }

  template <typename T> const T get(const std::string key) const {
    try {
      return std::any_cast<T>(items.at(key));
    } catch (std::exception &e) {
      XACCLogger::instance()->error(
          "HeterogeneousMap::get() error - Invalid type or key (" + key + ").");
    }
    return T();
  }

  template <class T> const T get_with_throw(const std::string key) const {
    return std::any_cast<T>(items.at(key));
  }

  bool stringExists(const std::string key) const {
    if (keyExists<const char *>(key)) {
      return true;
    }
    if (keyExists<std::string>(key)) {
      return true;
    }
    return false;
  }

  const std::string getString(const std::string key) const {
    if (keyExists<const char *>(key)) {
      return get<const char *>(key);
    } else if (keyExists<std::string>(key)) {
      return get<std::string>(key);
    } else {
      XACCLogger::instance()->error("No string-like value at provided key (" +
                                    key + ").");
      print_backtrace();
    }
    return "";
  }

  template <typename T> bool pointerLikeExists(const std::string key) const {
    if (keyExists<T *>(key)) {
      return true;
    } else if (keyExists<std::shared_ptr<T>>(key)) {
      return true;
    } else {
      return false;
    }
  }
  template <typename T> T *getPointerLike(const std::string key) const {
    if (keyExists<T *>(key)) {
      return get<T *>(key);
    } else if (keyExists<std::shared_ptr<T>>(key)) {
      return get<std::shared_ptr<T>>(key).get();
    } else {
      XACCLogger::instance()->error("No pointer-like value at provided key (" +
                                    key + ").");
      print_backtrace();
    }
    return nullptr;
  }
  void clear() { items.clear(); }

  template <class T> size_t number_of() const {
    _internal_number_of_visitor<T> v;
    visit(v);
    return v.count;
  }

  bool key_exists_any_type(const std::string key) const {
    return items.count(key);
  }

  template <typename T> bool keyExists(const std::string key) const {
    if (items.count(key)) {
      // we have the key, make sure it is the right type
      try {
        std::any_cast<T>(items.at(key));
      } catch (std::exception &e) {
        return false;
      }
      return true;
    }
    return false;
  }

  size_t size() const { return items.size(); }

  ~HeterogeneousMap() { clear(); }

  template <class T> void visit(T &&visitor) const {
    visit_impl(visitor, typename std::decay_t<T>::types{});
  }

private:
  std::map<std::string, std::any> items;

  template <typename T>
  class _internal_number_of_visitor : public visitor_base<T> {
  public:
    int count = 0;
    void operator()(const std::string &key, const T &t) { count++; }
  };

  template <typename... Ts>
  class _internal_print_visitor : public visitor_base<Ts...> {
  private:
    std::ostream &ss;

  public:
    _internal_print_visitor(std::ostream &s) : ss(s) {}

    template <typename T> void operator()(const std::string &key, const T &t) {
      ss << key << ": " << t << "\n";
    }
  };

  template <class T, class HEAD, class... TAIL> struct try_visit {
    template <class U> static void apply(T &visitor, U &&element) {
      try {
        visitor(element.first, std::any_cast<HEAD>(element.second));
      } catch (...) {
        try_visit<T, TAIL...>::apply(visitor, element);
      }
    }
  };
  template <class T, class HEAD> struct try_visit<T, HEAD> {
    template <class U> static void apply(T &visitor, U &&element) {
      try {
        visitor(element.first, std::any_cast<HEAD>(element.second));
      } catch (...) {
      }
    }
  };
  template <class T, class... U>
  void visit_impl(T &&visitor, xacc::type_list<U...>) const {
    for (auto &&element : items) {
      try_visit<std::decay_t<T>, U...>::apply(visitor, element);
    }
  }
};

// Make sure these basic types are always instantiatied for HeterogeneousMap
template const bool HeterogeneousMap::get<bool>(const std::string key) const;
template const int HeterogeneousMap::get<int>(const std::string key) const;
template const double
HeterogeneousMap::get<double>(const std::string key) const;
template const std::vector<std::complex<double>>
HeterogeneousMap::get<std::vector<std::complex<double>>>(
    const std::string key) const;
template const std::vector<double>
HeterogeneousMap::get<std::vector<double>>(const std::string key) const;
template const std::vector<double>
HeterogeneousMap::get_with_throw<std::vector<double>>(
    const std::string key) const;

template <typename... Types> class Variant : public mpark::variant<Types...> {

private:
  std::string originalExpression = "";

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

  template <typename To, typename From> class CastVisitor {
  public:
    To operator()(const From &t) const { return (To)t; }
  };

public:
  Variant() : mpark::variant<Types...>() {}
  template <typename T>
  Variant(T &element) : mpark::variant<Types...>(element) {}
  template <typename T>
  Variant(T &&element) : mpark::variant<Types...>(element) {}
  Variant(const Variant &element)
      : mpark::variant<Types...>(element),
        originalExpression(element.originalExpression) {}

  template <typename T> T as() const {
    try {
      // First off just try to get it
      return mpark::get<T>(*this);
    } catch (std::exception &e) {
      std::stringstream s;
      s << "InstructionParameter::this->toString() = " << toString() << "\n";
      s << "This InstructionParameter type id is " << this->which() << "\n";
      XACCLogger::instance()->error("Cannot cast Variant: " + s.str());
      print_backtrace();
      exit(0);
    }
    return T();
  }

  template <typename T> T as_no_error() const {
    // First off just try to get it
    return mpark::get<T>(*this);
  }

  int which() const { return this->index(); }

  bool isNumeric() const {
    IsArithmeticVisitor v;
    return mpark::visit(v, *this);
  }
  void storeOriginalExpression(std::string expr) { originalExpression = expr; }
  void storeOriginalExpression() { originalExpression = toString(); }
  std::string getOriginalExpression() { return originalExpression; }
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
} // namespace xacc
#endif