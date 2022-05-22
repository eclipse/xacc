#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <string>

namespace xacc {
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
} // namespace xacc
