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
#ifndef XACC_UTILS_UTILS_HPP_
#define XACC_UTILS_UTILS_HPP_

#include "Singleton.hpp"

#include <memory>
#include <queue>
#include <functional>
#include <sstream>
#include <algorithm>
#include <map>

namespace spdlog {
class logger;
}
namespace xacc {

void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);

bool fileExists(const std::string &name);
bool directoryExists(const std::string path);

void print_backtrace();


template <class Op> void split(const std::string &s, char delim, Op op) {
  std::stringstream ss(s);
  for (std::string item; std::getline(ss, item, delim);) {
    *op++ = item;
  }
}

//------------------------------------------------------------------------------
// Name: split
//------------------------------------------------------------------------------
inline std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, std::back_inserter(elems));
  return elems;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::pair<T, T> &p) {
  os << "[" << p.first << "," << p.second << "]";
  return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
  os << "[";
  for (int i = 0; i < v.size(); ++i) {
    os << v[i];
    if (i != v.size() - 1)
      os << ",";
  }
  os << "]";
  return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::map<int, T> &m) {
  os << "[";
  int i = 0;
  for (auto &kv : m) {
    os << "(" << kv.first << "," << kv.second << ")";
    if (i != m.size() - 1) {
      os << ",";
    }
    i++;
  }
  os << "]";
  return os;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::map<std::string, T> &m) {
  os << "[";
  int i = 0;
  for (auto &kv : m) {
    os << "(" << kv.first << "," << kv.second << ")";
    if (i != m.size() - 1) {
      os << ",";
    }
    i++;
  }
  os << "]";
  return os;
}
static inline bool is_base64(unsigned char c);

std::string base64_decode(std::string const &encoded_string);

template <typename TupleType, typename FunctionType>
void tuple_for_each(
    TupleType &&, FunctionType,
    std::integral_constant<
        size_t, std::tuple_size<
                    typename std::remove_reference<TupleType>::type>::value>) {}

template <std::size_t I, typename TupleType, typename FunctionType,
          typename = typename std::enable_if<
              I != std::tuple_size<typename std::remove_reference<
                       TupleType>::type>::value>::type>
void tuple_for_each(TupleType &&t, FunctionType f,
                    std::integral_constant<size_t, I>) {
  f(std::get<I>(t));
  tuple_for_each(std::forward<TupleType>(t), f,
                 std::integral_constant<size_t, I + 1>());
}

template <typename TupleType, typename FunctionType>
void tuple_for_each(TupleType &&t, FunctionType f) {
  tuple_for_each(std::forward<TupleType>(t), f,
                 std::integral_constant<size_t, 0>());
}

using MessagePredicate = std::function<bool(void)>;

class XACCLogger : public Singleton<XACCLogger> {

protected:
  std::shared_ptr<spdlog::logger> logger;

  bool useCout = false;

  bool useColor = true;

  MessagePredicate globalPredicate = []() { return true; };

  std::queue<std::string> logQueue;

  XACCLogger();

public:
  // Overriding here so we can have a custom constructor
  static XACCLogger *instance() {
    if (!instance_) {
      instance_ = new XACCLogger();
    }
    return instance_;
  }

  void enqueueLog(const std::string log) { logQueue.push(log); }

  void dumpQueue() {
    while (!logQueue.empty()) {
      info(logQueue.front());
      logQueue.pop();
    }
  }
  void setGlobalLoggerPredicate(MessagePredicate pred) {
    globalPredicate = pred;
  }
  void info(const std::string &msg,
            MessagePredicate predicate = std::function<bool(void)>([]() {
              return true;
            }));
  void warning(const std::string &msg,
               MessagePredicate predicate = std::function<bool(void)>([]() {
                 return true;
               }));
  void debug(const std::string &msg,
             MessagePredicate predicate = std::function<bool(void)>([]() {
               return true;
             }));
  void error(const std::string &msg,
             MessagePredicate predicate = std::function<bool(void)>([]() {
               return true;
             }));
};


template <typename T> std::vector<T> linspace(T a, T b, size_t N) {
  T h = (b - a) / static_cast<T>(N - 1);
  std::vector<T> xs(N);
  typename std::vector<T>::iterator x;
  T val;
  for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
    *x = val;
  return xs;
}

// container helper
namespace container {
  template<typename ContainerType, typename ElementType>
  bool contains(const ContainerType& container, const ElementType& item) {
    return std::find(container.begin(), container.end(), item) != container.end();
  }
} // namespace container

} // namespace xacc

#endif
