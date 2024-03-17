
ALL: test

test: VDut
	g++ main.cpp -I /usr/share/verilator/include/ -I VDut -L VDut -l verilated -l VDut -lz -lglog -o SimPipe

VDut: VDut/VDut.mk
	make -C VDut -f VDut.mk

VDut/VDut.mk: Dut.v
	verilator --cc --top-module Dut Dut.v -Mdir VDut --trace-fst -y ../verilog

