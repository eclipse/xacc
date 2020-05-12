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

#include <map>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <experimental/type_traits>

#include "variant.hpp"

#include "Utils.hpp"
#include <complex>

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
    clear_functions = _other.clear_functions;
    copy_functions = _other.copy_functions;
    size_functions = _other.size_functions;
    for (auto &&copy_function : copy_functions) {
      copy_function(_other, *this);
    }
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

  template <typename... Ts> void print(std::ostream &os) const {
    _internal_print_visitor<Ts...> v(os);
    visit(v);
  }

  template <class T> void insert(const std::string key, const T &_t) {
    // don't have it yet, so create functions for printing, copying, moving, and
    // destroying
    if (items<T>.find(this) == std::end(items<T>)) {
      clear_functions.emplace_back(
          [](HeterogeneousMap &_c) { items<T>.erase(&_c); });

      // if someone copies me, they need to call each copy_function and pass
      // themself
      copy_functions.emplace_back(
          [](const HeterogeneousMap &_from, HeterogeneousMap &_to) {
            items<T>[&_to] = items<T>[&_from];
          });
      size_functions.emplace_back(
          [](const HeterogeneousMap &_c) { return items<T>[&_c].size(); });
    }
    items<T>[this].insert({key, _t});
  }

  template <class T> T &get_mutable(const std::string key) const {
    if (!items<T>.count(this) && !items<T>[this].count(key)) {
      XACCLogger::instance()->error("Invalid type or key (" + key + ").");
      print_backtrace();
    }
    return items<T>[this][key];
  }

  template <class T> const T &get(const std::string key) const {
    if (!keyExists<T>(key)) {
      XACCLogger::instance()->error("Invalid type or key (" + key + ").");
      print_backtrace();
    }
    return items<T>[this][key];
  }

  template <class T> const T &get_with_throw(const std::string key) const {
    if (!items<T>.count(this) && !items<T>[this].count(key)) {
      throw new std::runtime_error("Invalid type.");
    }
    return items<T>[this][key];
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
  void clear() {
    for (auto &&clear_func : clear_functions) {
      clear_func(*this);
    }
  }

  template <class T> size_t number_of() const {
    auto iter = items<T>.find(this);
    if (iter != items<T>.cend())
      return items<T>[this].size();
    return 0;
  }

  template <typename T> bool keyExists(const std::string key) const {
    if (items<T>.count(this) && items<T>[this].count(key)) {
      return true;
    }
    return false;
  }

  size_t size() const {
    size_t sum = 0;
    for (auto &&size_func : size_functions) {
      sum += size_func(*this);
    }
    // gotta be careful about this overflowing
    return sum;
  }

  ~HeterogeneousMap() { clear(); }

  template <class T> void visit(T &&visitor) const {
    visit_impl(visitor, typename std::decay_t<T>::types{});
  }

private:
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

  template <class T>
  static std::unordered_map<const HeterogeneousMap *, std::map<std::string, T>>
      items;

  template <class T, class U>
  using visit_function = decltype(std::declval<T>().operator()(
      std::declval<const std::string &>(), std::declval<U &>()));
  template <class T, class U>
  static constexpr bool has_visit_v =
      std::experimental::is_detected<visit_function, T, U>::value;

  template <class T, template <class...> class TLIST, class... TYPES>
  void visit_impl(T &&visitor, TLIST<TYPES...>) const {
    using expander = int[];
    (void)expander{0, (void(visit_impl_help<T, TYPES>(visitor)), 0)...};
  }

  template <class T, class U> void visit_impl_help(T &visitor) const {
    static_assert(has_visit_v<T, U>, "Visitors must provide a visit function "
                                     "accepting a reference for each type");
    for (auto &&element : items<U>[this]) {
      visitor(element.first, element.second);
    }
  }

  std::vector<std::function<void(HeterogeneousMap &)>> clear_functions;
  std::vector<std::function<void(const HeterogeneousMap &, HeterogeneousMap &)>>
      copy_functions;
  std::vector<std::function<size_t(const HeterogeneousMap &)>> size_functions;
};

template <class T>
std::unordered_map<const HeterogeneousMap *, std::map<std::string, T>>
    HeterogeneousMap::items;

// Make sure these basic types are always instantiatied for HeterogeneousMap
template const bool &HeterogeneousMap::get<bool>(const std::string key) const;
template const int &HeterogeneousMap::get<int>(const std::string key) const;
template const double &
HeterogeneousMap::get<double>(const std::string key) const;
template const std::vector<std::complex<double>> &
HeterogeneousMap::get<std::vector<std::complex<double>>>(
    const std::string key) const;
template const std::vector<double> &
HeterogeneousMap::get<std::vector<double>>(const std::string key) const;
template const std::vector<double> &
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