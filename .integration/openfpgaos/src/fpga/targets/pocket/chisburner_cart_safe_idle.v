//------------------------------------------------------------------------------
// ChisBurner cartridge-port safe-idle driver.
//
// This first hardware profile deliberately implements no bus transaction.
// The Pocket cartridge mechanism selects the electrical level; this module
// neither selects nor requests a voltage.
//------------------------------------------------------------------------------

`default_nettype none

module chisburner_cart_safe_idle (
    output wire [7:0] bank3_out,
    output wire       bank3_dir,
    output wire [7:0] bank2_out,
    output wire       bank2_dir,
    output wire [7:0] bank1_out,
    output wire       bank1_dir,
    output wire [3:0] bank0_out,
    output wire       bank0_dir,
    output wire       pin30_out,
    output wire       pin30_dir,
    output wire       pin30_pwroff_reset,
    output wire       pin31_out,
    output wire       pin31_dir
);

// Multiplexed address/data banks are never driven in the safe-idle profile.
assign bank3_out = 8'h00;
assign bank3_dir = 1'b0;
assign bank2_out = 8'h00;
assign bank2_dir = 1'b0;
assign bank1_out = 8'h00;
assign bank1_dir = 1'b0;

// PHI/WR#/RD#/CS# are active-low.  Drive all four inactive so they cannot
// float into a cartridge transaction while the read sequencers are absent.
assign bank0_out = 4'hF;
assign bank0_dir = 1'b1;

// The two individual pins remain released.  The pin-30 power-off/reset
// sideband is also left deasserted.
assign pin30_out          = 1'b1;
assign pin30_dir          = 1'b0;
assign pin30_pwroff_reset = 1'b0;
assign pin31_out          = 1'b0;
assign pin31_dir          = 1'b0;

endmodule

`default_nettype wire
