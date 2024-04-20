module Dut(
	input clk,
	input rst_n,

	input               i_valid,
	output logic        i_ready,
	input  logic [31:0] i_data,

	output logic        o_valid,
	input               o_ready,
	output logic [31:0] o_data
);

logic o_en;
Pipeline u_pp(
	.clk(clk),
	.rst_n(rst_n),

	.i_valid(i_valid),
	.i_ready(i_ready),
	.o_en(o_en),

	.o_valid(o_valid),
	.o_ready(o_ready)
);

always_ff @(posedge clk or negedge rst_n) begin
	if (!rst_n) begin
		o_data <= '0;
	end else if (o_en) begin
		o_data <= i_data;
	end
end

endmodule
