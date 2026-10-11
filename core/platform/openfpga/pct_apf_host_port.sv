`default_nettype none

module pct_apf_host_port (
    input  logic        clk,
    input  logic        por_n,
    input  logic        bridge_endian_little,
    input  logic [31:0] bridge_addr,
    input  logic        bridge_rd,
    input  logic        bridge_wr,
    input  logic [31:0] bridge_wr_data,
    output logic        bridge_claimed,
    output logic [31:0] bridge_rd_data,

    input  logic        status_boot_done,
    input  logic        status_setup_done,

    output logic        core_reset_n,
    output logic        host_b1_strobe,
    output logic [31:0] host_b1_value,
    output logic        host_reset_enter_strobe,
    output logic        host_reset_exit_strobe
);

    localparam logic ST_IDLE = 1'b0;
    localparam logic ST_EXEC = 1'b1;

    logic        state;
    logic [15:0] pending_command;
    logic [31:0] host_command_status;
    logic [31:0] host_param0;
    logic [31:0] host_param1;
    logic [31:0] host_param2;
    logic [31:0] host_param3;

    // APF reserves F8xx00xx for Host commands. The middle byte is not fixed.
    wire host_selected = (bridge_addr[31:24] == 8'hF8) &&
                         (bridge_addr[15:8]  == 8'h00);

    function automatic logic [31:0] swap32(input logic [31:0] value);
        swap32 = {value[7:0], value[15:8], value[23:16], value[31:24]};
    endfunction

    wire [31:0] bridge_wr_native = bridge_endian_little
                                  ? swap32(bridge_wr_data)
                                  : bridge_wr_data;

    logic [31:0] bridge_rd_native;

    always_comb begin
        bridge_claimed = bridge_rd && host_selected;
        case (bridge_addr[7:0])
            8'h00: bridge_rd_native = host_command_status;
            8'h04: bridge_rd_native = 32'h00000020;
            8'h08: bridge_rd_native = 32'h00000040;
            8'h20: bridge_rd_native = host_param0;
            8'h24: bridge_rd_native = host_param1;
            8'h28: bridge_rd_native = host_param2;
            8'h2C: bridge_rd_native = host_param3;
            default: bridge_rd_native = 32'h00000000;
        endcase
        bridge_rd_data = bridge_endian_little
                       ? swap32(bridge_rd_native)
                       : bridge_rd_native;
    end

    always_ff @(posedge clk or negedge por_n) begin
        if (!por_n) begin
            state                    <= ST_IDLE;
            pending_command          <= 16'h0000;
            host_command_status      <= 32'h00000000;
            host_param0              <= 32'h00000000;
            host_param1              <= 32'h00000000;
            host_param2              <= 32'h00000000;
            host_param3              <= 32'h00000000;
            core_reset_n             <= 1'b0;
            host_b1_strobe           <= 1'b0;
            host_b1_value            <= 32'h00000000;
            host_reset_enter_strobe  <= 1'b0;
            host_reset_exit_strobe   <= 1'b0;
        end else begin
            host_b1_strobe          <= 1'b0;
            host_reset_enter_strobe <= 1'b0;
            host_reset_exit_strobe  <= 1'b0;

            if (bridge_wr && host_selected) begin
                case (bridge_addr[7:0])
                    8'h00: begin
                        if (bridge_wr_native[31:16] == 16'h434D && state == ST_IDLE) begin
                            pending_command     <= bridge_wr_native[15:0];
                            host_command_status <= {16'h4255, bridge_wr_native[15:0]};
                            state               <= ST_EXEC;
                        end
                    end
                    8'h20: host_param0 <= bridge_wr_native;
                    8'h24: host_param1 <= bridge_wr_native;
                    8'h28: host_param2 <= bridge_wr_native;
                    8'h2C: host_param3 <= bridge_wr_native;
                    default: begin end
                endcase
            end

            if (state == ST_EXEC) begin
                case (pending_command)
                    16'h0000: begin
                        if (!status_boot_done) begin
                            host_command_status <= 32'h4F4B0001;
                        end else if (!status_setup_done) begin
                            host_command_status <= 32'h4F4B0002;
                        end else if (!core_reset_n) begin
                            host_command_status <= 32'h4F4B0003;
                        end else begin
                            host_command_status <= 32'h4F4B0004;
                        end
                    end

                    16'h0010: begin
                        core_reset_n            <= 1'b0;
                        host_reset_enter_strobe <= 1'b1;
                        host_command_status     <= 32'h4F4B0000;
                    end

                    16'h0011: begin
                        core_reset_n           <= 1'b1;
                        host_reset_exit_strobe <= 1'b1;
                        host_command_status    <= 32'h4F4B0000;
                    end

                    16'h00B0: begin
                        host_command_status <= 32'h4F4B0000;
                    end

                    16'h00B1: begin
                        host_b1_value        <= host_param0;
                        host_b1_strobe       <= 1'b1;
                        host_command_status  <= 32'h4F4B0000;
                    end

                    default: begin
                        host_command_status <= 32'h4F4BFFFF;
                    end
                endcase
                state <= ST_IDLE;
            end
        end
    end

endmodule

`default_nettype wire
