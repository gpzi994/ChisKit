// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// One outstanding read, four-phase bundled-data handshake. Reset must be
// asserted for BOTH domains AND the physical reader. Deassert synchronously
// in each clock domain outside this module. Independent domain resets are
// not supported. Cancellation drains a read; it never releases it early.
module chisburner_read_cdc131(
 input wire s_clk,s_reset_n,s_cancel,s_req,
 input wire [22:0] s_address,
 output wire s_ready,
 output reg s_busy,s_done,s_error,
 output reg [31:0] s_data,
 input wire m_clk,m_reset_n,
 output wire m_req,
 output wire [22:0] m_address,
 input wire m_ready,m_done,m_error,
 input wire [31:0] m_data
);
 reg request,ack,cancelled;
 reg [22:0] address_hold;
 reg [31:0] data_hold;
 reg error_hold;
 (* async_reg="true" *) reg [2:0] ack_sync,req_sync;
 (* async_reg="true" *) reg [22:0] address_s1,address_s2;
 (* async_reg="true" *) reg [31:0] data_s1,data_s2;
 (* async_reg="true" *) reg error_s1,error_s2;
 reg returning;
 reg [1:0] m_state;
 localparam [1:0] M_IDLE=0,M_REQUEST=1,M_WAIT=2,M_RETURN=3;
 assign s_ready=s_reset_n && !s_busy && !s_cancel;
 assign m_req=m_state==M_REQUEST;
 assign m_address=address_s2;
 always @(posedge s_clk or negedge s_reset_n)begin
  if(!s_reset_n)begin
   request<=0;address_hold<=0;s_busy<=0;s_done<=0;s_error<=0;s_data<=0;
   cancelled<=0;returning<=0;ack_sync<=0;data_s1<=0;data_s2<=0;error_s1<=0;error_s2<=0;
  end else begin
   ack_sync<={ack_sync[1:0],ack};
   data_s1<=data_hold;data_s2<=data_s1;
   error_s1<=error_hold;error_s2<=error_s1;
   s_done<=0;
   if(s_busy)begin
    if(s_cancel)cancelled<=1;
    if(!returning && ack_sync[2])begin
     request<=0;returning<=1;
     if(!cancelled && !s_cancel)begin s_data<=data_s2;s_error<=error_s2;s_done<=1;end
    end else if(returning && !ack_sync[2])begin
     s_busy<=0;returning<=0;cancelled<=0;
    end
   end else if(s_req && s_ready)begin
    address_hold<=s_address;request<=1;s_busy<=1;cancelled<=0;s_error<=0;
   end
  end
 end
 always @(posedge m_clk or negedge m_reset_n)begin
  if(!m_reset_n)begin
   req_sync<=0;address_s1<=0;address_s2<=0;ack<=0;
   data_hold<=0;error_hold<=0;m_state<=M_IDLE;
  end else begin
   req_sync<={req_sync[1:0],request};
   address_s1<=address_hold;address_s2<=address_s1;
   case(m_state)
    M_IDLE:if(req_sync[2])m_state<=M_REQUEST;
    M_REQUEST:if(m_ready)m_state<=M_WAIT;
    M_WAIT:if(m_done)begin data_hold<=m_data;error_hold<=m_error;ack<=1;m_state<=M_RETURN;end
    M_RETURN:if(!req_sync[2])begin ack<=0;m_state<=M_IDLE;end
   endcase
  end
 end
endmodule
`default_nettype wire
