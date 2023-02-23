
import RSA_pkg::*;

module RSATwoPowerMod (
	// input
	input clk,
	input rst,

	// input data
	input i_valid,
	output i_ready,
	input KeyType i_modulus,
	input IntType i_power,

	// output data
	output o_valid,
	input o_ready,
	output KeyType o_out
);

typedef logic [MOD_WIDTH:0] ExtendKeyType;

typedef enum logic [1:0] {
  STATE_IDLE  = 0,
  STATE_CALCULATE = 1,
  STATE_WAITDONE = 2
} State_t;

ExtendKeyType data_modulus;
ExtendKeyType data_round_result;
IntType data_power;
IntType round_counter;
State_t state, state_next;

assign i_ready = state == STATE_IDLE;
assign o_valid = state == STATE_WAITDONE;
assign o_out = data_round_result[MOD_WIDTH-1:0];

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
    if (i_valid) begin
      state_next = STATE_CALCULATE;
    end
  end
  STATE_CALCULATE: begin
    if (round_counter == data_power - 1) begin
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

// read input data
always_ff @( posedge clk or negedge rst ) begin
  if (!rst) begin
	data_power <= 0;
	data_modulus <= 0;
  end
  else begin
	if (i_ready && i_valid) begin
		data_power <= i_power;
		data_modulus <= {1'b0, i_modulus};
	end else begin
		data_power <= data_power;
		data_modulus <= data_modulus;
	end
  end
end

// round_counter
always_ff @(posedge clk or negedge rst) begin
  if (!rst) begin
    round_counter <= 0;
  end
  else begin
	case (state)
		STATE_CALCULATE:
			round_counter <= round_counter + 1;
		default:
		round_counter <= 0;
	endcase
  end
end

// data_round_result
always_ff @(posedge clk or negedge rst) begin
  if (!rst) begin
    data_round_result <= 1;
  end
  else begin
	case (state)
		STATE_IDLE:
		data_round_result <= 1;
		STATE_CALCULATE:
			data_round_result <= ((data_round_result << 1) > data_modulus) ?
			(data_round_result << 1) - data_modulus :
			data_round_result << 1;
		STATE_WAITDONE:
			data_round_result <= data_round_result;
		default: begin
			data_round_result <= 1;
		end
	endcase
  end
end

endmodule
