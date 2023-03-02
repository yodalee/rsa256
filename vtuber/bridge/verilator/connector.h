#pragma once

class Connector {
public:
  virtual ~Connector() = default;
  virtual void before_clk() {}
  virtual bool after_clk() { return false; }
};