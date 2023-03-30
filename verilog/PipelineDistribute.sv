/*
 * Distribute one pair of valid/ready to N pair of valid/ready
 */
module PipelineDistribute
  # (parameter N = 2) (
    input clk,
    input rst,
    input i_valid,
    output logic i_ready,
    output logic [N-1:0] o_valid,
    input [N-1:0] o_ready
);

logic [N-1:0] o_sent; // whether output [i] is being sent

// o_valid should be set:
// 1. when input is valid
// 2. keep high previous o_valid is not consumed
always_comb begin
  for (int i = 0; i < N; i++) begin
    o_valid[i] = i_valid && !o_sent[i];
  end
end
// i_ready should be set:
// 1. all next modules are ready
// 2. no un-sent output
assign i_ready = &(o_ready | o_sent);

always_ff @( posedge clk or negedge rst ) begin
  if (!rst) begin o_sent <= 0; end
  else if (i_valid && i_ready) begin // transaction
    o_sent <= 0;
  end
  else begin
    for (int i = 0; i < N; i++) begin
      if (o_valid[i] && o_ready[i]) begin
        o_sent[i] <= 1'b1;
      end
    end
  end
end

endmodule