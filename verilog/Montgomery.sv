import RSA_pkg::*;

module Montgomery #(
	parameter MOD_WIDTH = 256
) (
	// input
	input clk,
	input rst_n,

	// input data
	input i_valid,
	output i_ready,
	input MontgomeryIn i_in,

	// output data
	output o_valid,
	input o_ready,
	output MontgomeryOut o_out
);

typedef logic [MOD_WIDTH + 2 - 1:0] ExtendKeyType;

ExtendKeyType data_a, data_b, data_modulus;
ExtendKeyType round_result, round_result_next;

ExtendKeyType mod_result;
assign mod_result = round_result > data_modulus ? round_result - data_modulus : round_result;
assign o_out = mod_result[MOD_WIDTH - 1 : 0];

// loop variable
logic loop_o_valid, loop_o_ready;
logic loop_init, loop_next;
logic loop_done = round_counter == MOD_WIDTH;

// read input data
always_ff @( posedge clk or negedge rst_n ) begin
  if (!rst_n) begin
    data_a <= 0;
    data_b <= 0;
    data_modulus <= 0;
  end
  else begin
    if (loop_init) begin
      data_a <= {2'b0, i_in.a};
      data_b <= {2'b0, i_in.b};
      data_modulus <= {2'b0, i_in.modulus};
    end
  end
end

// round_counter
logic [$clog2(MOD_WIDTH+1)-1:0] round_counter;

always_ff @(posedge clk) begin
  if (loop_init) begin
    round_counter <= 0;
  end
  else if (loop_next) begin
    round_counter <= round_counter + 1;
  end
end

// round_result
always_ff @(posedge clk) begin
  if (loop_init) begin
    round_result <= 0;
  end
  else begin
    round_result <= round_result_next;
  end
end

always_comb begin
  if (loop_done) begin round_result_next = round_result; end
  else begin
    round_result_next = round_result;
    if (data_a[round_counter]) begin
      round_result_next = round_result + data_b;
    end
    if (round_result_next[0]) begin
      round_result_next += data_modulus;
    end
    round_result_next >>= 1;
  end
end

PipelineLoop i_loop(
  .clk(clk),
  .rst_n(rst_n),
  .i_valid(i_valid),
  .i_ready(i_ready),
  .i_cen(loop_init),
  .o_valid(loop_o_valid),
  .o_ready(loop_o_ready),
  .o_done(loop_done),
  .o_cen(loop_next)
);

PipelineFilter i_filter(
  .i_valid(loop_o_valid),
  .i_ready(loop_o_ready),
  .i_pass(loop_done),
  .o_valid(o_valid),
  .o_ready(o_ready)
);

endmodule
