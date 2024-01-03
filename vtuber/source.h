#pragma once

#include "verilog/dtype.h"
#include "verilog/serialize.h"
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
    ::std::vector<T> data;
    T t;
    while (verilog::LoadHexString(ist, t)) {
      data.push_back(t);
    }
    return data;
  }
};

template <typename T> class VintBinarySource : public Source<T> {
  static_assert(verilog::is_dtype_v<T>, "T must be a verilog type");

public:
  ::std::vector<T> get(::std::istream &ist) override {
    ::std::vector<T> data;
    T t;
    while (verilog::LoadBinary(ist, t)) {
      data.push_back(t);
    }
    return data;
  }
};