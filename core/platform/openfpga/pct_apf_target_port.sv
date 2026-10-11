`default_nettype none

module pct_apf_target_port (
    input  logic        clk,
    input  logic        por_n,
    input  logic        bridge_endian_little,
    input  logic [31:0] bridge_addr,
    input  logic        bridge_rd,
    input  logic        bridge_wr,
    input  logic [31:0] bridge_wr_data,
    output logic        bridge_claimed,
    output logic [31:0] bridge_rd_data,

    input  logic        request_valid,
    input  logic [15:0] request_code,
    input  logic [31:0] request_param0,
    input  logic [31:0] request_param1,
    input  logic [31:0] request_param2,
    input  logic [31:0] request_param3,
    input  logic        request_abort,
    output logic        request_accept,
    output logic        request_done,
    output logic [15:0] request_result
);

    localparam logic ST_IDLE = 1'b0;
    localparam logic ST_WAIT = 1'b1;

    logic        state;
    logic [31:0] target_command_status;
    logic [31:0] target_param0;
    logic [31:0] target_param1;
    logic [31:0] target_param2;
    logic [31:0] target_param3;

    // APF reserves F8xx10xx for Target commands. The middle byte is not fixed.
    wire target_selected = (bridge_addr[31:24] == 8'hF8) &&
                           (bridge_addr[15:8]  == 8'h10);

    function automatic logic [31:0] swap32(input logic [31:0] value);
        swap32 = {value[7:0], value[15:8], value[23:16], value[31:24]};
    endfunction

    wire [31:0] bridge_wr_native = bridge_endian_little
                                  ? swap32(bridge_wr_data)
                                  : bridge_wr_data;

    logic [31:0] bridge_rd_native;

    always_comb begin
        bridge_claimed = bridge_rd && target_selected;
        case (bridge_addr[7:0])
            8'h00: bridge_rd_native = target_command_status;
            8'h04: bridge_rd_native = 32'h00000020;
            8'h08: bridge_rd_native = 32'h00000040;
            8'h20: bridge_rd_native = target_param0;
            8'h24: bridge_rd_native = target_param1;
            8'h28: bridge_rd_native = target_param2;
            8'h2C: bridge_rd_native = target_param3;
            default: bridge_rd_native = 32'h00000000;
        endcase
        bridge_rd_data = bridge_endian_little
                       ? swap32(bridge_rd_native)
                       : bridge_rd_native;
    end

    always_ff @(posedge clk or negedge por_n) begin
        if (!por_n) begin
            state                 <= ST_IDLE;
            target_command_status <= 32'h00000000;
            target_param0         <= 32'h00000000;
            target_param1         <= 32'h00000000;
            target_param2         <= 32'h00000000;
            target_param3         <= 32'h00000000;
            request_accept        <= 1'b0;
            request_done          <= 1'b0;
            request_result        <= 16'h0000;
        end else begin
            request_accept <= 1'b0;
            request_done   <= 1'b0;

            if (request_abort) begin
                state                 <= ST_IDLE;
                target_command_status <= 32'h00000000;
            end else if (bridge_wr && target_selected && bridge_addr[7:0] == 8'h00) begin
                target_command_status <= bridge_wr_native;
                if (state == ST_WAIT && bridge_wr_native[31:16] == 16'h6F6B) begin
                    request_result <= bridge_wr_native[15:0];
                    request_done   <= 1'b1;
                    state          <= ST_IDLE;
                end
            end

            if (!request_abort && state == ST_IDLE && request_valid) begin
                target_param0         <= request_param0;
                target_param1         <= request_param1;
                target_param2         <= request_param2;
                target_param3         <= request_param3;
                target_command_status <= {16'h636D, request_code};
                request_accept        <= 1'b1;
                state                 <= ST_WAIT;
            end
        end
    end

endmodule

`default_nettype wire
