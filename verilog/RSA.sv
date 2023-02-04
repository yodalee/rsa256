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

endmodule