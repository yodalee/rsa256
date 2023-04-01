/*
 * Ready until all complete pin has been set
 */
module PipelineUntil
  # (parameter N = 1) (
  input clk,
  input rst,
  input i_valid,
  output logic i_ready,
  input complete [N]
);

logic waiting [N];

assign i_ready = !waiting.and();

always_ff @( posedge clk or negedge rst ) begin
  if (!rst) begin waiting <= '{N{'0}}; end
  else if (i_valid && i_ready) begin waiting <= '{N{1'b1}}; end
  else begin
    for (int i = 0; i < N; i++) begin
      if (complete[i]) begin
        waiting[i] <= 1'b0;
      end
    end
  end
end

endmodule