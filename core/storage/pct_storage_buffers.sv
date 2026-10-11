`default_nettype none

module pct_storage_buffers #(
    parameter logic [31:0] OPEN_PARAM_ADDR = 32'hF8004000,
    parameter logic [31:0] PAYLOAD_ADDR    = 32'hF8004200,
    parameter integer      PAYLOAD_BYTES   = 256
) (
    input  logic        bridge_endian_little,
    input  logic [31:0] bridge_addr,
    input  logic        bridge_rd,
    output logic        bridge_claimed,
    output logic [31:0] bridge_rd_data
);

    localparam integer FILE_PATH_LENGTH = 53;
    localparam logic [FILE_PATH_LENGTH*8-1:0] FILE_PATH =
        "/Saves/gba/common/ChisBurner/SelfTest/no-cart.bin";

    logic [31:0] bridge_rd_native;
    logic [31:0] byte_offset;

    function automatic logic [31:0] swap32(input logic [31:0] value);
        swap32 = {value[7:0], value[15:8], value[23:16], value[31:24]};
    endfunction

    function automatic logic [7:0] path_byte(input integer index);
        if (index >= 0 && index < FILE_PATH_LENGTH) begin
            path_byte = FILE_PATH[(FILE_PATH_LENGTH-index)*8-1 -: 8];
        end else begin
            path_byte = 8'h00;
        end
    endfunction

    function automatic logic [7:0] payload_byte(input integer index);
        if (index >= 0 && index < PAYLOAD_BYTES) begin
            payload_byte = 8'(index * 73 + 19);
        end else begin
            payload_byte = 8'h00;
        end
    endfunction

    function automatic logic [7:0] open_struct_byte(input integer index);
        if (index >= 0 && index < 256) begin
            open_struct_byte = path_byte(index);
        end else if (index == 256) begin
            open_struct_byte = 8'h03;
        end else if (index >= 257 && index <= 259) begin
            open_struct_byte = 8'h00;
        end else if (index == 260) begin
            open_struct_byte = PAYLOAD_BYTES[7:0];
        end else if (index == 261) begin
            open_struct_byte = PAYLOAD_BYTES[15:8];
        end else if (index == 262) begin
            open_struct_byte = PAYLOAD_BYTES[23:16];
        end else if (index == 263) begin
            open_struct_byte = PAYLOAD_BYTES[31:24];
        end else begin
            open_struct_byte = 8'h00;
        end
    endfunction

    always_comb begin
        bridge_claimed   = 1'b0;
        bridge_rd_native = 32'h00000000;
        byte_offset      = 32'h00000000;

        if (bridge_rd && bridge_addr >= OPEN_PARAM_ADDR &&
            bridge_addr < (OPEN_PARAM_ADDR + 32'd264)) begin
            bridge_claimed = 1'b1;
            byte_offset    = bridge_addr - OPEN_PARAM_ADDR;
            bridge_rd_native = {
                open_struct_byte(byte_offset + 3),
                open_struct_byte(byte_offset + 2),
                open_struct_byte(byte_offset + 1),
                open_struct_byte(byte_offset)
            };
        end else if (bridge_rd && bridge_addr >= PAYLOAD_ADDR &&
                     bridge_addr < (PAYLOAD_ADDR + PAYLOAD_BYTES)) begin
            bridge_claimed = 1'b1;
            byte_offset    = bridge_addr - PAYLOAD_ADDR;
            bridge_rd_native = {
                payload_byte(byte_offset + 3),
                payload_byte(byte_offset + 2),
                payload_byte(byte_offset + 1),
                payload_byte(byte_offset)
            };
        end

        bridge_rd_data = bridge_endian_little
                       ? swap32(bridge_rd_native)
                       : bridge_rd_native;
    end

endmodule

`default_nettype wire
