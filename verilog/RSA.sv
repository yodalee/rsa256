`include "RSA_pkg.sv"

import RSA_pkg::*;

module RSA (
	// input
	input clk,
	input rst,

	// input data
	input i_valid,
	output i_ready,
	input KeyType i_msg,
	input KeyType i_key,
	input KeyType i_modulus,

	// output data
	output o_valid,
	input o_ready,
	output KeyType o_crypto
);

logic two_power_mod_in_valid;
logic two_power_mod_in_ready;
KeyType two_power_mod_in_modulus;
IntType two_power_mod_in_power;
logic two_power_mod_out_valid;
logic two_power_mod_out_ready;
KeyType two_power_mod_out;

RSATwoPowerMod i_two_power_mod (
	// input
	.clk(clk),
	.rst(rst),

	// input data
	.i_valid(two_power_mod_in_valid),
	.i_ready(two_power_mod_in_ready),
	.i_modulus(two_power_mod_in_modulus),
	.i_power(two_power_mod_in_power),

	// output data
	.o_valid(two_power_mod_out_valid),
	.o_ready(two_power_mod_out_ready),
	.o_out(two_power_mod_out)
);

logic montgomery_in_valid;
logic montgomery_in_ready;
KeyType montgomery_in_a;
KeyType montgomery_in_b;
KeyType montgomery_in_modulus;
logic montgomery_out_valid;
logic montgomery_out_ready;
KeyType montgomery_out;

RSAMontgomery i_montgomery (
	// input
	.clk(clk),
	.rst(rst),

	// input data
	.i_valid(montgomery_in_valid),
	.i_ready(montgomery_in_ready),
	.i_a(montgomery_in_a),
	.i_b(montgomery_in_b),
	.i_modulus(montgomery_in_modulus),

	// output data
	.o_valid(montgomery_out_valid),
	.o_ready(montgomery_out_ready),
	.o_out(montgomery_out)
);

endmodule