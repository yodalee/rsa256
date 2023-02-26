
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

typedef enum logic {
  STATE_IDLE = 0,
  STATE_WAITDONE = 1
} State_t;
State_t state, state_next;

KeyType msg, key, modulus;

assign i_ready = state == STATE_IDLE;

// update state
always_ff @(posedge clk or negedge rst) begin
  if (!rst) begin
    state <= STATE_IDLE;
  end
  else begin
    case (state)
    STATE_IDLE: begin
      if (i_valid) begin
        state <= STATE_WAITDONE;
        // kick start the two_power_mod
        two_power_mod_in_valid <= 1'b1;
      end
    end
    STATE_WAITDONE: begin
      two_power_mod_in_valid <= 1'b0;
      if (o_ready && o_valid) begin
        state <= STATE_IDLE;
      end
    end
    endcase
  end
end

// read input data
always_ff @( posedge clk or negedge rst ) begin
  if (!rst) begin
    msg <= 0;
    key <= 0;
    modulus <= 0;
    two_power_mod_in_valid <= 0;
  end
  else if (state == STATE_IDLE && i_valid) begin
    msg <= i_in.msg;
    key <= i_in.key;
    modulus <= i_in.modulus;
    // kick start the two_power_mod
    two_power_mod_in_valid <= 1'b1;
  end
  else if (state == STATE_WAITDONE) begin
    msg <= msg;
    key <= key;
    modulus <= modulus;
    two_power_mod_in_valid <= 1'b0;
  end
  else begin
    msg <= msg;
    key <= key;
    modulus <= modulus;
    two_power_mod_in_valid <= two_power_mod_in_valid;
  end
end

logic two_power_mod_in_valid;
logic two_power_mod_in_ready;
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
  .i_valid(two_power_mod_in_valid),
  .i_ready(two_power_mod_in_ready),
  .i_in({power, modulus}),

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
