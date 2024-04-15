module RotSeq(
	input clk, rst,
	input [7:0] i,
	output logic [7:0] o
);

always_ff @(posedge clk or negedge rst) begin
	if (!rst) o <= '0;
	else      o <= i;
end

endmodule

module RotComb(
	input clk, rst,
	input [7:0] i,
	output logic [7:0] o
);

assign o = (i == 0) ? 'd123 : (i>>1);

endmodule
