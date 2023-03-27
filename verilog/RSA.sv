
import RSA_pkg::*;

module RSA (
  // input
  input clk,
  input rst,

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

// read input data
always_ff @( posedge clk or negedge rst) begin
  if (!rst) begin
    msg <= 0;
    key <= 0;
    modulus <= 0;
  end
  else begin
    if (i_valid && i_ready) begin
      msg <= i_in.msg;
      key <= i_in.key;
      modulus <= i_in.modulus;
    end
  end
end

KeyType packed_val;
logic packed_valid;
logic packed_ready;
IntType power;
assign power = MOD_WIDTH * 2;

RSATwoPowerMod i_two_power_mod (
  // input
  .clk(clk),
  .rst(rst),

  // input data
  .i_valid(i_valid),
  .i_ready(i_ready),
  .i_in({power, i_in.modulus}),

  // output data
  .o_valid(packed_valid),
  .o_ready(packed_ready),
  .o_out(packed_val)
);

RSAMont i_RSAMont (
  // input
  .clk(clk),
  .rst(rst),

  // input data
  .i_valid(packed_valid),
  .i_ready(packed_ready),
  .i_base(packed_val), // 2 ^ 2n mod N
  .i_msg(msg), // msg
  .i_key(key),
  .i_modulus(modulus),

  // output data
  .o_valid(o_valid),
  .o_ready(o_ready),
  .o_crypto(o_out)
);

endmodule
