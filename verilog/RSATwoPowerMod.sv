
import RSA_pkg::*;

module RSATwoPowerMod (
  // input
  input clk,
  input rst,

  // input data
  input i_valid,
  output i_ready,
  input RSATwoPowerModIn i_in,

  // output data
  output o_valid,
  input o_ready,
  output RSATwoPowerModOut o_out
);

typedef logic [MOD_WIDTH:0] ExtendKeyType;

// input data
IntType data_power;
ExtendKeyType data_modulus;

// round register
IntType round_counter;
ExtendKeyType round_result;

assign o_out = round_result[MOD_WIDTH-1:0];

// read input data
always_ff @( posedge clk or negedge rst ) begin
  if (!rst) begin
    data_power <= 0;
    data_modulus <= 0;
  end
  else begin
    if (i_ready && i_valid) begin
      data_power <= i_in.power;
      data_modulus <= {1'b0, i_in.modulus};
    end
  end
end

logic loop_ivalid;
logic loop_ovalid, loop_oready;
logic loop_init, loop_next;

// round_counter
always_ff @(posedge clk or negedge rst) begin
  if (loop_init) begin
    round_counter <= 0;
  end else if (loop_next) begin
    round_counter <= round_counter + 1;
  end else if (round_counter == data_power) begin
    round_counter <= 0;
  end
end

// round_result
always_ff @(posedge clk or negedge rst) begin
  if (loop_init) begin
    round_result <= 'b1;
  end
  else if (loop_next) begin
    round_result <= ((round_result << 1) > data_modulus) ?
    (round_result << 1) - data_modulus :
    round_result << 1;
  end
end

PipelineLoop i_loop(
  .clk(clk),
  .rst(rst),
  .i_valid(i_valid),
  .i_ready(i_ready),
  .i_cen(loop_init),
  .o_valid(loop_ovalid),
  .o_ready(loop_oready), // The operation will complete in one clock cycle, the output is always ready
  .o_done(round_counter == data_power),
  .o_cen(loop_next)
);

PipelineFilter i_filter(
  .i_valid(loop_ovalid),
  .i_ready(loop_oready),
  .i_pass(round_counter == data_power),
  .o_valid(o_valid),
  .o_ready(o_ready)
);

endmodule
