`default_nettype none

module pct_cart_safe_pins (
    input  logic       read_ready,
    input  logic       protocol_agb,

    output logic [7:0] bank3_out,
    output logic       bank3_dir,
    output logic [7:0] bank2_out,
    output logic       bank2_dir,
    output logic [7:0] bank1_out,
    output logic       bank1_dir,
    output logic [3:0] bank0_out,
    output logic       bank0_dir,
    output logic       pin30_out,
    output logic       pin30_dir,
    output logic       pin30_pwroff_reset,
    output logic       pin31_out,
    output logic       pin31_dir
);

    always_comb begin
        // Wide banks stay inputs in this skeleton. A later read sequencer may
        // drive address phases only after read_ready is asserted.
        bank3_out = 8'h00;
        bank3_dir = 1'b0;
        bank2_out = 8'h00;
        bank2_dir = 1'b0;
        bank1_out = 8'h00;
        bank1_dir = 1'b0;

        // Match the official template's inactive control default. This keeps
        // PHI/WR#/RD#/CS# high instead of allowing active-low inputs to float.
        bank0_out = 4'hF;
        bank0_dir = 1'b1;

        pin30_out          = 1'b1;
        pin30_dir          = 1'b0;
        pin30_pwroff_reset = 1'b0;

        pin31_out = 1'b0;
        pin31_dir = 1'b0;

        if (read_ready) begin
            if (protocol_agb) begin
                // GBA pin 30 is CS2#. Hold it inactive for ROM-only work.
                pin30_out = 1'b1;
                pin30_dir = 1'b1;
            end else begin
                // For GB/GBC, release the special reset/power-off latch path.
                // Hardware timing must still be measured before real use.
                pin30_pwroff_reset = 1'b1;
            end
        end
    end

endmodule

`default_nettype wire
