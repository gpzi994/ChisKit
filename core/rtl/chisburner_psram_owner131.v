// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Memory-clock-domain lease for BOTH PSRAM controller ports.
// legacy_pending includes queued writes, issued reads and upstream requests
// already accepted before legacy_accept fell. Such work MUST keep draining.
// stream_pending includes the complete CDC handshake, not just reader_busy.
// lease must be a synchronized level, held until the stream job is finished.
// On cancellation, stop source admission first, then lower lease; queued stream
// work still owns the ports until it has drained. No independent owner reset.
module chisburner_psram_owner131(
 input wire clk,reset_n,lease,
 input wire legacy_pending,stream_pending,
 input wire p0_busy,p1_busy,p0_valid,p1_valid,
 output wire legacy_accept,legacy_owner,stream_accept,stream_owner,
 output wire acquiring,releasing
);
 localparam [1:0] LEGACY=0,ACQUIRE=1,STREAM=2,RELEASE=3;
 reg [1:0] state;
 reg quiet;
 wire physical_idle=!(p0_busy || p1_busy || p0_valid || p1_valid);
 assign legacy_accept=reset_n && state==LEGACY && !lease;
 assign legacy_owner=reset_n && (state==LEGACY || state==ACQUIRE);
 assign stream_accept=reset_n && state==STREAM && lease;
 assign stream_owner=reset_n && (state==STREAM || state==RELEASE);
 assign acquiring=state==ACQUIRE;
 assign releasing=state==RELEASE;
 always @(posedge clk or negedge reset_n)begin
  if(!reset_n)begin state<=LEGACY;quiet<=0;end
  else begin
   case(state)
    LEGACY:begin quiet<=0;if(lease)state<=ACQUIRE;end
    ACQUIRE:begin
     if(!lease)begin state<=LEGACY;quiet<=0;end
     else if(!legacy_pending && physical_idle)begin
      if(quiet)begin state<=STREAM;quiet<=0;end else quiet<=1;
     end else quiet<=0;
    end
    STREAM:begin quiet<=0;if(!lease)state<=RELEASE;end
    RELEASE:begin
     // Never reopen admission mid-drain, even if lease rises again.
     if(!stream_pending && physical_idle)begin
      if(quiet)begin state<=LEGACY;quiet<=0;end else quiet<=1;
     end else quiet<=0;
    end
   endcase
  end
 end
endmodule
`default_nettype wire
