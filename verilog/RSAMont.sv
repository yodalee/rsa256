
import RSA_pkg::*;

module RSAMont (
  // input
  input clk,
  input rst,

  // input data
  input i_valid,
  output i_ready,
  input RSAMontModIn i_in,

  // output data
  output o_valid,
  input o_ready,
  output RSAMontModOut o_out
);

localparam N_FANOUT = 2;

KeyType base, msg, key, modulus; // input
KeyType square, multiply;  // calculate result

logic [$clog2(2 * MOD_WIDTH+1)-1:0] round_counter;
logic [$clog2(2 * MOD_WIDTH+1)-1:0] key_idx;
assign key_idx = (round_counter - 2) >> 1;
assign o_out = multiply;

// read input data
always_ff @( posedge clk or negedge rst ) begin
  if (!rst) begin
    base <= 0;
    msg <= 0;
    key <= 0;
    modulus <= 0;
  end
  else if (i_ready && i_valid) begin
    base <= i_in.base;       // 2 ^ 2n mod N
    msg <= i_in.msg;         // msg
    key <= i_in.key;         // key
    modulus <= i_in.modulus; // N
  end
end

// round_counter
always_ff @(posedge clk or negedge rst) begin
  if (loop_init) begin
    round_counter <= 0;
  end
  else if (loop_next) begin
    round_counter <= round_counter + 1;
  end
end

// data
always_ff @(posedge clk) begin
  if (montgomery_o_valid) begin
    if (round_counter == 1) begin
      square <= montgomery_out;
      multiply <= 1;
    end
    else if (round_counter[0] == 1'b0) begin
      multiply <= key[key_idx[$clog2(MOD_WIDTH) - 1:0]] == 1'b1 ? montgomery_out : multiply;
      square <= square;
    end
    else begin
      square <= montgomery_out;
      multiply <= multiply;
    end
  end
end

always_comb begin
  if (round_counter == 0) begin
    montgomery_in_a = base;
    montgomery_in_b = msg;
  end
  else if (round_counter[0] == 1'b1) begin
    montgomery_in_a = square;
    montgomery_in_b = multiply;
  end
  else begin
    montgomery_in_a = square;
    montgomery_in_b = square;
  end
end

logic montgomery_o_valid;
KeyType montgomery_in_a, montgomery_in_b, montgomery_out;

logic [N_FANOUT-1:0] dist_o_valid, dist_o_ready;
logic loop_i_valid, loop_i_ready, loop_o_valid, loop_o_ready;
logic loop_init, loop_next, loop_done;
// check stop condition:
// 1 round for packed message
// 256 * 2 round: odd round for multiply, even round for square
assign loop_done = round_counter == MOD_WIDTH * 2 + 2;

PipelineLoop i_loop(
  .clk(clk),
  .rst(rst),
  .i_valid(i_valid),
  .i_ready(i_ready),
  .i_cen(loop_init),
  .o_valid(loop_o_valid),
  .o_ready(loop_o_ready),
  .o_done(loop_done),
  .o_cen(loop_next)
);

PipelineDistribute #(.N(N_FANOUT)) i_dist (
  .clk(clk),
  .rst(rst),
  .i_valid(loop_o_valid),
  .i_ready(loop_o_ready),
  .o_valid(dist_o_valid),
  .o_ready(dist_o_ready)
);

RSAMontgomery i_montgomery(
  // input
  .clk(clk),
  .rst(rst),

  // input data
  .i_valid(dist_o_valid[0]),
  .i_ready(dist_o_ready[0]),
  .i_in({montgomery_in_a, montgomery_in_b, modulus}),

  // output data
  .o_valid(montgomery_o_valid),
  .o_ready(1'b1), // always ready
  .o_out(montgomery_out)
);

PipelineFilter i_filter(
    .i_valid(dist_o_valid[1]),
    .i_ready(dist_o_ready[1]),
    .i_pass(loop_done),
    .o_valid(o_valid),
    .o_ready(o_ready)
);

endmodule