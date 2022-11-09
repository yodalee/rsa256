
#include "Vdummy.h"
#include "dut_wrapper.h"
#include <iostream>
#include <systemc>

using namespace std;
using namespace sc_core;

SC_MODULE(Testbench) {
  sc_clock clk;
  DUTWrapper<Vdummy> dut_wrapper;

public:
  SC_HAS_PROCESS(Testbench);
  Testbench(const sc_module_name &name)
      : sc_module(name), clk("clk", 1.0, SC_NS),
        dut_wrapper("dut_wrapper") {
    dut_wrapper.clk(clk);
    dut_wrapper.register_callback(CallbackPhase::Init,
                                  [](Vdummy *dut) { cout << "Init" << endl; });
    dut_wrapper.register_callback(CallbackPhase::BeforeClk, [](Vdummy *dut) {
      cout << "Before clk" << endl;
    });
    dut_wrapper.register_callback(CallbackPhase::AfterClk, [](Vdummy *dut) {
      cout << "After clk" << endl;
    });
  }
};

int sc_main(int, char **) {
  unique_ptr<Testbench> testbench(new Testbench("testbench"));
  sc_start(10.0, SC_NS);
  return 0;
}
