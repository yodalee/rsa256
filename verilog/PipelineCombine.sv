/*
 * Combine multiple pair of valid/ready to 1 pair of valid/ready
 */
module PipelineCombine
  # (parameter N = 2) (
    input i_valid [N],
    output logic i_ready [N],
    output logic o_valid,
    input o_ready
);


// o_valid should be set:
// 1. when all inputs are valid
assign o_valid = i_valid.and();

// i_ready reflect o_ready in-time
always_comb begin
  foreach(i_ready[i]) begin
    i_ready[i] = o_ready & o_valid;
  end
end

endmodule