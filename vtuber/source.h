#pragma once

#include <fstream>
#include <string>
#include <vector>

template <typename T> class Source {
public:
  Source() {}
  virtual ~Source() = default;
  virtual ::std::vector<T> get() = 0;
};

template <typename T> class VintLineSource : public Source<T> {
  static_assert(verilog::is_vint_v<T>,
                "T must be a vint type, not array, struct, or union");

public:
  VintLineSource(const ::std::string &filename_) : Source<T>() {
    filename = filename_;
  }

  ::std::vector<T> get() override {
    ::std::vector<T> data;
    ::std::string line;
    ::std::ifstream inputFile(filename);
    while (std::getline(inputFile, line)) {
      T t(line, 16);
      data.push_back(t);
    }
    inputFile.close();
    return data;
  }

private:
  ::std::string filename;
};