/*
 * Combine multiple pair of valid/ready to 1 pair of valid/ready
 * Only care those inputs that masked are true
 */
module PipelineCombineMasked
  # (parameter N = 2) (
    input i_valid [N],
    output logic i_ready [N],
    output logic o_valid,
    input o_ready,
    input i_mask[N]
);

// o_valid should be set:
// 1. when all inputs are either mask = 0, or mask =1 and valid
always_comb begin
  o_valid = 1'b1;
  for (int i = 0; i < N; i++) begin
    o_valid = o_valid & (i_valid[i] || !i_mask[i]);
  end
end

// i_ready reflect o_ready in-time
// Don't care (mask = 0) will not get notified
always_comb begin
  foreach(i_ready[i]) begin
    i_ready[i] = o_ready & o_valid & i_mask[i];
  end
end

endmodule