#pragma once

template <typename DUT> class Connector {
public:
  virtual ~Connector() = default;
  virtual void before_clk(DUT *dut) {}
  virtual bool after_clk(DUT *dut) { return false; }
  virtual bool is_pass(DUT *dut) { return true; }
};