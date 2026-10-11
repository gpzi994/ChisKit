// SPDX-License-Identifier: Apache-2.0
// SDRAM -> bounded dual-clock BRAM snapshot -> APF-like read port.
// Not integrated with Pocket. PREPARE, wait cmd_complete, issue host write,
// wait HOST completion, RELEASE, wait cmd_ready. No PSRAM. No NOR commands.
// Common reset assertion; synchronized deassertion required in both domains.
`default_nettype none
module chisburner_sdram_egress114 #(parameter APF_TAIL_PREFETCH = 0) (
 input wire cpu_clk,cpu_reset_n,bridge_clk,bridge_reset_n,
 input wire cmd_prepare,cmd_release,
 input wire [31:0] cmd_address,cmd_bytes,
 output wire cmd_ready,cmd_complete,cmd_fault,
 output reg cmd_rejected,
 output wire m_arvalid,
 input wire m_arready,
 output wire [31:0] m_araddr,
 output wire [7:0] m_arlen,
 input wire m_rvalid,
 output wire m_rready,
 input wire [31:0] m_rdata,
 input wire [1:0] m_rresp,
 input wire m_rlast,
 input wire bridge_read,
 input wire [31:0] bridge_offset,
 output wire [31:0] bridge_data,
 output reg bridge_valid,
 output reg bridge_ready,
 output reg bridge_fault
);
 localparam IDLE=3'd0,ADDR=3'd1,DATA=3'd2,PUBLISH=3'd3,
  READY=3'd4,RELEASE=3'd5,FAILED=3'd6,DRAIN=3'd7;
 reg [2:0] state;
 (* ramstyle="M10K" *) reg [31:0] buffer [0:8191];
 reg [31:0] bridge_data_q;
 assign bridge_data=bridge_valid&&!bridge_zero?bridge_data_q:32'd0;
 reg [31:0] address,byte_count,word_count,index;
 reg fault,publish_toggle,publish_valid;
 (* preserve, altera_attribute = "-name SYNCHRONIZER_IDENTIFICATION FORCED_IF_ASYNCHRONOUS" *) reg pub_b1,pub_b2,ack_c1,ack_c2,bfault_c1,bfault_c2;
 reg bridge_ack;
 reg [31:0] bridge_count;
 reg tail_armed,tail_used,bridge_zero;
 wire [31:0] rounded_count=(bridge_count+32'd3)&32'hfffffffc;
 wire tail_read=APF_TAIL_PREFETCH!=0 && tail_armed && !tail_used &&
  bridge_offset==rounded_count;
 wire [32:0] end_address={1'b0,cmd_address}+{1'b0,cmd_bytes};
 wire valid_command=cmd_address[31:26]==6'b000100&&cmd_address[1:0]==0&&
  cmd_bytes!=0&&cmd_bytes<=32768&&end_address<=33'h014000000;
 assign cmd_ready=cpu_reset_n&&state==IDLE&&!cmd_fault;
 assign cmd_complete=cpu_reset_n&&state==READY&&!cmd_fault;
 assign cmd_fault=fault|bfault_c2;
 assign m_arvalid=cpu_reset_n&&state==ADDR;
 assign m_araddr=address;
 assign m_arlen=0;
 assign m_rready=cpu_reset_n&&(state==DATA||state==DRAIN);
 // Dedicated synchronous RAM ports: reset only the publication/valid bits,
 // not the memory output register, so Quartus can infer dual-clock M10K.
 always @(posedge bridge_clk)
  if(bridge_reset_n&&bridge_read&&bridge_offset<bridge_count&&bridge_offset<32768)
   bridge_data_q<=buffer[bridge_offset[14:2]];
 always @(posedge cpu_clk)
  if(cpu_reset_n&&state==DATA&&m_rvalid&&m_rresp==0&&m_rlast) begin
   if(index==word_count-1&&byte_count[1:0]!=0)
    case(byte_count[1:0])
     1: buffer[index[12:0]]<={24'd0,m_rdata[7:0]};
     2: buffer[index[12:0]]<={16'd0,m_rdata[15:0]};
     default: buffer[index[12:0]]<={8'd0,m_rdata[23:0]};
    endcase
   else buffer[index[12:0]]<=m_rdata;
  end
 always @(posedge cpu_clk or negedge cpu_reset_n) begin
  if(!cpu_reset_n) begin
   state<=IDLE;fault<=0;address<=0;byte_count<=0;word_count<=0;index<=0;
   publish_toggle<=0;publish_valid<=0;ack_c1<=0;ack_c2<=0;
   bfault_c1<=0;bfault_c2<=0;cmd_rejected<=0;
  end else begin
   ack_c1<=bridge_ack;ack_c2<=ack_c1;
   bfault_c1<=bridge_fault;bfault_c2<=bfault_c1;
   cmd_rejected<=(cmd_prepare&&!cmd_ready)||(cmd_release&&state!=READY)||
    (cmd_prepare&&cmd_release);
   case(state)
    IDLE: if(cmd_prepare&&!cmd_release&&!cmd_fault) begin
     if(!valid_command) begin fault<=1;state<=FAILED;end
     else begin
      address<=cmd_address;byte_count<=cmd_bytes;
      word_count<=(cmd_bytes+3)>>2;index<=0;state<=ADDR;
     end
    end
    ADDR: if(m_arready) state<=DATA;
    DATA: if(m_rvalid) begin
     if(m_rresp!=0||!m_rlast) begin
      fault<=1;state<=m_rlast?FAILED:DRAIN;
     end else begin
      if(index==word_count-1) begin
       publish_valid<=1;publish_toggle<=~publish_toggle;state<=PUBLISH;
      end else begin index<=index+1;address<=address+4;state<=ADDR;end
     end
    end
    DRAIN: if(m_rvalid&&m_rlast) state<=FAILED;
    PUBLISH: if(ack_c2==publish_toggle) state<=READY;
    READY: if(cmd_release) begin
     publish_valid<=0;publish_toggle<=~publish_toggle;state<=RELEASE;
    end
    RELEASE: if(ack_c2==publish_toggle) state<=IDLE;
    default: state<=FAILED;
   endcase
  end
 end
 // Data RAM is not read until publication crosses two flops. Metadata stays
 // fixed from PREPARE until RELEASE ack. Integration needs bundled-data CDC
 // constraints as well as synchronizer placement; simulation is not proof.
 always @(posedge bridge_clk or negedge bridge_reset_n) begin
  if(!bridge_reset_n) begin
   pub_b1<=0;pub_b2<=0;bridge_ack<=0;bridge_ready<=0;bridge_count<=0;
   bridge_fault<=0;bridge_valid<=0;tail_armed<=0;tail_used<=0;bridge_zero<=0;
  end else begin
   pub_b1<=publish_toggle;pub_b2<=pub_b1;
   bridge_valid<=0;
   if(pub_b2!=bridge_ack) begin
    bridge_ready<=publish_valid;bridge_count<=byte_count;bridge_ack<=pub_b2;
    tail_armed<=0;tail_used<=0;
   end
   if(bridge_read) begin
    if(!bridge_ready||bridge_fault||bridge_offset[1:0]!=0||
       ((bridge_offset>=bridge_count||bridge_offset>=32768)&&!tail_read)) begin
     bridge_fault<=1;
    end else begin
     bridge_valid<=1;
     bridge_zero<=tail_read;
     // APF ST_READ_0 transmits the previous word before ST_READ_1
     // requests this address. Permit only one bounded flush prefetch;
     // never wrap its address into BRAM or expose unrelated SDRAM.
     if(tail_read) tail_used<=1;
     tail_armed<=!tail_read && bridge_offset+32'd4==rounded_count;
    end
   end
  end
 end
endmodule
`default_nettype wire
