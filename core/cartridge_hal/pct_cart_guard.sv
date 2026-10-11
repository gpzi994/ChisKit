`default_nettype none

module pct_cart_guard #(
    parameter integer SETTLE_CYCLES = 1024
) (
    input  logic        clk,
    input  logic        por_n,

    input  logic        host_b1_strobe,
    input  logic [31:0] host_b1_value,
    input  logic        host_reset_enter_strobe,
    input  logic        host_reset_exit_strobe,
    input  logic        abort,

    output logic [2:0]  state,
    output logic        play_selected,
    output logic        power_after_reset_exit,
    output logic [7:0]  adapter_id,
    output logic        read_ready,
    output logic        force_safe,
    output logic        writes_enabled
);

    localparam logic [2:0] ST_LOCKED          = 3'd0;
    localparam logic [2:0] ST_WAIT_RESET_EXIT = 3'd1;
    localparam logic [2:0] ST_SETTLING        = 3'd2;
    localparam logic [2:0] ST_READ_READY      = 3'd3;

    localparam integer SETTLE_COUNTER_WIDTH =
        (SETTLE_CYCLES < 2) ? 1 : $clog2(SETTLE_CYCLES);
    localparam integer SETTLE_LIMIT =
        (SETTLE_CYCLES < 1) ? 0 : SETTLE_CYCLES - 1;

    logic [SETTLE_COUNTER_WIDTH-1:0] settle_counter;

    assign read_ready    = (state == ST_READ_READY);
    assign force_safe    = !read_ready;

    // This slice has no cartridge write path. The constant is deliberately an
    // output so integration tests can prove that no higher layer enabled one.
    assign writes_enabled = 1'b0;

    always_ff @(posedge clk or negedge por_n) begin
        if (!por_n) begin
            state                  <= ST_LOCKED;
            play_selected          <= 1'b0;
            power_after_reset_exit <= 1'b0;
            adapter_id             <= 8'h00;
            settle_counter         <= '0;
        end else if (host_reset_enter_strobe || abort) begin
            state                  <= ST_LOCKED;
            play_selected          <= 1'b0;
            power_after_reset_exit <= 1'b0;
            adapter_id             <= 8'h00;
            settle_counter         <= '0;
        end else if (host_b1_strobe) begin
            play_selected          <= host_b1_value[24];
            power_after_reset_exit <= host_b1_value[16];
            adapter_id             <= host_b1_value[7:0];
            settle_counter         <= '0;

            if (host_b1_value[24] && host_b1_value[16]) begin
                state <= ST_WAIT_RESET_EXIT;
            end else begin
                state <= ST_LOCKED;
            end
        end else begin
            case (state)
                ST_LOCKED: begin
                    settle_counter <= '0;
                end

                ST_WAIT_RESET_EXIT: begin
                    settle_counter <= '0;
                    if (host_reset_exit_strobe) begin
                        if (SETTLE_CYCLES == 0) begin
                            state <= ST_READ_READY;
                        end else begin
                            state <= ST_SETTLING;
                        end
                    end
                end

                ST_SETTLING: begin
                    if (settle_counter == SETTLE_LIMIT) begin
                        state          <= ST_READ_READY;
                        settle_counter <= '0;
                    end else begin
                        settle_counter <= settle_counter + 1'b1;
                    end
                end

                ST_READ_READY: begin
                    settle_counter <= '0;
                end

                default: begin
                    state                  <= ST_LOCKED;
                    play_selected          <= 1'b0;
                    power_after_reset_exit <= 1'b0;
                    adapter_id             <= 8'h00;
                    settle_counter         <= '0;
                end
            endcase
        end
    end

endmodule

`default_nettype wire
