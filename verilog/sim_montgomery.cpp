
#include "Vmontgomery.h"
#include "assign_port.h"
#include "callback.h"
#include "dut_wrapper.h"
#include "model_rsa.h"
#include "scoreboard.h"
#include "verilog_int.h"
#include <iostream>
#include <systemc>

using namespace std;
using namespace sc_core;

using DUT = Vmontgomery;

int sc_main(int, char **) {
  sc_start(1200.0, SC_NS);
  return 0;
}
