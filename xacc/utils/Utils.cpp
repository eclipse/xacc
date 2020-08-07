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
#include "Utils.hpp"
#include <unistd.h>
#include "spdlog/spdlog.h"
#include "RuntimeOptions.hpp"

#include <iostream>
#include <sstream>
#include <istream>
#include <iomanip>
#include <dirent.h>
#include "xacc_config.hpp"

#ifdef HAS_LIBUNWIND
#include <libunwind.h>
#include <cxxabi.h>
#endif

namespace xacc {

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz"
                                        "0123456789+/";

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

bool fileExists(const std::string &name) {
  if (FILE *file = fopen(name.c_str(), "r")) {
    fclose(file);
    return true;
  } else {
    return false;
  }
}

bool directoryExists(const std::string path) {
  auto p = path.c_str();
  if (p == NULL)
    return false;

  DIR *pDir;
  bool bExists = false;

  pDir = opendir(p);

  if (pDir != NULL) {
    bExists = true;
    (void)closedir(pDir);
  }

  return bExists;
}

bool makeDirectory(const std::string& path) {
  int ret = mkdir(path.c_str(), S_IRWXU | S_IRGRP |  S_IXGRP | S_IROTH | S_IXOTH);
  return (ret == 0);
}

std::string getCurrentTimeForFileName() {
  auto time = std::time(nullptr);
  std::stringstream ss;
  // ISO 8601 without timezone information.
  ss << std::put_time(std::localtime(&time), "%F_%T"); 
  auto s = ss.str();
  std::replace(s.begin(), s.end(), ':', '-');
  return s;
}

void print_backtrace() {
#ifdef HAS_LIBUNWIND
  unw_cursor_t cursor;
  unw_context_t context;

  // Initialize cursor to current frame for local unwinding.
  unw_getcontext(&context);
  unw_init_local(&cursor, &context);

  // Unwind frames one by one, going up the frame stack.
  while (unw_step(&cursor) > 0) {
    unw_word_t offset, pc;
    unw_get_reg(&cursor, UNW_REG_IP, &pc);
    if (pc == 0) {
      break;
    }
    std::printf("0x%lx:", pc);

    char sym[256];
    if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
      char *nameptr = sym;
      int status;
      char *demangled = abi::__cxa_demangle(sym, nullptr, nullptr, &status);
      if (status == 0) {
        nameptr = demangled;
      }
      std::printf(" (%s+0x%lx)\n", nameptr, offset);
      std::free(demangled);
    } else {
      std::printf(" -- error: unable to obtain symbol name for this frame\n");
    }
  }
#endif
}

std::string base64_decode(std::string const &encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && (encoded_string[in_] != '=') &&
         is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_];
    in_++;
    if (i == 4) {
      for (i = 0; i < 4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] =
          (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] =
          ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++)
      char_array_4[j] = 0;

    for (j = 0; j < 4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] =
        ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++)
      ret += char_array_3[j];
  }

  return ret;
}

void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  [](int ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       [](int ch) { return !std::isspace(ch); })
              .base(),
          s.end());
}

// trim from both ends (in place)
void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}
XACCLogger::XACCLogger()
    : useColor(!RuntimeOptions::instance()->exists("no-color")),
      useCout(RuntimeOptions::instance()->exists("use-cout")),
      useFile(RuntimeOptions::instance()->exists("use-file")) {
  // Create a std::out logger instance
  std::string loggerName = "xacc-logger";
  if (RuntimeOptions::instance()->exists("logger-name")) {
    loggerName = (*RuntimeOptions::instance())["logger-name"];
  }
  auto _log = spdlog::get(loggerName);
  if (_log) {
    stdOutLogger = _log;
  } else {
    stdOutLogger = spdlog::stdout_logger_mt(loggerName);
  }

  stdOutLogger->set_level(spdlog::level::info);
}

void XACCLogger::createFileLogger() {
  // Create a file logger instance
  std::string loggerName = "xacc-file-logger";
  auto _log = spdlog::get(loggerName);
  if (_log) {
    fileLogger = _log;
  } else {
    const std::string rootPath(XACC_INSTALL_DIR);
    // We'll put the log file (timestamped) to the "logs" sub-directory:
    std::string logDir = rootPath + "/logs";
    if (!directoryExists(logDir)) {
      if (!makeDirectory(logDir)) {
        // Cannot make the directory, use the current directory instead.
        logDir = ".";
      }
    }

    const std::string DEFAULT_FILE_NAME_PREFIX = "xacc_log_";
    const std::string DEFAULT_FILE_NAME_POSTFIX = ".txt";
    const std::string fileName = logFileNamePrefix + "_" + DEFAULT_FILE_NAME_PREFIX + getCurrentTimeForFileName() + DEFAULT_FILE_NAME_POSTFIX;
    // Create a file logger using the timestamped filename in the logs folder.
    fileLogger = spdlog::basic_logger_mt(loggerName, logDir + "/" + fileName);
  }

  fileLogger->set_level(spdlog::level::info);
}

void XACCLogger::logToFile(bool enable, const std::string& fileNamePrefix) {
  // Switching the current setting
  if (enable != useFile) {
    // Always dump any enqueued messages before switching.
    dumpQueue();
    // Set runtime *useFile* flag, this will redirect 
    // log message to the appropriate logger.
    useFile = enable;
  }
  logFileNamePrefix = fileNamePrefix;
}

void XACCLogger::setLoggingLevel(int level) {
  if (level < 0 || level > 2) {
    // Ignored
    return;
  }
  // Converted the level to the spd-log enum
  if (level == 0) {
    // Warning and above
    getLogger()->set_level(spdlog::level::warn);
  }

  if (level == 1) {
    // Info and above
    getLogger()->set_level(spdlog::level::info);
  }

  if (level == 2) {
    // Debug and above
    getLogger()->set_level(spdlog::level::debug);
  }

  // Notify subscribers
  for (const auto& callback : loggingLevelSubscribers) {
    try {
      callback(level);
    }
    catch (...) {
      // Do nothing. This is to prevent any rogue subscribers.
    }
  }
}

int XACCLogger::getLoggingLevel() {
  const auto spdLevel = getLogger()->level();
  switch (spdLevel) {
  case spdlog::level::trace:
  case spdlog::level::debug: return 2;
  case spdlog::level::info: return 1;
  default:
    return 0;
  }
}

void XACCLogger::info(const std::string &msg, MessagePredicate predicate) {
  if (useCout) {
    if (predicate() && globalPredicate()) {
      if (useColor) {
        std::cout << "\033[1;34m[XACC Info] " + msg + "\033[0m \n";
      } else {
        std::cout << "[XACC Info] " + msg + "\n";
      }
    }
  } else {
    if (predicate() && globalPredicate()) {
      if (useColor & !useFile) {
        getLogger()->info("\033[1;34m" + msg + "\033[0m");
      } else {
        getLogger()->info(msg);
      }
    }
  }
}

void XACCLogger::warning(const std::string &msg, MessagePredicate predicate) {
  if (useCout) {
    if (predicate() && globalPredicate()) {
      if (useColor) {
        std::cout << "\033[1;33m[XACC Warning] " + msg + "\033[0m \n";
      } else {
        std::cout << "[XACC Warning] " + msg + "\n";
      }
    }
  } else {
    if (predicate() && globalPredicate()) {
      if (useColor & !useFile) {
        getLogger()->info("\033[1;33m" + msg + "\033[0m");
      } else {
        getLogger()->info(msg);
      }
    }
  }
}

void XACCLogger::debug(const std::string &msg, MessagePredicate predicate) {
  if (useCout) {
    if (predicate() && globalPredicate()) {
      if (useColor) {
        std::cout << "\033[1;32m[XACC Debug] " + msg + "\033[0m \n";
      } else {
        std::cout << "[XACC Debug] " + msg + "\n";
      }
    }
  } else {
    if (predicate() && globalPredicate()) {
      if (useColor & !useFile) {
        getLogger()->info("\033[1;32m" + msg + "\033[0m");
      } else {
        getLogger()->info(msg);
      }
    }
  }
}
void XACCLogger::error(const std::string &msg, MessagePredicate predicate) {
  if (useCout) {
    if (predicate() && globalPredicate())
      std::cerr << msg << "\n";
  } else {
    if (predicate() && globalPredicate()) {
      getLogger()->error("\033[1;31m[XACC Error] " + msg + "\033[0m");
    }
  }
}

ScopeTimer::ScopeTimer(const std::string& scopeName, bool shouldLog):
  m_startTime(std::chrono::system_clock::now()),
  m_shouldLog(shouldLog),
  m_scopeName(scopeName)
  {
    if (m_shouldLog) {
      XACCLogger::instance()->info("'" + scopeName + "' started.");
    }
  }

double ScopeTimer::getDurationMs() const {
  return static_cast<double>(
    std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - m_startTime).count()/1000.0);
}

ScopeTimer::~ScopeTimer() {
  const double elapsedTime = getDurationMs();
  if (m_shouldLog) {
    XACCLogger::instance()->info("'" + m_scopeName + "' finished [" + std::to_string(elapsedTime) + " ms].");
  }
}
} // namespace xacc
