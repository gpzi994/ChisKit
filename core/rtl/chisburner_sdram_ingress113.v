// SPDX-License-Identifier: Apache-2.0
// Experimental APF-like word stream -> AXI SDRAM write ingress.
// Not wired to Pocket yet. No PSRAM dependency. Two-clock FIFO; one AXI
// transaction at a time. DONE means B-response completed, not FIFO popped.
// Upstream without backpressure MUST check fault after each transfer.
// Common asynchronous reset is required; deassert synchronously per domain.
`default_nettype none
module chisburner_sdram_ingress113 #(
 parameter FIFO_BITS=10
)(
 input wire src_clk, src_reset_n, dst_clk, dst_reset_n,
 input wire src_valid,
 input wire [31:0] src_address, src_data,
 output wire src_ready, src_drained, src_fault,
 output wire m_awvalid,
 input wire m_awready,
 output wire [31:0] m_awaddr,
 output wire [7:0] m_awlen,
 output wire m_wvalid,
 input wire m_wready,
 output wire [31:0] m_wdata,
 output wire [3:0] m_wstrb,
 output wire m_wlast,
 input wire m_bvalid,
 input wire [1:0] m_bresp,
 output wire m_bready
);
 localparam PTR=FIFO_BITS+1;
 localparam IDLE=2'd0, SEND=2'd1, RESPONSE=2'd2, HALT=2'd3;
 (* ramstyle="M10K" *) reg [63:0] fifo [0:(1<<FIFO_BITS)-1];
 reg [PTR-1:0] wr_bin,wr_gray,rd_bin,rd_gray,done_bin,done_gray;
 (* preserve, altera_attribute = "-name SYNCHRONIZER_IDENTIFICATION FORCED_IF_ASYNCHRONOUS" *) reg [PTR-1:0] rd_s1,rd_s2,done_s1,done_s2,wr_d1,wr_d2;
 (* preserve, altera_attribute = "-name SYNCHRONIZER_IDENTIFICATION FORCED_IF_ASYNCHRONOUS" *) reg err_s1,err_s2,src_err_d1,src_err_d2;
 reg source_error,dest_error;
 reg [1:0] state;
 reg [63:0] payload;
 reg aw_sent,w_sent;
 wire [PTR-1:0] wr_next=wr_bin+{{(PTR-1){1'b0}},1'b1};
 wire [PTR-1:0] rd_next=rd_bin+{{(PTR-1){1'b0}},1'b1};
 wire [PTR-1:0] done_next=done_bin+{{(PTR-1){1'b0}},1'b1};
 // Gray full comparator inverts the two most significant bits.
 wire full=wr_gray=={~rd_s2[PTR-1:PTR-2],rd_s2[PTR-3:0]};
 wire empty=rd_gray==wr_d2;
 wire address_ok=src_address[31:26]==6'b000100 && src_address[1:0]==2'b00;
 assign src_fault=source_error|err_s2;
 assign src_ready=src_reset_n&&!full&&!src_fault;
 assign src_drained=src_reset_n&&(wr_gray==done_s2);
 assign m_awvalid=dst_reset_n&&state==SEND&&!aw_sent;
 assign m_awaddr=payload[63:32];
 assign m_awlen=8'd0;
 assign m_wvalid=dst_reset_n&&state==SEND&&!w_sent;
 assign m_wdata=payload[31:0];
 assign m_wstrb=4'b1111;
 assign m_wlast=1'b1;
 assign m_bready=dst_reset_n&&state==RESPONSE;
 // RAM ports have no asynchronous reset. Validity lives in resettable
 // pointers/state; stale RAM contents are never consumed after reset.
 always @(posedge src_clk)
  if(src_reset_n&&src_valid&&src_ready&&address_ok)
   fifo[wr_bin[FIFO_BITS-1:0]]<={src_address,src_data};
 always @(posedge dst_clk)
  if(dst_reset_n&&state==IDLE&&!src_err_d2&&!empty)
   payload<=fifo[rd_bin[FIFO_BITS-1:0]];
 always @(posedge src_clk or negedge src_reset_n) begin
  if(!src_reset_n) begin
   wr_bin<=0;wr_gray<=0;rd_s1<=0;rd_s2<=0;done_s1<=0;done_s2<=0;
   err_s1<=0;err_s2<=0;source_error<=0;
  end else begin
   rd_s1<=rd_gray;rd_s2<=rd_s1;done_s1<=done_gray;done_s2<=done_s1;
   err_s1<=dest_error;err_s2<=err_s1;
   if(src_valid) begin
    if(!src_ready||!address_ok) source_error<=1;
    else begin
     wr_bin<=wr_next;wr_gray<=(wr_next>>1)^wr_next;
    end
   end
  end
 end
 always @(posedge dst_clk or negedge dst_reset_n) begin
  if(!dst_reset_n) begin
   rd_bin<=0;rd_gray<=0;done_bin<=0;done_gray<=0;wr_d1<=0;wr_d2<=0;
   src_err_d1<=0;src_err_d2<=0;dest_error<=0;state<=IDLE;
   aw_sent<=0;w_sent<=0;
  end else begin
   wr_d1<=wr_gray;wr_d2<=wr_d1;
   src_err_d1<=source_error;src_err_d2<=src_err_d1;
   case(state)
    IDLE: if(src_err_d2) state<=HALT;
     else if(!empty) begin
      // Registered dual-port RAM read. Pointer advances only when captured;
      // the payload is thereafter stable until both channels handshake.
      rd_bin<=rd_next;rd_gray<=(rd_next>>1)^rd_next;
      aw_sent<=0;w_sent<=0;state<=SEND;
     end
    SEND: begin
     if(m_awvalid&&m_awready) aw_sent<=1;
     if(m_wvalid&&m_wready) w_sent<=1;
     if((aw_sent||(m_awvalid&&m_awready))&&(w_sent||(m_wvalid&&m_wready))) state<=RESPONSE;
    end
    RESPONSE: if(m_bvalid) begin
     done_bin<=done_next;done_gray<=(done_next>>1)^done_next;
     if(m_bresp!=0) begin dest_error<=1;state<=HALT;end
     else state<=src_err_d2?HALT:IDLE;
    end
    default: state<=HALT;
   endcase
  end
 end
endmodule
`default_nettype wire
