// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Four-phase whole-job lease, CPU clock <-> memory clock. Reset must be common
// with the data CDC and physical controllers, with synchronized local release.
// request is the parent's level request; hold covers the ENTIRE program/data
// adapter drain. Never remove memory ownership merely because request falls.
module chisburner_lease_cdc131 #(
 parameter EXTERNAL_OWNER=0
)(
 input wire s_clk,s_reset_n,request,cancel,lifecycle_ok,hold,
 output wire granted,busy,
 input wire m_clk,m_reset_n,legacy_pending,stream_pending,
 input wire p0_busy,p1_busy,p0_valid,p1_valid,
 output wire legacy_accept,legacy_owner,stream_owner,stream_accept,
 output wire m_lease,input wire m_granted,m_returned
);
 localparam [1:0] IDLE=0,ACQUIRE=1,ACTIVE=2,RELEASE=3;
 reg [1:0] state;
 reg lease,revoked;
 (* ASYNC_REG="TRUE" *) reg [2:0] lease_sync,grant_sync,returned_sync;
 wire returned=(EXTERNAL_OWNER?m_returned:legacy_accept) && !lease_sync[2];
 assign m_lease=lease_sync[2];
 assign busy=state!=IDLE;
 assign granted=state==ACTIVE && grant_sync[2] && request &&
  lifecycle_ok && !cancel && !revoked;
 always @(posedge s_clk or negedge s_reset_n)begin
  if(!s_reset_n)begin
   state<=IDLE;lease<=0;revoked<=0;grant_sync<=0;returned_sync<=0;
  end else begin
   grant_sync<={grant_sync[1:0],stream_accept};
   returned_sync<={returned_sync[1:0],returned};
   if(state!=IDLE && (!request || cancel || !lifecycle_ok))revoked<=1;
   case(state)
    IDLE:begin
     revoked<=0;
     if(request && !cancel && lifecycle_ok && !hold)begin lease<=1;state<=ACQUIRE;end
    end
    ACQUIRE:if(grant_sync[2])state<=ACTIVE;
    ACTIVE:if((revoked || !request || cancel || !lifecycle_ok) && !hold)begin
     lease<=0;state<=RELEASE;
    end
    RELEASE:if(!grant_sync[2] && returned_sync[2])state<=IDLE;
   endcase
  end
 end
 always @(posedge m_clk or negedge m_reset_n)
  if(!m_reset_n)lease_sync<=0;else lease_sync<={lease_sync[1:0],lease};
 generate if(!EXTERNAL_OWNER)begin:internal_owner
 chisburner_psram_owner131 owner(
  .clk(m_clk),.reset_n(m_reset_n),.lease(lease_sync[2]),
  .legacy_pending(legacy_pending),.stream_pending(stream_pending),
  .p0_busy(p0_busy),.p1_busy(p1_busy),.p0_valid(p0_valid),.p1_valid(p1_valid),
  .legacy_accept(legacy_accept),.legacy_owner(legacy_owner),
  .stream_owner(stream_owner),.stream_accept(stream_accept));
 end else begin:external_owner
 // No second physical owner. Ownership/mux outputs must come from the unified
 // arbiter, not these unused legacy interface outputs.
 assign legacy_accept=1'b0;
 assign legacy_owner=1'b0;
 assign stream_owner=1'b0;
 assign stream_accept=m_granted;
 end endgenerate
endmodule
`default_nettype wire
