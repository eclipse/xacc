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
namespace xacc {

template <class...> struct type_list {};

template <class... TYPES> struct visitor_base {
  using types = xacc::type_list<TYPES...>;
};

class HeterogeneousMap {
public:
  HeterogeneousMap() = default;
  HeterogeneousMap(const HeterogeneousMap &_other) { *this = _other; }

  template <typename T> void loop_pairs(T value) {
    insert(value.first, value.second);
  }

  template <typename First, typename... Rest>
  void loop_pairs(First firstValue, Rest... rest) {
    loop_pairs(firstValue);
    loop_pairs(rest...);
  }

  template <typename... TYPES> HeterogeneousMap(TYPES &&... list) {
    loop_pairs(list...);
  }

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
  template <class T> const T &get(const std::string key) const {
    if (!items<T>.count(this) && !items<T>[this].count(key)) {
      XACCLogger::instance()->error("Invalid type (" +
                                    std::string(typeid(T).name()) +
                                    ") or key (" + key + ").");
      print_backtrace();
    }
    return items<T>[this][key];
  }

  template <class T> const T &get_with_throw(const std::string key) const {
    if (!items<T>.count(this) && !items<T>[this].count(key)) {
      throw new std::runtime_error("Invalid type (" +
                                   std::string(typeid(T).name()) + ").");
    }
    return items<T>[this][key];
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

  template<typename T>
  bool keyExists(const std::string key) const {
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

  template <class T> void visit(T &&visitor) {
    visit_impl(visitor, typename std::decay_t<T>::types{});
  }

private:
  template <class T>
  static std::unordered_map<const HeterogeneousMap *, std::map<std::string, T>>
      items;

  template <class T, class U>
  using visit_function =
      decltype(std::declval<T>().operator()(std::declval<U &>()));
  template <class T, class U>
  static constexpr bool has_visit_v =
      std::experimental::is_detected<visit_function, T, U>::value;

  template <class T, template <class...> class TLIST, class... TYPES>
  void visit_impl(T &&visitor, TLIST<TYPES...>) {
    using expander = int[];
    (void)expander{0, (void(visit_impl_help<T, TYPES>(visitor)), 0)...};
  }

  template <class T, class U> void visit_impl_help(T &visitor) {
    static_assert(has_visit_v<T, U>, "Visitors must provide a visit function "
                                     "accepting a reference for each type");
    for (auto &&element : items<U>[this]) {
      visitor(element);
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

  template <typename To, typename From>
  class CastVisitor {
      public:
      To operator()(const From & t) const {
          return (To) t;
      }
  };

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
        s << "InstructionParameter::this->toString() = " << toString() << "\n";
        s << "This InstructionParameter type id is " << this->which() << "\n";
        XACCLogger::instance()->error("Cannot cast Variant to ("+std::string(typeid(T).name())+"):\n" + s.str());
        print_backtrace();
        exit(0);
    }
    return T();
  }

  template <typename T> T as_no_error() const {
     // First off just try to get it
     return mpark::get<T>(*this);
  }

  int which() const {
      return this->index();
  }
  
  bool isNumeric() const {
    IsArithmeticVisitor v;
    return mpark::visit(v, *this);
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
} // namespace xacc
#endif