/*
 * Distribute one pair of valid/ready to N pair of valid/ready
 * Only those valid/ready with masked true get notified
 */
module PipelineDistributeMasked
  # (parameter N = 2) (
    input clk,
    input rst_n,
    input i_valid,
    output logic i_ready,
    output logic o_valid [N],
    input o_ready [N],
    input i_mask [N]
);

logic o_sent[N]; // whether output [i] is being sent
logic ready_sent[N];

// o_valid should be set:
// 1. when input is valid
// 2. keep high previous o_valid is not consumed
// 3. the output pin is not masked
always_comb begin
  for (int i = 0; i < N; i++) begin
    o_valid[i] = i_valid && !o_sent[i] && i_mask[i];
  end
end
// i_ready should be set:
// 1. all next modules are ready, or
// 2. all output are sent, or
// 3. being masked
always_comb begin
  foreach(ready_sent[i]) ready_sent[i] = o_ready[i] | o_sent[i] | i_mask[i];
end
always_comb begin
  i_ready = 1'b1;
  foreach(ready_sent[i]) begin
    i_ready = i_ready && ready_sent[i];
  end
end

always_ff @( posedge clk or negedge rst_n ) begin
  if (!rst_n) begin o_sent <= '{N{'0}}; end
  else if (i_valid && i_ready) begin // transaction
    o_sent <= '{N{'0}};
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