module Pipeline (
    input clk,
    input rst_n,
    input i_valid,
    output logic i_ready,
    output logic o_en,
    output logic o_valid,
    input o_ready
);

logic o_valid_w;

// There is valid data if:
// 1. this cycle there is data to be stored
// 2. there is old data in register and not consumed yet
assign o_valid_w = i_valid || (o_valid && !o_ready);
// The module is ready to receive data if:
// 1. No data need to write out, or
// 2. The next module is ready to receive data
assign i_ready = !o_valid || o_ready;
assign o_en = i_ready && i_valid;

always_ff @( posedge clk or negedge rst_n ) begin
  if (!rst_n) begin o_valid <= 1'b0; end
  else begin o_valid <= o_valid_w; end
end


endmodule