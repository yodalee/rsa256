
import RSA_pkg::*;

module RSA (
  // input
  input clk,
  input rst_n,

  // input data
  input i_valid,
  output i_ready,
  input RSAModIn i_in,

  // output data
  output o_valid,
  input o_ready,
  output RSAModOut o_out
);

KeyType msg, key, modulus;
logic i_en;
logic s1_i_valid, s1_i_ready;
logic s1_o_valid, s1_o_ready;

Pipeline pipeline_input (
  .clk(clk),
  .rst_n(rst_n),
  .i_valid(i_valid),
  .i_ready(i_ready),
  .o_en(i_en),
  .o_valid(s1_i_valid),
  .o_ready(s1_i_ready)
);

// read input data
always_ff @( posedge clk or negedge rst_n) begin
  if (!rst_n) begin
    msg <= 0;
    key <= 0;
    modulus <= 0;
  end
  else begin
    if (i_en) begin
      msg <= i_in.msg;
      key <= i_in.key;
      modulus <= i_in.modulus;
    end
  end
end

IntType power;
assign power = MOD_WIDTH * 2;
KeyType packed_val;
logic s1_dist_valid[2], s1_dist_ready[2];
logic s1_comb_valid[2], s1_comb_ready[2];

logic s1_en;
KeyType s1_msg, s1_key, s1_modulus;

PipelineDistribute #(.N(2)) i_dist (
    .clk(clk),
    .rst_n(rst_n),
    .i_valid(s1_i_valid),
    .i_ready(s1_i_ready),
    .o_valid(s1_dist_valid),
    .o_ready(s1_dist_ready)
);

Pipeline pipeline_stg1 (
  .clk(clk),
  .rst_n(rst_n),
  .i_valid(s1_dist_valid[0]),
  .i_ready(s1_dist_ready[0]),
  .o_en(s1_en),
  .o_valid(s1_comb_valid[0]),
  .o_ready(s1_comb_ready[0])
);

// read input data
always_ff @( posedge clk or negedge rst_n) begin
  if (!rst_n) begin
    s1_msg <= 0;
    s1_key <= 0;
    s1_modulus <= 0;
  end
  else begin
    if (s1_en) begin
      s1_msg <= msg;
      s1_key <= key;
      s1_modulus <= modulus;
    end
  end
end


TwoPower i_twopower (
  // input
  .clk(clk),
  .rst_n(rst_n),

  // input data
  .i_valid(s1_dist_valid[1]),
  .i_ready(s1_dist_ready[1]),
  .i_in({power, modulus}),

  // output data
  .o_valid(s1_comb_valid[1]),
  .o_ready(s1_comb_ready[1]),
  .o_out(packed_val)
);

PipelineCombine #(.N(2)) i_comb (
    .i_valid(s1_comb_valid),
    .i_ready(s1_comb_ready),
    .o_valid(s1_o_valid),
    .o_ready(s1_o_ready)
);


RSAMont i_RSAMont (
  // input
  .clk(clk),
  .rst_n(rst_n),

  // input data
  .i_valid(s1_o_valid),
  .i_ready(s1_o_ready),
  .i_in({packed_val, s1_msg, s1_key, s1_modulus}),

  // output data
  .o_valid(o_valid),
  .o_ready(o_ready),
  .o_out(o_out)
);

endmodule
