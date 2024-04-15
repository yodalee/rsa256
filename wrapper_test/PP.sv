module PPSlow(
	input clk, rst,
	input              ivalid,
	output logic       iready,
	input        [7:0] idata,
	output logic       ovalid,
	input              oready,
	output logic [7:0] odata
);

logic ivalid2;
assign ivalid2 = ivalid && (idata < 'd20);
assign iready = !ovalid;

always_ff @(posedge clk or negedge rst) begin
	if (!rst) ovalid <= 1'b1;
	else      ovalid <= ovalid ? (!oready) : ivalid2;
end

always_ff @(posedge clk or negedge rst) begin
	if (!rst)                   odata <= '0;
	else if (ivalid2 && iready) odata <= idata + 'd1;
end

endmodule

module PPFast(
	input clk, rst,
	input              ivalid,
	output logic       iready,
	input        [7:0] idata,
	output logic       ovalid,
	input              oready,
	output logic [7:0] odata
);

assign iready = !ovalid || oready;

always_ff @(posedge clk or negedge rst) begin
	if (!rst) ovalid <= 1'b0;
	else      ovalid <= ivalid || !iready;
end

always_ff @(posedge clk or negedge rst) begin
	if (!rst)                  odata <= '0;
	else if (ivalid && iready) odata <= idata + 'd1;
end

endmodule
