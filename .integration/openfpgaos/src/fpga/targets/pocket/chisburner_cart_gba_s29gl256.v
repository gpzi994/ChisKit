//------------------------------------------------------------------------------
// SPDX-License-Identifier: Apache-2.0
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: (c) 2026, ChisBurner contributors
//------------------------------------------------------------------------------
// S29GL256S-only GBA cartridge programmer wrapper.
//
// This block intentionally does not expose a generic GBA write primitive.
// The only destructive commands are one 128 KiB sector erase and one aligned
// one boundary-contained AMD write-buffer operation of 2..512 bytes.  The
// 512-byte ceiling is the exact S29GL256S CFI buffer size; short final writes
// are permitted but may not cross a 512-byte write-buffer boundary.  Command
// addresses and opcodes are
// constants in this RTL.  Every destructive command requires a fresh two-key
// arm and a successful hardware-owned ID/CFI probe from the read-only engine.
// Chip erase is not implemented.
//------------------------------------------------------------------------------

`default_nettype none

module chisburner_cart_gba_s29gl256 #(
    parameter integer ADDRESS_SETUP_CYCLES = 12,
    parameter integer SELECT_SETUP_CYCLES  = 6,
    parameter integer TURNAROUND_CYCLES    = 3,
    parameter integer DATA_WAIT_CYCLES     = 18,
    parameter integer RELEASE_CYCLES       = 6,
    parameter integer MAX_READ_COMMANDS    = 0,
    parameter integer MAX_IDENTIFY_COMMANDS = 0
) (
    input  wire        clk,
    input  wire        reset_n,
    input  wire        lifecycle_ok,
    input  wire [31:0] lifecycle_status,
    input  wire [31:0] host_b1_raw,
    input  wire [31:0] host_diag,

    input  wire        reg_wr,
    input  wire        reg_rd,
    input  wire [4:0]  reg_addr,
    input  wire [31:0] reg_wdata,
    output reg  [31:0] reg_rdata,

    input  wire [7:0]  bank3_in,
    input  wire [7:0]  bank2_in,

    output wire [7:0]  bank3_out,
    output wire        bank3_dir,
    output wire [7:0]  bank2_out,
    output wire        bank2_dir,
    output wire [7:0]  bank1_out,
    output wire        bank1_dir,
    output wire [3:0]  bank0_out,
    output wire        bank0_dir,
    output wire        pin30_out,
    output wire        pin30_dir,
    output wire        pin30_pwroff_reset,
    output wire        pin31_out,
    output wire        pin31_dir,
    output wire        writes_enabled
);

localparam [4:0] REG_CAPS         = 5'd1;
localparam [4:0] REG_STATUS       = 5'd2;
localparam [4:0] REG_COMMAND      = 5'd3;
localparam [4:0] REG_ADDRESS      = 5'd5;
localparam [4:0] REG_LENGTH       = 5'd6;
localparam [4:0] REG_TIMEOUT      = 5'd7;
localparam [4:0] REG_ERROR        = 5'd10;
localparam [4:0] REG_TX_DATA      = 5'd17;
localparam [4:0] REG_TX_LEVEL     = 5'd18;
localparam [4:0] REG_ARM          = 5'd19;
localparam [4:0] REG_PROFILE      = 5'd20;
localparam [4:0] REG_FLASH_BYTES  = 5'd21;
localparam [4:0] REG_SECTOR_BYTES = 5'd22;
localparam [4:0] REG_BUFFER_BYTES = 5'd23;

localparam [31:0] CMD_SAFE_IDLE    = 32'd1;
localparam [31:0] CMD_READ         = 32'd2;
localparam [31:0] CMD_ABORT        = 32'd3;
localparam [31:0] CMD_ERASE_SECTOR = 32'd4;
localparam [31:0] CMD_PROGRAM_BUFFER = 32'd5;
localparam [31:0] CMD_DISARM       = 32'd6;
localparam [31:0] CMD_QUERY_FW_INFO = 32'h0000_00A1;
localparam [31:0] CMD_PING          = 32'h0000_00FE;

localparam [31:0] ARM_KEY_1 = 32'h5332_3947; // bytes: "G92S"
localparam [31:0] ARM_KEY_2 = 32'h3635_324C; // bytes: "L256"

localparam [31:0] ST_IDLE    = 32'h0000_0001;
localparam [31:0] ST_BUSY    = 32'h0000_0002;
localparam [31:0] ST_DONE    = 32'h0000_0004;
localparam [31:0] ST_ERROR   = 32'h0000_0008;
localparam [31:0] ST_ABORTED = 32'h0000_0010;
localparam [31:0] ST_TIMEOUT = 32'h0000_0020;

localparam [31:0] ERR_NONE          = 32'd0;
localparam [31:0] ERR_BAD_COMMAND   = 32'd1;
localparam [31:0] ERR_LIFECYCLE     = 32'd3;
localparam [31:0] ERR_TIMEOUT       = 32'd4;
localparam [31:0] ERR_NOT_IDENTIFIED = 32'd5;
localparam [31:0] ERR_NOT_ARMED      = 32'd6;
localparam [31:0] ERR_BAD_RANGE      = 32'd7;
localparam [31:0] ERR_TX_LENGTH      = 32'd8;
localparam [31:0] ERR_FLASH_FAIL     = 32'd9;
localparam [31:0] ERR_WB_ABORT       = 32'd10;

// Reader capability layout plus dedicated erase/program bits [13:12].
// GBA spaces = ROM + IDENTIFY, operations = read/safe/abort/query/erase/program.
localparam [31:0] CART_CAPS = 32'h0220_3F02;

localparam [3:0] S_IDLE      = 4'd0;
localparam [3:0] S_W_ADDRESS = 4'd1;
localparam [3:0] S_W_SELECT  = 4'd2;
localparam [3:0] S_W_DATA    = 4'd3;
localparam [3:0] S_W_PULSE   = 4'd4;
localparam [3:0] S_W_RELEASE = 4'd5;
localparam [3:0] S_R_ADDRESS = 4'd6;
localparam [3:0] S_R_SELECT  = 4'd7;
localparam [3:0] S_R_TURN    = 4'd8;
localparam [3:0] S_R_READ    = 4'd9;
localparam [3:0] S_R_RELEASE = 4'd10;

localparam [5:0] P_NONE       = 6'd0;
localparam [5:0] P_E_AA1      = 6'd1;
localparam [5:0] P_E_55_1     = 6'd2;
localparam [5:0] P_E_80       = 6'd3;
localparam [5:0] P_E_AA2      = 6'd4;
localparam [5:0] P_E_55_2     = 6'd5;
localparam [5:0] P_E_30       = 6'd6;
localparam [5:0] P_E_POLL     = 6'd7;
localparam [5:0] P_P_AA       = 6'd8;
localparam [5:0] P_P_55       = 6'd9;
localparam [5:0] P_P_25       = 6'd10;
localparam [5:0] P_P_COUNT    = 6'd11;
localparam [5:0] P_P_DATA     = 6'd12;
localparam [5:0] P_P_29       = 6'd13;
localparam [5:0] P_P_POLL     = 6'd14;
localparam [5:0] P_RESET_DONE = 6'd15;
localparam [5:0] P_RECOVERY   = 6'd16;
localparam [5:0] P_E_RECHECK  = 6'd17;
localparam [5:0] P_P_RECHECK  = 6'd18;
localparam [5:0] P_FAIL_RESET = 6'd19;
localparam [5:0] P_WBA_AA     = 6'd20;
localparam [5:0] P_WBA_55     = 6'd21;
localparam [5:0] P_WBA_F0     = 6'd22;

wire [31:0] reader_rdata;
wire [7:0] reader_bank3_out;
wire reader_bank3_dir;
wire [7:0] reader_bank2_out;
wire reader_bank2_dir;
wire [7:0] reader_bank1_out;
wire reader_bank1_dir;
wire [3:0] reader_bank0_out;
wire reader_bank0_dir;
wire reader_pin30_out;
wire reader_pin30_dir;
wire reader_pin30_pwroff_reset;
wire reader_pin31_out;
wire reader_pin31_dir;
wire reader_writes_enabled;
wire reader_busy;
wire reader_profile_verified;

reg [3:0] writer_state;
reg [5:0] writer_phase;
reg [31:0] writer_status;
reg [31:0] writer_error;
reg [31:0] address_reg;
reg [31:0] length_reg;
reg [31:0] timeout_reg;
reg [31:0] phase_counter;
reg [31:0] watchdog_counter;
reg [23:0] word_address;
reg [15:0] write_data;
reg [15:0] read_data;
reg [9:0] tx_count;
reg [1:0] arm_stage;
reg [7:0] program_word_index;
reg [6:0] tx_read_address;
reg [31:0] tx_read_data;
reg tx_last_low_dq7;
reg tx_last_high_dq7;
reg expected_program_dq7;
reg writer_selected;
reg [31:0] recovery_status;
reg [31:0] recovery_error;
(* ramstyle = "M10K, no_rw_check" *) reg [31:0] tx_fifo [0:127];

reg [7:0] writer_bank3_out;
reg writer_bank3_dir;
reg [7:0] writer_bank2_out;
reg writer_bank2_dir;
reg [7:0] writer_bank1_out;
reg writer_bank1_dir;
reg [3:0] writer_bank0_out;
reg writer_bank0_dir;
reg writer_pin30_out;
reg writer_pin30_dir;

wire writer_active = writer_state != S_IDLE;
wire writer_recovering = writer_phase == P_RECOVERY ||
                         writer_phase == P_FAIL_RESET ||
                         writer_phase == P_WBA_AA ||
                         writer_phase == P_WBA_55 ||
                         writer_phase == P_WBA_F0;
wire program_sequence_active = (writer_phase >= P_P_AA &&
                                writer_phase <= P_P_POLL) ||
                               writer_phase == P_P_RECHECK;
wire arm_valid = arm_stage == 2;
wire erase_address_valid = address_reg < 32'h0200_0000 &&
                           (address_reg & 32'h0001_FFFF) == 0 &&
                           length_reg == 32'h0002_0000;
wire program_address_valid = address_reg < 32'h0200_0000 &&
                              (address_reg & 32'h0000_0001) == 0 &&
                              length_reg >= 2 && length_reg <= 512 &&
                              (length_reg & 32'h0000_0001) == 0 &&
                              address_reg <= 32'h0200_0000 - length_reg &&
                              {1'b0, address_reg[8:0]} +
                                  length_reg[9:0] <= 10'd512;
wire [23:0] sector_word_address = {address_reg[24:17], 16'h0000};
wire [8:0] program_words = length_reg[9:1];
wire [9:0] required_tx_count =
    (length_reg[9:0] + 10'd3) & 10'h3FC;
wire [7:0] last_program_word_index = program_words[7:0] - 1'b1;
wire next_program_is_high = !program_word_index[0];
wire [15:0] next_program_word = next_program_is_high ?
                                tx_read_data[31:16] :
                                tx_read_data[15:0];

wire writer_register = reg_addr >= REG_TX_DATA && reg_addr <= REG_BUFFER_BYTES;
wire writer_command = reg_addr == REG_COMMAND &&
                      (reg_wdata == CMD_ERASE_SECTOR ||
                       reg_wdata == CMD_PROGRAM_BUFFER ||
                       reg_wdata == CMD_DISARM ||
                       (writer_active && reg_wdata == CMD_ABORT));
wire reader_reg_wr = reg_wr && !writer_register && !writer_command &&
                     !writer_active;

chisburner_cart_gba_readonly #(
    .READS_ENABLED(1),
    .ADDRESS_SETUP_CYCLES(ADDRESS_SETUP_CYCLES),
    .SELECT_SETUP_CYCLES(SELECT_SETUP_CYCLES),
    .TURNAROUND_CYCLES(TURNAROUND_CYCLES),
    .DATA_WAIT_CYCLES(DATA_WAIT_CYCLES),
    .RELEASE_CYCLES(RELEASE_CYCLES),
    .MAX_READ_BYTES(32),
    .READ_WINDOW_BASE(32'h0000_0000),
    .READ_WINDOW_BYTES(32'h0200_0000),
    .EXACT_READ_WINDOW(0),
    .REQUIRED_READ_BYTES(32),
    .READ_ADDRESS_MASK(32'h0000_001F),
    .FIXED_ADDRESS_SET(0),
    .MAX_READ_COMMANDS(MAX_READ_COMMANDS),
    .IDENTIFY_ENABLED(1),
    .MAX_IDENTIFY_COMMANDS(MAX_IDENTIFY_COMMANDS)
) reader (
    .clk(clk), .reset_n(reset_n),
    .lifecycle_ok(lifecycle_ok), .lifecycle_status(lifecycle_status),
    .host_b1_raw(host_b1_raw), .host_diag(host_diag),
    .reg_wr(reader_reg_wr), .reg_rd(reg_rd), .reg_addr(reg_addr),
    .reg_wdata(reg_wdata), .reg_rdata(reader_rdata),
    .bank3_in(bank3_in), .bank2_in(bank2_in),
    .bank3_out(reader_bank3_out), .bank3_dir(reader_bank3_dir),
    .bank2_out(reader_bank2_out), .bank2_dir(reader_bank2_dir),
    .bank1_out(reader_bank1_out), .bank1_dir(reader_bank1_dir),
    .bank0_out(reader_bank0_out), .bank0_dir(reader_bank0_dir),
    .pin30_out(reader_pin30_out), .pin30_dir(reader_pin30_dir),
    .pin30_pwroff_reset(reader_pin30_pwroff_reset),
    .pin31_out(reader_pin31_out), .pin31_dir(reader_pin31_dir),
    .writes_enabled(reader_writes_enabled), .busy(reader_busy),
    .profile_verified(reader_profile_verified)
);

assign bank3_out = writer_active ? writer_bank3_out : reader_bank3_out;
assign bank3_dir = writer_active ? writer_bank3_dir : reader_bank3_dir;
assign bank2_out = writer_active ? writer_bank2_out : reader_bank2_out;
assign bank2_dir = writer_active ? writer_bank2_dir : reader_bank2_dir;
assign bank1_out = writer_active ? writer_bank1_out : reader_bank1_out;
assign bank1_dir = writer_active ? writer_bank1_dir : reader_bank1_dir;
assign bank0_out = writer_active ? writer_bank0_out : reader_bank0_out;
assign bank0_dir = writer_active ? writer_bank0_dir : reader_bank0_dir;
assign pin30_out = writer_active ? writer_pin30_out : reader_pin30_out;
assign pin30_dir = writer_active ? writer_pin30_dir : reader_pin30_dir;
assign pin30_pwroff_reset = reader_pin30_pwroff_reset;
assign pin31_out = reader_pin31_out;
assign pin31_dir = reader_pin31_dir;
assign writes_enabled = reader_profile_verified;

always @(*) begin
    reg_rdata = reader_rdata;
    case (reg_addr)
        REG_CAPS: reg_rdata = CART_CAPS;
        REG_STATUS: if (writer_selected) reg_rdata = writer_status;
        REG_ERROR: if (writer_selected) reg_rdata = writer_error;
        REG_TX_DATA: reg_rdata = 32'd0;
        REG_TX_LEVEL: reg_rdata = {22'd0, tx_count};
        REG_ARM: reg_rdata = {30'd0, arm_stage};
        REG_PROFILE: reg_rdata = {28'd0, reader_writes_enabled,
                                  writer_active, arm_valid,
                                  reader_profile_verified};
        REG_FLASH_BYTES: reg_rdata = 32'h0200_0000;
        REG_SECTOR_BYTES: reg_rdata = 32'h0002_0000;
        REG_BUFFER_BYTES: reg_rdata = 32'd512;
        default: begin end
    endcase
end

// GBA address/data multiplexing. bank0 is {PHI#, WR#, RD#, CS#}.
always @(*) begin
    writer_bank3_out = 8'h00;
    writer_bank3_dir = 1'b0;
    writer_bank2_out = 8'h00;
    writer_bank2_dir = 1'b0;
    writer_bank1_out = 8'h00;
    writer_bank1_dir = 1'b0;
    writer_bank0_out = 4'hF;
    writer_bank0_dir = 1'b1;
    writer_pin30_out = 1'b1;
    writer_pin30_dir = 1'b0;

    case (writer_state)
        S_W_ADDRESS, S_R_ADDRESS: begin
            writer_bank3_out = word_address[7:0];
            writer_bank3_dir = 1'b1;
            writer_bank2_out = word_address[15:8];
            writer_bank2_dir = 1'b1;
            writer_bank1_out = word_address[23:16];
            writer_bank1_dir = 1'b1;
            writer_bank0_out = 4'b1111;
            writer_pin30_dir = 1'b1;
        end
        S_W_SELECT, S_R_SELECT: begin
            writer_bank3_out = word_address[7:0];
            writer_bank3_dir = 1'b1;
            writer_bank2_out = word_address[15:8];
            writer_bank2_dir = 1'b1;
            writer_bank1_out = word_address[23:16];
            writer_bank1_dir = 1'b1;
            writer_bank0_out = 4'b1110;
            writer_pin30_dir = 1'b1;
        end
        S_W_DATA, S_W_PULSE, S_W_RELEASE: begin
            writer_bank3_out = write_data[7:0];
            writer_bank3_dir = 1'b1;
            writer_bank2_out = write_data[15:8];
            writer_bank2_dir = 1'b1;
            writer_bank1_out = word_address[23:16];
            writer_bank1_dir = 1'b1;
            writer_bank0_out = writer_state == S_W_PULSE ? 4'b1010 : 4'b1110;
            writer_pin30_dir = 1'b1;
        end
        S_R_TURN, S_R_READ, S_R_RELEASE: begin
            writer_bank1_out = word_address[23:16];
            writer_bank1_dir = 1'b1;
            writer_bank0_out = writer_state == S_R_READ ? 4'b1100 : 4'b1110;
            writer_pin30_dir = 1'b1;
        end
        default: begin end
    endcase
end

task launch_write;
    input [23:0] addr;
    input [15:0] data;
    input [5:0] next_phase;
    begin
        writer_state <= S_W_ADDRESS;
        word_address <= addr;
        write_data <= data;
        writer_phase <= next_phase;
        phase_counter <= 0;
    end
endtask

task launch_read;
    input [23:0] addr;
    input [5:0] next_phase;
    begin
        writer_state <= S_R_ADDRESS;
        word_address <= addr;
        writer_phase <= next_phase;
        phase_counter <= 0;
    end
endtask

task reject_command;
    input [31:0] err;
    begin
        writer_state <= S_IDLE;
        writer_status <= ST_IDLE | ST_ERROR;
        writer_error <= err;
        writer_phase <= P_NONE;
        arm_stage <= 0;
    end
endtask

// A single-write-port, synchronous-read M10K keeps the 512-byte staging area
// out of scarce ALMs.  The NOR write bus phases provide ample time to prefetch
// the next 32-bit word before it is consumed.
always @(posedge clk) begin
    if (reg_wr && !writer_active && reg_addr == REG_TX_DATA &&
        tx_count <= 508)
        tx_fifo[tx_count[8:2]] <= reg_wdata;
    tx_read_data <= tx_fifo[tx_read_address];
end

always @(posedge clk or negedge reset_n) begin
    if (!reset_n) begin
        writer_state <= S_IDLE;
        writer_phase <= P_NONE;
        writer_status <= ST_IDLE;
        writer_error <= ERR_NONE;
        address_reg <= 0;
        length_reg <= 0;
        timeout_reg <= 0;
        phase_counter <= 0;
        watchdog_counter <= 0;
        word_address <= 0;
        write_data <= 0;
        read_data <= 0;
        tx_count <= 0;
        arm_stage <= 0;
        program_word_index <= 0;
        tx_read_address <= 0;
        tx_last_low_dq7 <= 0;
        tx_last_high_dq7 <= 0;
        expected_program_dq7 <= 0;
        writer_selected <= 0;
        recovery_status <= ST_IDLE;
        recovery_error <= ERR_NONE;
    end else if (writer_active && !lifecycle_ok) begin
        writer_state <= S_IDLE;
        writer_phase <= P_NONE;
        writer_status <= ST_IDLE | ST_ERROR;
        writer_error <= ERR_LIFECYCLE;
        watchdog_counter <= 0;
        arm_stage <= 0;
        tx_count <= 0;
    end else if (writer_active && reg_wr && reg_addr == REG_COMMAND &&
                 reg_wdata == CMD_ABORT) begin
        writer_status <= ST_BUSY;
        writer_error <= ERR_NONE;
        recovery_status <= ST_IDLE | ST_ABORTED;
        recovery_error <= ERR_NONE;
        watchdog_counter <= 0;
        arm_stage <= 0;
        tx_count <= 0;
        if (program_sequence_active)
            launch_write(24'h000555, 16'h00AA, P_WBA_AA);
        else
            launch_write(24'd0, 16'h00F0, P_RECOVERY);
    end else if (writer_active && watchdog_counter + 1 >= timeout_reg &&
                 !writer_recovering) begin
        writer_status <= ST_BUSY;
        writer_error <= ERR_NONE;
        recovery_status <= ST_IDLE | ST_ERROR | ST_TIMEOUT;
        recovery_error <= ERR_TIMEOUT;
        watchdog_counter <= 0;
        arm_stage <= 0;
        tx_count <= 0;
        if (program_sequence_active)
            launch_write(24'h000555, 16'h00AA, P_WBA_AA);
        else
            launch_write(24'd0, 16'h00F0, P_RECOVERY);
    end else begin
        if (!lifecycle_ok)
            arm_stage <= 0;
        if (writer_active)
            watchdog_counter <= watchdog_counter + 1;

        if (reg_wr && !writer_active) begin
            case (reg_addr)
                REG_ADDRESS: address_reg <= reg_wdata;
                REG_LENGTH: length_reg <= reg_wdata;
                REG_TIMEOUT: timeout_reg <= reg_wdata;
                REG_TX_LEVEL: begin
                    if (reg_wdata == 0) begin
                        tx_count <= 0;
                        writer_error <= ERR_NONE;
                    end else begin
                        writer_status <= ST_IDLE | ST_ERROR;
                        writer_error <= ERR_BAD_COMMAND;
                    end
                end
                REG_TX_DATA: begin
                    if (tx_count <= 508) begin
                        tx_count <= tx_count + 4;
                        tx_last_low_dq7 <= reg_wdata[7];
                        tx_last_high_dq7 <= reg_wdata[23];
                    end else begin
                        writer_status <= ST_IDLE | ST_ERROR;
                        writer_error <= ERR_TX_LENGTH;
                        arm_stage <= 0;
                    end
                end
                REG_ARM: begin
                    if (reg_wdata == ARM_KEY_1)
                        arm_stage <= 1;
                    else if (reg_wdata == ARM_KEY_2 && arm_stage == 1)
                        arm_stage <= 2;
                    else
                        arm_stage <= 0;
                end
                REG_COMMAND: begin
                    if (reg_wdata == CMD_ERASE_SECTOR ||
                        reg_wdata == CMD_PROGRAM_BUFFER) begin
                        writer_selected <= 1;
                        if (!lifecycle_ok)
                            reject_command(ERR_LIFECYCLE);
                        else if (reader_busy)
                            reject_command(ERR_BAD_COMMAND);
                        else if (!reader_profile_verified)
                            reject_command(ERR_NOT_IDENTIFIED);
                        else if (!arm_valid)
                            reject_command(ERR_NOT_ARMED);
                        else if (timeout_reg == 0)
                            reject_command(ERR_BAD_RANGE);
                        else if (reg_wdata == CMD_ERASE_SECTOR &&
                                 !erase_address_valid)
                            reject_command(ERR_BAD_RANGE);
                        else if (reg_wdata == CMD_PROGRAM_BUFFER &&
                                 !program_address_valid)
                            reject_command(ERR_BAD_RANGE);
                        else if (reg_wdata == CMD_PROGRAM_BUFFER &&
                                 tx_count != required_tx_count)
                            reject_command(ERR_TX_LENGTH);
                        else begin
                            writer_status <= ST_BUSY;
                            writer_error <= ERR_NONE;
                            watchdog_counter <= 0;
                            arm_stage <= 0;
                            if (reg_wdata == CMD_ERASE_SECTOR)
                                launch_write(24'h000555, 16'h00AA, P_E_AA1);
                            else begin
                                tx_read_address <= 0;
                                expected_program_dq7 <= length_reg[1] ?
                                                        tx_last_low_dq7 :
                                                        tx_last_high_dq7;
                                launch_write(24'h000555, 16'h00AA, P_P_AA);
                            end
                        end
                    end else if (reg_wdata == CMD_DISARM ||
                                 reg_wdata == CMD_SAFE_IDLE) begin
                        writer_selected <= 0;
                        writer_status <= ST_IDLE;
                        writer_error <= ERR_NONE;
                        writer_phase <= P_NONE;
                        arm_stage <= 0;
                        tx_count <= 0;
                    end else if (reg_wdata == CMD_READ ||
                                 reg_wdata == CMD_ABORT ||
                                 reg_wdata == CMD_QUERY_FW_INFO ||
                                 reg_wdata == CMD_PING) begin
                        writer_selected <= 0;
                        arm_stage <= 0;
                    end
                end
                default: begin end
            endcase
        end

        case (writer_state)
            S_W_ADDRESS: begin
                if (phase_counter + 1 >= ADDRESS_SETUP_CYCLES) begin
                    writer_state <= S_W_SELECT; phase_counter <= 0;
                end else phase_counter <= phase_counter + 1;
            end
            S_W_SELECT: begin
                if (phase_counter + 1 >= SELECT_SETUP_CYCLES) begin
                    writer_state <= S_W_DATA; phase_counter <= 0;
                end else phase_counter <= phase_counter + 1;
            end
            S_W_DATA: begin
                if (phase_counter + 1 >= TURNAROUND_CYCLES) begin
                    writer_state <= S_W_PULSE; phase_counter <= 0;
                end else phase_counter <= phase_counter + 1;
            end
            S_W_PULSE: begin
                if (phase_counter + 1 >= DATA_WAIT_CYCLES) begin
                    writer_state <= S_W_RELEASE; phase_counter <= 0;
                end else phase_counter <= phase_counter + 1;
            end
            S_W_RELEASE: begin
                if (phase_counter + 1 >= RELEASE_CYCLES) begin
                    phase_counter <= 0;
                    case (writer_phase)
                        P_E_AA1: launch_write(24'h0002AA, 16'h0055, P_E_55_1);
                        P_E_55_1: launch_write(24'h000555, 16'h0080, P_E_80);
                        P_E_80: launch_write(24'h000555, 16'h00AA, P_E_AA2);
                        P_E_AA2: launch_write(24'h0002AA, 16'h0055, P_E_55_2);
                        P_E_55_2: launch_write(sector_word_address, 16'h0030, P_E_30);
                        P_E_30: launch_read(sector_word_address, P_E_POLL);
                        P_P_AA: launch_write(24'h0002AA, 16'h0055, P_P_55);
                        P_P_55: launch_write(sector_word_address, 16'h0025, P_P_25);
                        P_P_25: launch_write(sector_word_address,
                                             {{7{1'b0}}, program_words} - 16'd1,
                                             P_P_COUNT);
                        P_P_COUNT: begin
                            program_word_index <= 0;
                            launch_write(address_reg[24:1],
                                tx_read_data[15:0], P_P_DATA);
                        end
                        P_P_DATA: begin
                            if (program_word_index == last_program_word_index)
                                launch_write(sector_word_address, 16'h0029, P_P_29);
                            else begin
                                if (!program_word_index[0])
                                    tx_read_address <=
                                        program_word_index[7:1] + 1'b1;
                                program_word_index <= program_word_index + 1;
                                launch_write(address_reg[24:1] +
                                    {{16{1'b0}}, program_word_index} + 1'b1,
                                    next_program_word, P_P_DATA);
                            end
                        end
                        P_P_29: launch_read(address_reg[24:1] +
                                            {{16{1'b0}}, last_program_word_index},
                                            P_P_POLL);
                        P_RESET_DONE: begin
                            writer_state <= S_IDLE;
                            writer_phase <= P_NONE;
                            writer_status <= ST_IDLE | ST_DONE;
                            writer_error <= ERR_NONE;
                            watchdog_counter <= 0;
                            tx_count <= 0;
                        end
                        P_RECOVERY: begin
                            writer_state <= S_IDLE;
                            writer_phase <= P_NONE;
                            writer_status <= recovery_status;
                            writer_error <= recovery_error;
                            watchdog_counter <= 0;
                        end
                        P_FAIL_RESET: begin
                            writer_state <= S_IDLE;
                            writer_phase <= P_NONE;
                            writer_status <= recovery_status;
                            writer_error <= recovery_error;
                            watchdog_counter <= 0;
                            tx_count <= 0;
                        end
                        P_WBA_AA:
                            launch_write(24'h0002AA, 16'h0055, P_WBA_55);
                        P_WBA_55:
                            launch_write(24'h000555, 16'h00F0, P_WBA_F0);
                        P_WBA_F0: begin
                            writer_state <= S_IDLE;
                            writer_phase <= P_NONE;
                            writer_status <= recovery_status;
                            writer_error <= recovery_error;
                            watchdog_counter <= 0;
                            tx_count <= 0;
                        end
                        default: reject_command(ERR_BAD_COMMAND);
                    endcase
                end else phase_counter <= phase_counter + 1;
            end
            S_R_ADDRESS: begin
                if (phase_counter + 1 >= ADDRESS_SETUP_CYCLES) begin
                    writer_state <= S_R_SELECT; phase_counter <= 0;
                end else phase_counter <= phase_counter + 1;
            end
            S_R_SELECT: begin
                if (phase_counter + 1 >= SELECT_SETUP_CYCLES) begin
                    writer_state <= S_R_TURN; phase_counter <= 0;
                end else phase_counter <= phase_counter + 1;
            end
            S_R_TURN: begin
                if (phase_counter + 1 >= TURNAROUND_CYCLES) begin
                    writer_state <= S_R_READ; phase_counter <= 0;
                end else phase_counter <= phase_counter + 1;
            end
            S_R_READ: begin
                if (phase_counter + 1 >= DATA_WAIT_CYCLES) begin
                    read_data <= {bank2_in, bank3_in};
                    writer_state <= S_R_RELEASE; phase_counter <= 0;
                end else phase_counter <= phase_counter + 1;
            end
            S_R_RELEASE: begin
                if (phase_counter + 1 >= RELEASE_CYCLES) begin
                    phase_counter <= 0;
                    if (writer_phase == P_E_POLL) begin
                        if (read_data[7])
                            launch_write(24'd0, 16'h00F0, P_RESET_DONE);
                        else if (read_data[5])
                            launch_read(sector_word_address, P_E_RECHECK);
                        else
                            launch_read(sector_word_address, P_E_POLL);
                    end else if (writer_phase == P_E_RECHECK) begin
                        if (read_data[7])
                            launch_write(24'd0, 16'h00F0, P_RESET_DONE);
                        else begin
                            recovery_status <= ST_IDLE | ST_ERROR;
                            recovery_error <= ERR_FLASH_FAIL;
                            launch_write(24'd0, 16'h00F0, P_FAIL_RESET);
                        end
                    end else if (writer_phase == P_P_POLL) begin
                        if (read_data[7] == expected_program_dq7)
                            launch_write(24'd0, 16'h00F0, P_RESET_DONE);
                        else if (read_data[1]) begin
                            recovery_status <= ST_IDLE | ST_ERROR;
                            recovery_error <= ERR_WB_ABORT;
                            launch_write(24'h000555, 16'h00AA, P_WBA_AA);
                        end else if (read_data[5])
                            launch_read(address_reg[24:1] +
                                        {{16{1'b0}}, last_program_word_index},
                                        P_P_RECHECK);
                        else
                            launch_read(address_reg[24:1] +
                                        {{16{1'b0}}, last_program_word_index},
                                        P_P_POLL);
                    end else if (writer_phase == P_P_RECHECK) begin
                        if (read_data[7] == expected_program_dq7)
                            launch_write(24'd0, 16'h00F0, P_RESET_DONE);
                        else if (read_data[1]) begin
                            recovery_status <= ST_IDLE | ST_ERROR;
                            recovery_error <= ERR_WB_ABORT;
                            launch_write(24'h000555, 16'h00AA, P_WBA_AA);
                        end else begin
                            recovery_status <= ST_IDLE | ST_ERROR;
                            recovery_error <= ERR_FLASH_FAIL;
                            launch_write(24'd0, 16'h00F0, P_FAIL_RESET);
                        end
                    end else begin
                        reject_command(ERR_BAD_COMMAND);
                    end
                end else phase_counter <= phase_counter + 1;
            end
            default: begin end
        endcase
    end
end

endmodule

`default_nettype wire
