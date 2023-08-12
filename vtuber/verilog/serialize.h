#pragma once
#include "operation/pack.h"
#include "operation/unpack.h"
#include <iostream>

namespace verilog {

template <class T>::std::ostream &SaveBinary(::std::ostream &os, const T &rhs) {
  static_assert(::std::is_trivially_copyable_v<T>);
  os.write(reinterpret_cast<const char *>(&rhs), sizeof(T));
  return os;
};

template <class T>::std::istream &LoadBinary(::std::istream &ist, T &rhs) {
  static_assert(::std::is_trivially_copyable_v<T>);
  ist.read(reinterpret_cast<char *>(&rhs), sizeof(T));
  return ist;
};

template <class T>
::std::ostream &SaveHexString(::std::ostream &os, const T &rhs) {
  static_assert(verilog::is_dtype_v<T>);
  os.write(to_hex(pack(rhs)));
  return os;
}

template <class T>::std::istream &LoadHexString(::std::istream &ist, T &rhs) {
  static_assert(verilog::is_dtype_v<T>);
  verilog::vuint<verilog::bits<T>()> buf;
  ::std::string line;
  ::std::getline(ist, line);
  from_string(buf, line, 16);
  verilog::unpack(rhs, buf);
  return ist;
}

} // namespace verilog
