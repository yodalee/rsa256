#pragma once

#include "verilog/dtype.h"
#include <fstream>
#include <optional>
#include <string>
#include <vector>

template <typename T> class Source {
public:
  Source() {}
  virtual ~Source() = default;
  virtual ::std::optional<::std::vector<T>> get() = 0;
};

template <typename T> class VintLineSource : public Source<T> {
  static_assert(verilog::is_dtype_v<T>, "T must be a verilog type");

public:
  VintLineSource(const ::std::string &filename_) : Source<T>() {
    filename = filename_;
  }

  ::std::optional<::std::vector<T>> get() override {
    ::std::string line;
    ::std::ifstream inputFile(filename);
    if (not inputFile.good()) {
      inputFile.close();
      return {};
    }
    ::std::vector<T> data;
    verilog::vuint<verilog::bits<T>()> buf;
    while (std::getline(inputFile, line)) {
      from_string(buf, line, 16);
      T t;
      verilog::unpack(t, buf);
      data.push_back(t);
    }
    inputFile.close();
    return data;
  }

private:
  ::std::string filename;
};