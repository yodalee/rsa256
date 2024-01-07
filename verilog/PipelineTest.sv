module PipelineTest (
  input clk,
  input rst_n,

  // input
  input i_valid,
  input [31:0] i_data,
  output logic i_ready,

  // output
  output logic o_valid,
  output [31:0] o_data,
  input o_ready
);

logic en;
logic [31:0] data;

always_ff @(posedge clk or negedge rst_n) begin
  if (!rst_n) begin
    data <= 'b0;
  end
  else if (en) begin
    data <= i_data;
  end
end

assign o_data = data;

Pipeline i_pipe(
    .clk(clk),
    .rst_n(rst_n),
    .i_valid(i_valid),
    .i_ready(i_ready),
    .o_en(en),
    .o_valid(o_valid),
    .o_ready(o_ready)
);

endmodule