// smoke.57: two repeatable 0x180-byte headers, five fixed preflight samples,
// one complete 32 MiB ROM pass, and one fixed NOR identification sequence.
// Normal ROM reads remain 32-byte aligned; only the FPGA owns the constant
// JEDEC/CFI write sequence.  No program/erase or generic write-data API exists.
localparam integer CHISBURNER_READS_ENABLED = 1;
localparam integer CHISBURNER_ADDRESS_SETUP = 12;
localparam integer CHISBURNER_SELECT_SETUP = 6;
localparam integer CHISBURNER_TURNAROUND = 3;
localparam integer CHISBURNER_DATA_WAIT = 18;
localparam integer CHISBURNER_RELEASE = 6;
localparam integer CHISBURNER_MAX_READ_COMMANDS = 1048605;
localparam integer CHISBURNER_EXACT_READ_WINDOW = 0;
localparam integer CHISBURNER_REQUIRED_READ_BYTES = 32;
localparam [31:0] CHISBURNER_READ_ADDRESS_MASK = 32'h0000001f;
localparam integer CHISBURNER_FIXED_ADDRESS_SET = 0;
localparam [31:0] CHISBURNER_READ_WINDOW_BASE = 32'h00000000;
localparam [31:0] CHISBURNER_READ_WINDOW_BYTES = 32'h02000000;
