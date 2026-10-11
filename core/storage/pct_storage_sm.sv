`default_nettype none

module pct_storage_sm #(
    parameter logic [15:0] SLOT_ID               = 16'h0100,
    parameter logic [31:0] OPEN_PARAM_ADDR       = 32'hF8004000,
    parameter logic [31:0] PAYLOAD_ADDR          = 32'hF8004200,
    parameter integer      PAYLOAD_BYTES         = 256,
    parameter integer      CHUNK_BYTES           = 64,
    parameter integer      COMMAND_TIMEOUT_CYCLES = 1024
) (
    input  logic        clk,
    input  logic        por_n,
    input  logic        start,
    input  logic        cancel,

    output logic        request_valid,
    output logic [15:0] request_code,
    output logic [31:0] request_param0,
    output logic [31:0] request_param1,
    output logic [31:0] request_param2,
    output logic [31:0] request_param3,
    output logic        request_abort,
    input  logic        request_accept,
    input  logic        request_done,
    input  logic [15:0] request_result,

    output logic        busy,
    output logic        done,
    output logic        success,
    output logic [15:0] error_code,
    output logic [31:0] bytes_committed,
    output logic [3:0]  state
);

    localparam logic [3:0] ST_IDLE       = 4'd0;
    localparam logic [3:0] ST_OPEN_REQ   = 4'd1;
    localparam logic [3:0] ST_OPEN_WAIT  = 4'd2;
    localparam logic [3:0] ST_WRITE_REQ  = 4'd3;
    localparam logic [3:0] ST_WRITE_WAIT = 4'd4;
    localparam logic [3:0] ST_FLUSH_REQ  = 4'd5;
    localparam logic [3:0] ST_FLUSH_WAIT = 4'd6;

    localparam logic [15:0] CMD_DATASLOT_WRITE = 16'h0184;
    localparam logic [15:0] CMD_DATASLOT_FLUSH = 16'h0188;
    localparam logic [15:0] CMD_DATASLOT_OPEN  = 16'h0192;
    localparam logic [15:0] ERR_TIMEOUT        = 16'hFFFD;
    localparam logic [15:0] ERR_CANCELLED      = 16'hFFFE;
    localparam integer COMMAND_TIMEOUT_LIMIT =
        (COMMAND_TIMEOUT_CYCLES < 1) ? 0 : COMMAND_TIMEOUT_CYCLES - 1;

    logic [31:0] offset;
    logic [31:0] timeout_counter;
    logic [31:0] current_chunk_bytes;

    always_comb begin
        if ((PAYLOAD_BYTES - offset) > CHUNK_BYTES) begin
            current_chunk_bytes = CHUNK_BYTES;
        end else begin
            current_chunk_bytes = PAYLOAD_BYTES - offset;
        end

        request_valid  = 1'b0;
        request_code   = 16'h0000;
        request_param0 = 32'h00000000;
        request_param1 = 32'h00000000;
        request_param2 = 32'h00000000;
        request_param3 = 32'h00000000;

        case (state)
            ST_OPEN_REQ: begin
                request_valid  = 1'b1;
                request_code   = CMD_DATASLOT_OPEN;
                request_param0 = {16'h0000, SLOT_ID};
                request_param1 = OPEN_PARAM_ADDR;
            end

            ST_WRITE_REQ: begin
                request_valid  = 1'b1;
                request_code   = CMD_DATASLOT_WRITE;
                request_param0 = {16'h0000, SLOT_ID};
                request_param1 = offset;
                request_param2 = PAYLOAD_ADDR + offset;
                request_param3 = current_chunk_bytes;
            end

            ST_FLUSH_REQ: begin
                request_valid  = 1'b1;
                request_code   = CMD_DATASLOT_FLUSH;
                request_param0 = {16'h0000, SLOT_ID};
            end

            default: begin end
        endcase
    end

    assign busy = (state != ST_IDLE);

    always_ff @(posedge clk or negedge por_n) begin
        if (!por_n) begin
            state            <= ST_IDLE;
            done             <= 1'b0;
            success          <= 1'b0;
            error_code       <= 16'h0000;
            bytes_committed  <= 32'h00000000;
            offset           <= 32'h00000000;
            timeout_counter  <= 32'h00000000;
            request_abort    <= 1'b0;
        end else begin
            done          <= 1'b0;
            request_abort <= 1'b0;

            if (cancel && state != ST_IDLE) begin
                state           <= ST_IDLE;
                done            <= 1'b1;
                success         <= 1'b0;
                error_code      <= ERR_CANCELLED;
                timeout_counter <= 32'h00000000;
                request_abort   <= 1'b1;
            end else begin
                case (state)
                    ST_IDLE: begin
                        timeout_counter <= 32'h00000000;
                        if (start) begin
                            state           <= ST_OPEN_REQ;
                            success         <= 1'b0;
                            error_code      <= 16'h0000;
                            bytes_committed <= 32'h00000000;
                            offset          <= 32'h00000000;
                        end
                    end

                    ST_OPEN_REQ: begin
                        if (request_accept) begin
                            state           <= ST_OPEN_WAIT;
                            timeout_counter <= 32'h00000000;
                        end else if (timeout_counter >= COMMAND_TIMEOUT_LIMIT) begin
                            state           <= ST_IDLE;
                            done            <= 1'b1;
                            success         <= 1'b0;
                            error_code      <= ERR_TIMEOUT;
                            timeout_counter <= 32'h00000000;
                            request_abort   <= 1'b1;
                        end else begin
                            timeout_counter <= timeout_counter + 1'b1;
                        end
                    end

                    ST_OPEN_WAIT: begin
                        if (request_done) begin
                            timeout_counter <= 32'h00000000;
                            if (request_result == 16'h0000 || request_result == 16'h0001) begin
                                if (PAYLOAD_BYTES == 0) begin
                                    state <= ST_FLUSH_REQ;
                                end else begin
                                    state <= ST_WRITE_REQ;
                                end
                            end else begin
                                state      <= ST_IDLE;
                                done       <= 1'b1;
                                success    <= 1'b0;
                                error_code <= request_result;
                            end
                        end else if (timeout_counter >= COMMAND_TIMEOUT_LIMIT) begin
                            state           <= ST_IDLE;
                            done            <= 1'b1;
                            success         <= 1'b0;
                            error_code      <= ERR_TIMEOUT;
                            timeout_counter <= 32'h00000000;
                            request_abort   <= 1'b1;
                        end else begin
                            timeout_counter <= timeout_counter + 1'b1;
                        end
                    end

                    ST_WRITE_REQ: begin
                        if (request_accept) begin
                            state           <= ST_WRITE_WAIT;
                            timeout_counter <= 32'h00000000;
                        end else if (timeout_counter >= COMMAND_TIMEOUT_LIMIT) begin
                            state           <= ST_IDLE;
                            done            <= 1'b1;
                            success         <= 1'b0;
                            error_code      <= ERR_TIMEOUT;
                            timeout_counter <= 32'h00000000;
                            request_abort   <= 1'b1;
                        end else begin
                            timeout_counter <= timeout_counter + 1'b1;
                        end
                    end

                    ST_WRITE_WAIT: begin
                        if (request_done) begin
                            timeout_counter <= 32'h00000000;
                            if (request_result == 16'h0000) begin
                                bytes_committed <= bytes_committed + current_chunk_bytes;
                                offset          <= offset + current_chunk_bytes;
                                if ((offset + current_chunk_bytes) >= PAYLOAD_BYTES) begin
                                    state <= ST_FLUSH_REQ;
                                end else begin
                                    state <= ST_WRITE_REQ;
                                end
                            end else begin
                                state      <= ST_IDLE;
                                done       <= 1'b1;
                                success    <= 1'b0;
                                error_code <= request_result;
                            end
                        end else if (timeout_counter >= COMMAND_TIMEOUT_LIMIT) begin
                            state           <= ST_IDLE;
                            done            <= 1'b1;
                            success         <= 1'b0;
                            error_code      <= ERR_TIMEOUT;
                            timeout_counter <= 32'h00000000;
                            request_abort   <= 1'b1;
                        end else begin
                            timeout_counter <= timeout_counter + 1'b1;
                        end
                    end

                    ST_FLUSH_REQ: begin
                        if (request_accept) begin
                            state           <= ST_FLUSH_WAIT;
                            timeout_counter <= 32'h00000000;
                        end else if (timeout_counter >= COMMAND_TIMEOUT_LIMIT) begin
                            state           <= ST_IDLE;
                            done            <= 1'b1;
                            success         <= 1'b0;
                            error_code      <= ERR_TIMEOUT;
                            timeout_counter <= 32'h00000000;
                            request_abort   <= 1'b1;
                        end else begin
                            timeout_counter <= timeout_counter + 1'b1;
                        end
                    end

                    ST_FLUSH_WAIT: begin
                        if (request_done) begin
                            state           <= ST_IDLE;
                            done            <= 1'b1;
                            timeout_counter <= 32'h00000000;
                            if (request_result == 16'h0000) begin
                                success    <= 1'b1;
                                error_code <= 16'h0000;
                            end else begin
                                success    <= 1'b0;
                                error_code <= request_result;
                            end
                        end else if (timeout_counter >= COMMAND_TIMEOUT_LIMIT) begin
                            state           <= ST_IDLE;
                            done            <= 1'b1;
                            success         <= 1'b0;
                            error_code      <= ERR_TIMEOUT;
                            timeout_counter <= 32'h00000000;
                            request_abort   <= 1'b1;
                        end else begin
                            timeout_counter <= timeout_counter + 1'b1;
                        end
                    end

                    default: begin
                        state           <= ST_IDLE;
                        done            <= 1'b1;
                        success         <= 1'b0;
                        error_code      <= 16'hFFFF;
                        timeout_counter <= 32'h00000000;
                    end
                endcase
            end
        end
    end

endmodule

`default_nettype wire
