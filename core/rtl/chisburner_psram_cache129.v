`default_nettype none

// Read-only verifier for the two 16 MiB Pocket PSRAM chips.  A start pulse
// scans PSRAM0 followed by PSRAM1, one 32-bit word at a time, and computes a
// standard reflected CRC-32 over bytes in file order.  The controllers remain
// outside this module so the same physical paths are used by loading and by
// later NOR programming.
module chisburner_psram_cache129 (
    input  wire        clk,
    input  wire        reset_n,
    input  wire        start,
    input  wire        read_start,
    input  wire [22:0] read_addr,

    output reg         p0_rd,
    output reg [21:0]  p0_addr,
    input  wire [31:0] p0_data,
    input  wire        p0_valid,

    output reg         p1_rd,
    output reg [21:0]  p1_addr,
    input  wire [31:0] p1_data,
    input  wire        p1_valid,

    output reg         active,
    output reg         done,
    output reg         fault,
    output reg         half,
    output reg [31:0]  crc0,
    output reg [31:0]  crc1,
    output reg         read_busy,
    output reg         read_done_toggle,
    output reg         read_fault,
    output reg [31:0]  read_data,
    output wire [21:0] progress_word
);

localparam [1:0] S_IDLE=2'd0, S_ISSUE=2'd1, S_WAIT=2'd2;
localparam [21:0] LAST_WORD=22'h3fffff;
reg [1:0] state;
reg [31:0] crc;
reg [27:0] watchdog;
reg single_mode;
assign progress_word = half ? p1_addr : p0_addr;

function [31:0] crc_byte;
    input [31:0] c;
    input [7:0] b;
    integer i;
    reg [31:0] x;
    begin
        x = c ^ b;
        for (i=0;i<8;i=i+1)
            x = x[0] ? ((x >> 1) ^ 32'hedb88320) : (x >> 1);
        crc_byte = x;
    end
endfunction

wire [31:0] selected_data = half ? p1_data : p0_data;
wire selected_valid = half ? p1_valid : p0_valid;
wire [31:0] crc_b0 = crc_byte(crc, selected_data[7:0]);
wire [31:0] crc_b1 = crc_byte(crc_b0, selected_data[15:8]);
wire [31:0] crc_b2 = crc_byte(crc_b1, selected_data[23:16]);
wire [31:0] crc_next = crc_byte(crc_b2, selected_data[31:24]);

always @(posedge clk or negedge reset_n) begin
    if (!reset_n) begin
        state<=S_IDLE; p0_rd<=1'b0; p1_rd<=1'b0;
        p0_addr<=22'd0; p1_addr<=22'd0;
        active<=1'b0; done<=1'b0; fault<=1'b0; half<=1'b0;
        crc<=32'hffffffff; crc0<=32'd0; crc1<=32'd0;
        read_busy<=1'b0; read_done_toggle<=1'b0;
        read_fault<=1'b0; read_data<=32'd0; single_mode<=1'b0;
        watchdog<=28'd0;
    end else begin
        p0_rd<=1'b0;
        p1_rd<=1'b0;
        if (start && !active) begin
            state<=S_ISSUE; p0_addr<=22'd0; p1_addr<=22'd0;
            active<=1'b1; done<=1'b0; fault<=1'b0; half<=1'b0;
            crc<=32'hffffffff; crc0<=32'd0; crc1<=32'd0;
            watchdog<=28'd0;
            single_mode<=1'b0;
        end else if (read_start && !active && !read_busy) begin
            state<=S_ISSUE; p0_addr<=read_addr[21:0]; p1_addr<=read_addr[21:0];
            half<=read_addr[22]; read_busy<=1'b1; read_fault<=1'b0;
            single_mode<=1'b1; watchdog<=28'd0;
        end else if (active || read_busy) begin
            case (state)
                S_ISSUE: begin
                    if (half) p1_rd<=1'b1; else p0_rd<=1'b1;
                    watchdog<=28'd0;
                    state<=S_WAIT;
                end
                S_WAIT: begin
                    watchdog<=watchdog+28'd1;
                    if (selected_valid) begin
                        watchdog<=28'd0;
                        if (single_mode) begin
                            read_data<=selected_data;
                            read_busy<=1'b0;
                            read_done_toggle<=~read_done_toggle;
                            single_mode<=1'b0;
                            state<=S_IDLE;
                        end else if ((!half && p0_addr==LAST_WORD) ||
                            ( half && p1_addr==LAST_WORD)) begin
                            if (!half) begin
                                crc0<=~crc_next;
                                crc<=32'hffffffff;
                                half<=1'b1;
                                p1_addr<=22'd0;
                                state<=S_ISSUE;
                            end else begin
                                crc1<=~crc_next;
                                active<=1'b0;
                                done<=1'b1;
                                state<=S_IDLE;
                            end
                        end else begin
                            crc<=crc_next;
                            if (half) p1_addr<=p1_addr+22'd1;
                            else      p0_addr<=p0_addr+22'd1;
                            state<=S_ISSUE;
                        end
                    end else if (&watchdog) begin
                        if(single_mode) begin
                            read_busy<=1'b0; read_fault<=1'b1;
                            read_done_toggle<=~read_done_toggle; single_mode<=1'b0;
                        end else begin
                            active<=1'b0; done<=1'b1; fault<=1'b1;
                        end
                        state<=S_IDLE;
                    end
                end
                default: state<=S_IDLE;
            endcase
        end
    end
end

endmodule

`default_nettype wire
