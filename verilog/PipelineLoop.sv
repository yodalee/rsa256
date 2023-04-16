module PipelineLoop (
    input clk,
    input rst_n,
    input i_valid,
    output logic i_ready,
    output logic i_cen,
    output logic o_valid,
    input o_ready,
    input o_done,
    output logic o_cen
);

logic o_valid_w;

always_comb begin
    i_ready = !o_valid || o_ready && o_done;
    i_cen = i_ready && i_valid;
    o_cen = o_ready && !o_done && o_valid; // next
end
assign o_valid_w = i_valid || o_valid && !(o_ready && o_done);

always_ff @(posedge clk or negedge rst_n) begin
    if(!rst_n) begin o_valid <= 1'b0; end
    else begin o_valid <= o_valid_w; end
end

endmodule