
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

typedef enum logic {
  STATE_IDLE = 0,
  STATE_WAITDONE = 1
} State_t;
State_t state, state_next;

KeyType msg, key, modulus;

assign i_ready = state == STATE_IDLE;

// update state
// latch msg, key, modulus
always_latch @(posedge clk or negedge rst) begin
  if (!rst) begin
    state <= STATE_IDLE;
  end
  else begin
    case (state)
    STATE_IDLE: begin
      if (i_valid) begin
        state <= STATE_WAITDONE;
        msg <= i_msg;
        key <= i_key;
        modulus <= i_modulus;
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

logic two_power_mod_in_valid;
logic two_power_mod_in_ready;
KeyType packed_val;

logic packed_valid;
logic packed_ready;

RSATwoPowerMod i_two_power_mod (
  // input
  .clk(clk),
  .rst(rst),

  // input data
  .i_valid(two_power_mod_in_valid),
  .i_ready(two_power_mod_in_ready),
  .i_modulus(modulus),
  .i_power(MOD_WIDTH * 2),

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
  .o_crypto(o_crypto)
);

endmodule
