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
#include "Stream.hpp"
#include <memory>
#include <queue>
#include <functional>
#include <sstream>
#include <algorithm>
#include <map>
#include <chrono>

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


template <typename T> struct empty_delete {
  empty_delete() {}
  void operator()(T *const) const {}
};

// This function assumes ownership of pointer is taken care of elsewhere.
template<typename T> std::shared_ptr<T> as_shared_ptr(T* ptr) {
    return std::shared_ptr<T>(ptr, empty_delete<T>());
}

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
// Notify subscribers when the logging level was changed.
// This can be used to control logging level/verbosity of 
// external libraries (e.g. those used by plugins) to
// match that of XACC.
using LoggingLevelNotification = std::function<void(int)>;

class XACCLogger : public Singleton<XACCLogger> {

protected:
  std::shared_ptr<spdlog::logger> stdOutLogger;
  std::shared_ptr<spdlog::logger> fileLogger;
  
  bool useCout = false;
  
  // Should we log to file?
  bool useFile = false;

  bool useColor = true;

  MessagePredicate globalPredicate = []() { return true; };
  
  std::vector<LoggingLevelNotification> loggingLevelSubscribers;
  
  std::queue<std::string> logQueue;
  
  // Custom filename prefix (if logging to file)
  std::string logFileNamePrefix;

  XACCLogger();
  
  std::shared_ptr<spdlog::logger> getLogger() { 
    static bool fileLoggerUsed = false;
    if (!fileLoggerUsed && useFile) {
      createFileLogger();
      fileLoggerUsed = true;
    }

    return useFile ? fileLogger : stdOutLogger; 
  }

  // On-demand create a file logger:
  // We don't want to create one if not being used.
  void createFileLogger();

public:
  // Overriding here so we can have a custom constructor
  static XACCLogger *instance() {
    if (!instance_) {
      instance_ = new XACCLogger();
    }
    return instance_;
  }

  // If enable = true, switch to File logging (if not already logging to file).
  // If enable = false, stop logging to File if currently is.
  // This enables dev to scope a section which should log to File.
  // Optionally, a prefix can be specified to customize log file.
  void logToFile(bool enable, const std::string& fileNamePrefix = "");

  // Set level for log filtering:
  // 0: Errors and Warnings only
  // 1: Info and above
  // 2: Debug and above
  // Note: this will only take effect when xacc::verbose is set.
  void setLoggingLevel(int level);
  int getLoggingLevel();
  
  void subscribeLoggingLevel(LoggingLevelNotification onLevelChangeFn) { 
    loggingLevelSubscribers.emplace_back(onLevelChangeFn);
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

// Util timer to log execution elapsed time of a scope block.
// Example usage:
// (1) Time a function body:
/* 
void slowFunc {
  // Use __FUNCTION__ macro to get the function name 
  // Can use a custom string as well
  ScopeTimer timer(__FUNCTION__);
  .... code
}
*/
// The above timer will log when the function starts and ends (with elapsed time).
// (2) Time a code block, including function calls
/* 
 ... irrelevant code
 // Create a scope block:
 {
  ScopeTimer timer("human readable name of this block");
  .... code
 }
  ... irrelevant code
*/
// This will log the timing data of that specific code block.
class ScopeTimer {
public:
  ScopeTimer(const std::string& scopeName, bool shouldLog = true);
  double getDurationMs() const;
  ~ScopeTimer();
private:
  std::chrono::time_point<std::chrono::system_clock> m_startTime;
  bool m_shouldLog;
  std::string m_scopeName;
};

// container helper
namespace container {
  template<typename ContainerType, typename ElementType>
  bool contains(const ContainerType& container, const ElementType& item) {
    return std::find(container.begin(), container.end(), item) != container.end();
  }
} // namespace container

} // namespace xacc

#endif
