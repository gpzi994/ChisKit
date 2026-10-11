// SPDX-License-Identifier: Apache-2.0
// Four-phase reservation handshake. Gate NEW jobs with bridge_accept;
// never gate data/responses belonging to already accepted jobs.
// Single common reset must be coordinated with host/AXI transaction owners.
`default_nettype none
module chisburner_sdram_interlock119 #(
 parameter integer TIMEOUT_CYCLES=1000000
)(
 input wire cpu_clk,bridge_clk,reset_n,
 input wire cpu_acquire,cpu_release,cpu_axi_idle,
 output wire cpu_idle,cpu_owned,cpu_fault,
 output reg cpu_rejected,cpu_timeout,
 input wire bridge_new_job,bridge_host_busy,bridge_read_pending,
 input wire bridge_writes_drained,bridge_error,
 output wire bridge_accept,bridge_blocked,bridge_owned,
 output reg bridge_rejected
);
 (* preserve, altera_attribute = "-name SYNCHRONIZER_IDENTIFICATION FORCED_IF_ASYNCHRONOUS" *) reg [1:0] crst,brst;
 always @(posedge cpu_clk or negedge reset_n)
  if(!reset_n) crst<=0;else crst<={crst[0],1'b1};
 always @(posedge bridge_clk or negedge reset_n)
  if(!reset_n) brst<=0;else brst<={brst[0],1'b1};
 localparam IDLE=3'd0,ACQUIRE=3'd1,OWNED=3'd2,RELEASE=3'd3,FAULT=3'd4;
 reg [2:0] state;
 reg request,cpu_fault_latched,ack,bridge_fault_latched;
 reg [31:0] timer;
 reg quiet_seen;
 (* preserve, altera_attribute = "-name SYNCHRONIZER_IDENTIFICATION FORCED_IF_ASYNCHRONOUS" *) reg req_b1,req_b2,cf_b1,cf_b2,online_b1,online_b2;
 (* preserve, altera_attribute = "-name SYNCHRONIZER_IDENTIFICATION FORCED_IF_ASYNCHRONOUS" *) reg ack_c1,ack_c2,bf_c1,bf_c2,online_c1,online_c2;
 wire bridge_quiet=!bridge_host_busy&&!bridge_read_pending&&bridge_writes_drained;
 assign cpu_fault=cpu_fault_latched|bf_c2;
 assign cpu_idle=crst[1]&&online_c2&&state==IDLE&&!cpu_fault;
 assign cpu_owned=crst[1]&&state==OWNED&&ack_c2&&!cpu_fault;
 assign bridge_blocked=!brst[1]||!online_b2||req_b2||cf_b2||bridge_fault_latched||bridge_error;
 assign bridge_accept=bridge_new_job&&!bridge_blocked;
 assign bridge_owned=brst[1]&&ack&&req_b2&&!bridge_fault_latched&&!cf_b2&&!bridge_error;
 always @(posedge cpu_clk or negedge reset_n) begin
  if(!reset_n) begin
   state<=IDLE;request<=0;cpu_fault_latched<=0;timer<=0;
   cpu_rejected<=0;cpu_timeout<=0;ack_c1<=0;ack_c2<=0;
   bf_c1<=0;bf_c2<=0;online_c1<=0;online_c2<=0;
  end else if(!crst[1]) begin
   state<=IDLE;request<=0;cpu_rejected<=0;
  end else begin
   ack_c1<=ack;ack_c2<=ack_c1;bf_c1<=bridge_fault_latched;bf_c2<=bf_c1;
   online_c1<=brst[1];online_c2<=online_c1;
   cpu_rejected<=0;
   if(bf_c2) begin state<=FAULT;request<=1;cpu_fault_latched<=1;end
   else case(state)
    IDLE: begin
     timer<=0;
     if(cpu_acquire&&!cpu_release&&cpu_idle) begin request<=1;state<=ACQUIRE;end
     else if(cpu_acquire||cpu_release) cpu_rejected<=1;
    end
    ACQUIRE: begin
     if(cpu_acquire||cpu_release) cpu_rejected<=1;
     // Bridge ack proves host and queued bridge writes drained. Local idle
     // additionally includes OFFERED as well as accepted AXI transactions.
     if(ack_c2&&cpu_axi_idle) begin state<=OWNED;timer<=0;end
     else if(timer==TIMEOUT_CYCLES-1) begin
      state<=FAULT;request<=1;cpu_fault_latched<=1;cpu_timeout<=1;
     end else timer<=timer+1;
    end
    OWNED: begin
     if(cpu_release&&!cpu_acquire&&cpu_axi_idle) begin request<=0;state<=RELEASE;timer<=0;end
     else if(cpu_acquire||cpu_release) cpu_rejected<=1;
    end
    RELEASE: begin
     if(cpu_acquire||cpu_release) cpu_rejected<=1;
     if(!ack_c2) begin state<=IDLE;timer<=0;end
     else if(timer==TIMEOUT_CYCLES-1) begin
      state<=FAULT;request<=1;cpu_fault_latched<=1;cpu_timeout<=1;
     end else timer<=timer+1;
    end
    default: begin
     request<=1;
     if(cpu_acquire||cpu_release) cpu_rejected<=1;
    end
   endcase
  end
 end
 always @(posedge bridge_clk or negedge reset_n) begin
  if(!reset_n) begin
   req_b1<=0;req_b2<=0;cf_b1<=0;cf_b2<=0;online_b1<=0;online_b2<=0;
   ack<=0;quiet_seen<=0;bridge_fault_latched<=0;bridge_rejected<=0;
  end else if(!brst[1]) begin ack<=0;quiet_seen<=0;bridge_rejected<=0;end
  else begin
   req_b1<=request;req_b2<=req_b1;cf_b1<=cpu_fault_latched;cf_b2<=cf_b1;
   online_b1<=crst[1];online_b2<=online_b1;
   bridge_rejected<=bridge_new_job&&bridge_blocked;
   // Activity after acknowledgment violates the reservation, so quarantine
   // both sides instead of silently treating a stale idle sample as safe.
   if(bridge_error||(ack&&req_b2&&!bridge_quiet)) bridge_fault_latched<=1;
   if(!req_b2) begin ack<=0;quiet_seen<=0;end
   else if(!bridge_fault_latched&&!cf_b2) begin
    if(bridge_quiet&&!bridge_accept) begin
     quiet_seen<=1;
     if(quiet_seen) ack<=1;
    end else quiet_seen<=0;
   end
  end
 end
endmodule
`default_nettype wire
