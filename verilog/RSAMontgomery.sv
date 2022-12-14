module RSAMontgomery #(
	parameter MOD_WIDTH = 256
) (
	// input
	input clk,
	input rst,

	// input data
	input i_valid,
	output i_ready,
	input [MOD_WIDTH-1:0] i_a,
	input [MOD_WIDTH-1:0] i_b,
	input [MOD_WIDTH-1:0] i_modulus,

	// output data
	output o_valid,
	input o_ready,
	output [MOD_WIDTH-1:0] o_out
);

logic [MOD_WIDTH + 2 - 1:0] data_a;
logic [MOD_WIDTH + 2 - 1:0] data_b;
logic [MOD_WIDTH + 2 - 1:0] data_modulus;
logic [MOD_WIDTH + 2 - 1:0] round_result;
logic [MOD_WIDTH + 2 - 1:0] round_result_next;

logic [$clog2(MOD_WIDTH+1)-1:0] round_counter;

typedef enum logic [1:0] {
  STATE_IDLE  = 0,
  STATE_CALCULATE = 1,
  STATE_WAITDONE = 2
} State_t;

State_t state, state_next;

assign i_ready = state == STATE_IDLE;
assign o_valid = state == STATE_WAITDONE;
assign o_out = round_result[MOD_WIDTH-1:0];

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
    if (round_counter == MOD_WIDTH-1) begin
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
		data_a <= 0;
		data_b <= 0;
		data_modulus <= 0;
  end
  else begin
		if (i_ready && i_valid) begin
			data_a <= {2'b0, i_a};
			data_b <= {2'b0, i_b};
			data_modulus <= {2'b0, i_modulus};
		end else begin
			data_a <= data_a;
			data_b <= data_b;
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

// round_result
always_ff @(posedge clk or negedge rst) begin
  if (!rst) begin
    round_result <= 0;
  end
  else begin
		round_result <= round_result_next;
	end
end

always_comb begin
  case (state)
  STATE_IDLE: begin
		round_result_next = 0;
  end
  STATE_CALCULATE: begin
		if (data_a[round_counter]) begin
			round_result_next = round_result + data_b;
		end
		if (round_result_next[0]) begin
			round_result_next += data_modulus;
		end
		round_result_next >>= 1;
	end
	STATE_WAITDONE: begin
		round_result_next = round_result;
	end
	default: begin
		round_result_next = 0;
	end
  endcase
end

endmodule
