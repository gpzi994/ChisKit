`default_nettype none

module pct_core_skeleton #(
    parameter integer CART_SETTLE_CYCLES      = 1024,
    parameter integer STORAGE_PAYLOAD_BYTES   = 256,
    parameter integer STORAGE_CHUNK_BYTES     = 64,
    parameter integer STORAGE_TIMEOUT_CYCLES  = 1024
) (
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
    input  logic        protocol_agb,
    input  logic        cart_abort,
    input  logic        storage_test_start,
    input  logic        storage_test_cancel,

    output logic        core_reset_n,
    output logic [2:0]  cart_guard_state,
    output logic        cart_read_ready,
    output logic        cart_writes_enabled,
    output logic        storage_start_blocked,
    output logic        storage_busy,
    output logic        storage_done,
    output logic        storage_success,
    output logic [15:0] storage_error_code,
    output logic [31:0] storage_bytes_committed,

    output logic [7:0]  cart_bank3_out,
    output logic        cart_bank3_dir,
    output logic [7:0]  cart_bank2_out,
    output logic        cart_bank2_dir,
    output logic [7:0]  cart_bank1_out,
    output logic        cart_bank1_dir,
    output logic [3:0]  cart_bank0_out,
    output logic        cart_bank0_dir,
    output logic        cart_pin30_out,
    output logic        cart_pin30_dir,
    output logic        cart_pin30_pwroff_reset,
    output logic        cart_pin31_out,
    output logic        cart_pin31_dir
);

    localparam logic [15:0] STORAGE_SLOT_ID         = 16'h0100;
    localparam logic [31:0] STORAGE_OPEN_PARAM_ADDR = 32'hF8004000;
    localparam logic [31:0] STORAGE_PAYLOAD_ADDR    = 32'hF8004200;

    logic        host_claimed;
    logic [31:0] host_rd_data;
    logic        target_claimed;
    logic [31:0] target_rd_data;
    logic        buffer_claimed;
    logic [31:0] buffer_rd_data;

    logic        host_b1_strobe;
    logic [31:0] host_b1_value;
    logic        host_reset_enter_strobe;
    logic        host_reset_exit_strobe;
    logic        cart_play_selected;
    logic        cart_power_after_reset_exit;
    logic [7:0]  cart_adapter_id;
    logic        cart_force_safe;

    logic        target_request_valid;
    logic [15:0] target_request_code;
    logic [31:0] target_request_param0;
    logic [31:0] target_request_param1;
    logic [31:0] target_request_param2;
    logic [31:0] target_request_param3;
    logic        target_request_abort;
    logic        target_request_accept;
    logic        target_request_done;
    logic [15:0] target_request_result;
    logic [3:0]  storage_state;
    logic        storage_allowed;
    logic        storage_start_internal;
    logic        storage_cancel_internal;

    assign storage_allowed         = !cart_play_selected && !cart_read_ready;
    assign storage_start_internal  = storage_test_start && storage_allowed;
    assign storage_cancel_internal = storage_test_cancel || cart_play_selected || cart_abort;

    always_ff @(posedge clk or negedge por_n) begin
        if (!por_n) begin
            storage_start_blocked <= 1'b0;
        end else begin
            storage_start_blocked <= storage_test_start && !storage_allowed;
        end
    end

    always_comb begin
        bridge_claimed = host_claimed || target_claimed || buffer_claimed;
        if (host_claimed) begin
            bridge_rd_data = host_rd_data;
        end else if (target_claimed) begin
            bridge_rd_data = target_rd_data;
        end else if (buffer_claimed) begin
            bridge_rd_data = buffer_rd_data;
        end else begin
            bridge_rd_data = 32'h00000000;
        end
    end

    pct_apf_host_port host_port (
        .clk                     (clk),
        .por_n                   (por_n),
        .bridge_endian_little    (bridge_endian_little),
        .bridge_addr             (bridge_addr),
        .bridge_rd               (bridge_rd),
        .bridge_wr               (bridge_wr),
        .bridge_wr_data          (bridge_wr_data),
        .bridge_claimed          (host_claimed),
        .bridge_rd_data          (host_rd_data),
        .status_boot_done        (status_boot_done),
        .status_setup_done       (status_setup_done),
        .core_reset_n            (core_reset_n),
        .host_b1_strobe          (host_b1_strobe),
        .host_b1_value           (host_b1_value),
        .host_reset_enter_strobe (host_reset_enter_strobe),
        .host_reset_exit_strobe  (host_reset_exit_strobe)
    );

    pct_cart_guard #(
        .SETTLE_CYCLES (CART_SETTLE_CYCLES)
    ) cart_guard (
        .clk                     (clk),
        .por_n                   (por_n),
        .host_b1_strobe          (host_b1_strobe),
        .host_b1_value           (host_b1_value),
        .host_reset_enter_strobe (host_reset_enter_strobe),
        .host_reset_exit_strobe  (host_reset_exit_strobe),
        .abort                   (cart_abort),
        .state                   (cart_guard_state),
        .play_selected           (cart_play_selected),
        .power_after_reset_exit  (cart_power_after_reset_exit),
        .adapter_id              (cart_adapter_id),
        .read_ready              (cart_read_ready),
        .force_safe              (cart_force_safe),
        .writes_enabled          (cart_writes_enabled)
    );

    pct_cart_safe_pins cart_safe_pins (
        .read_ready          (cart_read_ready),
        .protocol_agb        (protocol_agb),
        .bank3_out           (cart_bank3_out),
        .bank3_dir           (cart_bank3_dir),
        .bank2_out           (cart_bank2_out),
        .bank2_dir           (cart_bank2_dir),
        .bank1_out           (cart_bank1_out),
        .bank1_dir           (cart_bank1_dir),
        .bank0_out           (cart_bank0_out),
        .bank0_dir           (cart_bank0_dir),
        .pin30_out           (cart_pin30_out),
        .pin30_dir           (cart_pin30_dir),
        .pin30_pwroff_reset  (cart_pin30_pwroff_reset),
        .pin31_out           (cart_pin31_out),
        .pin31_dir           (cart_pin31_dir)
    );

    pct_storage_sm #(
        .SLOT_ID                (STORAGE_SLOT_ID),
        .OPEN_PARAM_ADDR        (STORAGE_OPEN_PARAM_ADDR),
        .PAYLOAD_ADDR           (STORAGE_PAYLOAD_ADDR),
        .PAYLOAD_BYTES          (STORAGE_PAYLOAD_BYTES),
        .CHUNK_BYTES            (STORAGE_CHUNK_BYTES),
        .COMMAND_TIMEOUT_CYCLES (STORAGE_TIMEOUT_CYCLES)
    ) storage_sm (
        .clk              (clk),
        .por_n            (por_n),
        .start            (storage_start_internal),
        .cancel           (storage_cancel_internal),
        .request_valid    (target_request_valid),
        .request_code     (target_request_code),
        .request_param0   (target_request_param0),
        .request_param1   (target_request_param1),
        .request_param2   (target_request_param2),
        .request_param3   (target_request_param3),
        .request_abort    (target_request_abort),
        .request_accept   (target_request_accept),
        .request_done     (target_request_done),
        .request_result   (target_request_result),
        .busy             (storage_busy),
        .done             (storage_done),
        .success          (storage_success),
        .error_code       (storage_error_code),
        .bytes_committed  (storage_bytes_committed),
        .state            (storage_state)
    );

    pct_apf_target_port target_port (
        .clk                  (clk),
        .por_n                (por_n),
        .bridge_endian_little (bridge_endian_little),
        .bridge_addr          (bridge_addr),
        .bridge_rd            (bridge_rd),
        .bridge_wr            (bridge_wr),
        .bridge_wr_data       (bridge_wr_data),
        .bridge_claimed       (target_claimed),
        .bridge_rd_data       (target_rd_data),
        .request_valid        (target_request_valid),
        .request_code         (target_request_code),
        .request_param0       (target_request_param0),
        .request_param1       (target_request_param1),
        .request_param2       (target_request_param2),
        .request_param3       (target_request_param3),
        .request_abort        (target_request_abort),
        .request_accept       (target_request_accept),
        .request_done         (target_request_done),
        .request_result       (target_request_result)
    );

    pct_storage_buffers #(
        .OPEN_PARAM_ADDR (STORAGE_OPEN_PARAM_ADDR),
        .PAYLOAD_ADDR    (STORAGE_PAYLOAD_ADDR),
        .PAYLOAD_BYTES   (STORAGE_PAYLOAD_BYTES)
    ) storage_buffers (
        .bridge_endian_little (bridge_endian_little),
        .bridge_addr          (bridge_addr),
        .bridge_rd            (bridge_rd),
        .bridge_claimed       (buffer_claimed),
        .bridge_rd_data       (buffer_rd_data)
    );

endmodule

`default_nettype wire
