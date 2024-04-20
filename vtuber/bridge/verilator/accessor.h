#pragma once

#include "verilator_assign.h"
#include "verilog/dtype/vint.h"

template <typename T> struct Accessor {
  virtual void write(const T &t) = 0;
  virtual T read() const = 0;
};

template <typename PortType, typename DataType>
class VerilatorAccessor : public Accessor<DataType> {
private:
  PortType &ptr;

public:
  // Constructor
  VerilatorAccessor(PortType &t) : ptr(t) {}

  // Write to the pointer
  void write(const DataType &value) override {
    verilog::verilator::write_port(ptr, value);
  }

  // Read from the pointer
  DataType read() const override {
    DataType t;
    verilog::verilator::read_port(t, ptr);
    return t;
  }
};