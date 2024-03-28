/*
After each output data is send out from the module, 
the module must stop thread, until o_done = 1
*/

module PipelineMutex (
    input clk,
    input rst_n,
    input i_valid,
    output logic i_ready,
    output logic i_cen,
    output logic o_valid,
    input o_ready,
    input o_done
);

    logic block;
    
assign i_cen = i_valid && i_ready;

always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin block <= 1'b0; end
    else if (o_done) begin block <= 1'b0; end
    else if (i_cen) begin block <= 1'b1; end 
end

PipelineBlock i_block (
    .clk(clk),
    .rst_n(rst_n),
    .i_valid(i_valid),
    .i_ready(i_ready),
    .o_valid(o_valid),
    .o_ready(o_ready),
    .block(block)
);

endmodule