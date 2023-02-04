
import RSA_pkg::*;

module RSAMont (
  // input
  input clk,
  input rst,

  // input data
  input i_valid,
  output i_ready,
  input KeyType i_base,    // 2 ^ 2n mod N
  input KeyType i_msg,     // msg
  input KeyType i_key,     // key e
  input KeyType i_modulus, // N

  // output data
  output o_valid,
  input o_ready,
  output KeyType o_crypto
);

typedef enum logic [1:0] {
  STATE_IDLE = 0,
  STATE_LOOP = 1,
  STATE_WAITDONE = 2
} State_t;
State_t state, state_next;

KeyType base, msg, key, modulus; // input
KeyType square, multiply;  // calculate result
logic [$clog2(2 * MOD_WIDTH+1)-1:0] round_counter;
logic [$clog2(2 * MOD_WIDTH+1)-1:0] key_idx;

assign i_ready = state == STATE_IDLE;
assign o_valid = state == STATE_WAITDONE;
assign key_idx = (round_counter - 2) >> 1;

// read input data
always_latch @( posedge clk or negedge rst ) begin
  if (!rst) begin
    base <= 0;
    msg <= 0;
    key <= 0;
    modulus <= 0;
  end
  else begin
    if (i_ready && i_valid) begin
      base <= i_base;
      msg <= i_msg;
      key <= i_key;
      modulus <= i_modulus;
    end
  end
end

// update state logic
always_ff @(posedge clk or negedge rst) begin
  if (!rst) begin
    state <= STATE_IDLE;
  end
  else begin
    state <= state_next;
  end
end

// next logic for state
always_comb begin
  case (state)
  STATE_IDLE: begin
    if (i_valid && i_ready) begin
      state_next = STATE_LOOP;
    end
  end
  STATE_LOOP: begin
    if (loop_done) begin
      state_next = STATE_WAITDONE;
    end
  end
  STATE_WAITDONE: begin
    if (o_ready) begin
      state_next = STATE_IDLE;
    end
  end
  default: begin
    state_next = STATE_IDLE;
  end
  endcase
end

// round_counter
always_latch @(posedge clk or negedge rst) begin
  if (loop_init) begin
    round_counter <= 0;
  end
  else if (loop_next) begin
    round_counter <= round_counter + 1;
  end
end

// data
always_ff @(posedge clk) begin
  if (montgomery_out_ready && montgomery_out_valid) begin
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
  else begin
    square <= square;
    multiply <= multiply;
  end
end

always_comb begin
  if (state == STATE_LOOP) begin
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
  else begin
    montgomery_in_a = 0;
    montgomery_in_b = 0;
  end
end

logic montgomery_out_valid;
logic montgomery_out_ready;
KeyType montgomery_out;
KeyType montgomery_in_a, montgomery_in_b;

logic loop_ivalid, loop_iready;
logic loop_ovalid, loop_oready;
logic loop_init, loop_next, loop_done;
assign loop_ivalid = state == STATE_LOOP;
// check stop condition:
// 1 round for packed message
// 256 * 2 round for square and multiply
assign loop_done = state == STATE_LOOP && round_counter == MOD_WIDTH * 2 + 2;
assign montgomery_out_ready = state == STATE_LOOP;
assign o_crypto = multiply;

PipelineLoop i_loop(
  .clk(clk),
  .rst(rst),
  .i_valid(loop_ivalid),
  .i_ready(loop_iready),
  .i_cen(loop_init),
  .o_valid(loop_ovalid),
  .o_ready(loop_oready),
  .o_done(loop_done),
  .o_cen(loop_next)
);

RSAMontgomery i_montgomery(
  // input
  .clk(clk),
  .rst(rst),

  // input data
  .i_valid(loop_ovalid),
  .i_ready(loop_oready),
  .i_a(montgomery_in_a),
  .i_b(montgomery_in_b),
  .i_modulus(modulus),

  // output data
  .o_valid(montgomery_out_valid),
  .o_ready(montgomery_out_ready),
  .o_out(montgomery_out)
);

endmodule