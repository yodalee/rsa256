#pragma once

#include "verilog/dtype.h"
#include <iostream>
#include <string>
#include <vector>

template <typename T> class Source {
public:
  Source() {}
  virtual ~Source() = default;
  virtual ::std::vector<T> get(::std::istream &ist) = 0;
};

template <typename T> class VintHexSource : public Source<T> {
  static_assert(verilog::is_dtype_v<T>, "T must be a verilog type");

public:
  ::std::vector<T> get(::std::istream &ist) override {
    verilog::vuint<verilog::bits<T>()> buf;
    ::std::vector<T> data;
    ::std::string line;
    while (::std::getline(ist, line)) {
      from_string(buf, line, 16);
      T t;
      verilog::unpack(t, buf);
      data.push_back(t);
    }
    return data;
  }
};