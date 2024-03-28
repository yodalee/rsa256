/*
If block = 0 ,then the module is just pipeline.
If block = 1, then the module will stop thread.
*/

module PipelineBlock(
    input clk,
    input rst_n,
    input i_valid,
    output i_ready,
    output logic o_valid,
    input o_ready,
    input block
);

logic o_valid_w;

assign i_ready = !block && (!o_valid || o_ready);
assign o_valid_w = !block && i_valid || o_valid && !o_ready;

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin o_valid <= 1'b0; end
    else begin o_valid <= o_valid_w; end
end

endmodule