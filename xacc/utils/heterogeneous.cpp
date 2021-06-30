#include "heterogeneous.hpp"
#include "Utils.hpp"
namespace xacc {
void emit_error(const std::string&& s) {
  XACCLogger::instance()->error(s);
  print_backtrace();
}
} // namespace xacc