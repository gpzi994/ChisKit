// smoke.56: two repeatable 0x180-byte GBA headers plus one fixed NOR probe.
// The identification space is a constant RTL sequence, not a write API.
localparam integer CHISBURNER_READS_ENABLED = 1;
localparam integer CHISBURNER_ADDRESS_SETUP = 12;
localparam integer CHISBURNER_SELECT_SETUP = 6;
localparam integer CHISBURNER_TURNAROUND = 3;
localparam integer CHISBURNER_DATA_WAIT = 18;
localparam integer CHISBURNER_RELEASE = 6;
localparam integer CHISBURNER_MAX_READ_COMMANDS = 24;
localparam integer CHISBURNER_EXACT_READ_WINDOW = 0;
localparam integer CHISBURNER_REQUIRED_READ_BYTES = 32;
localparam [31:0] CHISBURNER_READ_ADDRESS_MASK = 32'h0000001f;
localparam integer CHISBURNER_FIXED_ADDRESS_SET = 0;
localparam [31:0] CHISBURNER_READ_WINDOW_BASE = 32'h00000000;
localparam [31:0] CHISBURNER_READ_WINDOW_BYTES = 32'h00000180;
