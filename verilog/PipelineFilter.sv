module PipelineFilter (
    input i_valid,
    output logic i_ready,
    input i_pass,
    output logic o_valid,
    input o_ready
);

assign o_valid = i_pass && i_valid;
// consume input when the input is not valid to pass to output
assign i_ready = !i_pass || o_ready;

endmodule