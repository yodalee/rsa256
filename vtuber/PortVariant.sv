typedef struct packed {
  logic [19:0] a20;
  logic [19:0] b20;
} Struct;

module PortVariant (
  input bit_in,
  output bit_out,
  input [7:0] byte_in,
  output [7:0] byte_out,
  input [15:0] short_in,
  output [15:0] short_out,
  input [31:0] int_in,
  output [31:0] int_out,
  input [63:0] long_in,
  output [63:0] long_out,
  input [1023:0] verylong_in,
  output [1023:0] verylong_out,

  input Struct struct_in,
  output Struct struct_out
);

assign bit_out = bit_in;
assign byte_out = byte_in;
assign short_out = short_in;
assign int_out = int_in;
assign long_out = long_in;
assign verylong_out = verylong_in;
assign struct_out = struct_in;

endmodule