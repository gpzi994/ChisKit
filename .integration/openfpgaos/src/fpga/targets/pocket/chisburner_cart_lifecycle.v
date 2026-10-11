//------------------------------------------------------------------------------
// ChisBurner cartridge lifecycle guard.
//
// Host notifications are already synchronized into clk before reaching this
// block.  A read can be authorized only after Play Cartridge selection, the
// host's post-reset power announcement, reset exit, and a bounded settle wait.
// ABORT or reset entry always returns to the locked state.
//------------------------------------------------------------------------------

`default_nettype none

module chisburner_cart_lifecycle #(
    parameter integer SETTLE_CYCLES = 9000000
) (
    input  wire        clk,
    input  wire        por_n,
    input  wire        host_b1_strobe,
    input  wire [31:0] host_b1_value,
    input  wire        host_b1_valid,
    input  wire        host_reset_enter_strobe,
    input  wire        host_reset_exit_strobe,
    input  wire        host_reset_exited,
    input  wire        abort,
    output reg  [2:0]  state,
    output reg         play_selected,
    output reg         power_after_reset_exit,
    output reg  [7:0]  adapter_id,
    output reg         read_ready,
    output wire [31:0] status_word
);

localparam [2:0] ST_LOCKED          = 3'd0;
localparam [2:0] ST_WAIT_RESET_EXIT = 3'd1;
localparam [2:0] ST_SETTLING        = 3'd2;
localparam [2:0] ST_READ_READY      = 3'd3;

localparam integer SETTLE_COUNTER_WIDTH =
    (SETTLE_CYCLES < 2) ? 1 : $clog2(SETTLE_CYCLES);
localparam integer SETTLE_LIMIT =
    (SETTLE_CYCLES < 1) ? 0 : SETTLE_CYCLES - 1;

reg [SETTLE_COUNTER_WIDTH-1:0] settle_counter;
reg b1_captured;

assign status_word = {read_ready, 7'd0, adapter_id, 6'd0,
                      power_after_reset_exit, play_selected, 5'd0, state};

always @(posedge clk or negedge por_n) begin
    if (!por_n) begin
        state                  <= ST_LOCKED;
        play_selected          <= 1'b0;
        power_after_reset_exit <= 1'b0;
        adapter_id             <= 8'd0;
        settle_counter         <= {SETTLE_COUNTER_WIDTH{1'b0}};
        b1_captured             <= 1'b0;
        read_ready              <= 1'b0;
    end else if (abort) begin
        state                  <= ST_LOCKED;
        play_selected          <= 1'b0;
        power_after_reset_exit <= 1'b0;
        adapter_id             <= 8'd0;
        settle_counter         <= {SETTLE_COUNTER_WIDTH{1'b0}};
        // Do not automatically reuse the still-latched B1 level after ABORT.
        b1_captured             <= host_b1_valid;
        read_ready              <= 1'b0;
    end else if (host_reset_enter_strobe) begin
        state                  <= ST_LOCKED;
        play_selected          <= 1'b0;
        power_after_reset_exit <= 1'b0;
        adapter_id             <= 8'd0;
        settle_counter         <= {SETTLE_COUNTER_WIDTH{1'b0}};
        b1_captured             <= 1'b1;
        read_ready              <= 1'b0;
    end else if (!host_b1_valid) begin
        state                  <= ST_LOCKED;
        play_selected          <= 1'b0;
        power_after_reset_exit <= 1'b0;
        adapter_id             <= 8'd0;
        settle_counter         <= {SETTLE_COUNTER_WIDTH{1'b0}};
        b1_captured             <= 1'b0;
        read_ready              <= 1'b0;
    end else if (host_b1_strobe || !b1_captured) begin
        play_selected          <= host_b1_value[24];
        power_after_reset_exit <= host_b1_value[16];
        adapter_id             <= host_b1_value[7:0];
        settle_counter         <= {SETTLE_COUNTER_WIDTH{1'b0}};
        b1_captured             <= 1'b1;
        read_ready              <= 1'b0;
        if (host_b1_value[24] && host_b1_value[16]) begin
            if (host_reset_exited) begin
                if (SETTLE_CYCLES == 0) begin
                    state      <= ST_READ_READY;
                    read_ready <= 1'b1;
                end
                else
                    state <= ST_SETTLING;
            end else begin
                state <= ST_WAIT_RESET_EXIT;
            end
        end else begin
            state <= ST_LOCKED;
        end
    end else begin
        case (state)
            ST_LOCKED: begin
                settle_counter <= {SETTLE_COUNTER_WIDTH{1'b0}};
                read_ready      <= 1'b0;
            end
            ST_WAIT_RESET_EXIT: begin
                settle_counter <= {SETTLE_COUNTER_WIDTH{1'b0}};
                if (host_reset_exit_strobe || host_reset_exited) begin
                    if (SETTLE_CYCLES == 0) begin
                        state      <= ST_READ_READY;
                        read_ready <= 1'b1;
                    end
                    else
                        state <= ST_SETTLING;
                end
            end
            ST_SETTLING: begin
                if (!host_reset_exited) begin
                    state          <= ST_WAIT_RESET_EXIT;
                    settle_counter <= {SETTLE_COUNTER_WIDTH{1'b0}};
                    read_ready     <= 1'b0;
                end else if (settle_counter == SETTLE_LIMIT) begin
                    state          <= ST_READ_READY;
                    settle_counter <= {SETTLE_COUNTER_WIDTH{1'b0}};
                    read_ready     <= 1'b1;
                end else begin
                    settle_counter <= settle_counter + 1'b1;
                    read_ready     <= 1'b0;
                end
            end
            ST_READ_READY: begin
                if (!host_reset_exited) begin
                    state <= ST_WAIT_RESET_EXIT;
                    read_ready <= 1'b0;
                end else begin
                    read_ready <= 1'b1;
                end
                settle_counter <= {SETTLE_COUNTER_WIDTH{1'b0}};
            end
            default: begin
                state                  <= ST_LOCKED;
                play_selected          <= 1'b0;
                power_after_reset_exit <= 1'b0;
                adapter_id             <= 8'd0;
                settle_counter         <= {SETTLE_COUNTER_WIDTH{1'b0}};
                read_ready              <= 1'b0;
            end
        endcase
    end
end

endmodule

`default_nettype wire
