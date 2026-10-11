// SPDX-License-Identifier: Apache-2.0
// CPU-clock control plane; not yet connected to Pocket MMIO.
// selected must come from an exclusive top-level address decode. offset is
// the FULL byte offset, not truncated low address bits. One strobe per request.
// All status inputs must be synchronized/coherent in clk before connection.
// No software/local reset: timeout locks new work but preserves AXI draining.
`default_nettype none
module chisburner_sdram_control116 #(
 parameter integer TIMEOUT_CYCLES=1000000
)(
 input wire clk, reset_n,
 input wire selected, reg_write,
 input wire [31:0] reg_offset, reg_wdata,
 input wire [3:0] reg_wstrb,
 output reg [31:0] reg_rdata,
 input wire transport_ready, transport_complete, transport_fault,
 input wire transport_rejected, ingress_drained, ingress_fault,
 input wire host_quiet, axi_idle,
 output reg prepare, release_snapshot,
 output reg [31:0] source_address, byte_count,
 output wire locked, idle
);
 localparam IDLE=3'd0, PREPARING=3'd1, SNAPSHOT=3'd2,
  RELEASING=3'd3, LOCKED=3'd4;
 reg [2:0] state;
 reg rejected, timeout_fault;
 reg [31:0] timer;
 wire fault_now=transport_fault||ingress_fault||transport_rejected;
 wire quiet=host_quiet&&axi_idle&&ingress_drained;
 wire [32:0] end_address={1'b0,source_address}+{1'b0,byte_count};
 wire valid_range=source_address[31:26]==6'b000100&&
  source_address[1:0]==0&&byte_count!=0&&byte_count<=32768&&
  end_address<=33'h014000000;
 assign locked=state==LOCKED||fault_now;
 assign idle=state==IDLE&&!locked;
 always @* begin
  reg_rdata=0;
  if(selected) case(reg_offset)
   32'h00: reg_rdata=32'h53443116; // feature signature, not firmware version
   32'h04: reg_rdata={23'd0,axi_idle,host_quiet,ingress_drained,
     timeout_fault,rejected,locked,state==SNAPSHOT,idle,transport_ready};
   32'h08: reg_rdata=source_address;
   32'h0c: reg_rdata=byte_count;
   default: reg_rdata=0;
  endcase
 end
 always @(posedge clk or negedge reset_n) begin
  if(!reset_n) begin
   state<=IDLE;rejected<=0;timeout_fault<=0;timer<=0;
   prepare<=0;release_snapshot<=0;source_address<=0;byte_count<=0;
  end else begin
   prepare<=0;release_snapshot<=0;
   if(fault_now) state<=LOCKED;
   else case(state)
    PREPARING: if(transport_complete) begin state<=SNAPSHOT;timer<=0;end
     else if(timer==TIMEOUT_CYCLES-1) begin
      state<=LOCKED;timeout_fault<=1;
     end else timer<=timer+1;
    RELEASING: if(transport_ready) begin state<=IDLE;timer<=0;end
     else if(timer==TIMEOUT_CYCLES-1) begin
      state<=LOCKED;timeout_fault<=1;
     end else timer<=timer+1;
    default: timer<=0;
   endcase
   if(selected&&reg_write) begin
    // Full-word commands only; no address aliases or partial command writes.
    if(reg_wstrb!=4'hf||locked) rejected<=1;
    else case(reg_offset)
     32'h08: if(idle) source_address<=reg_wdata;else rejected<=1;
     32'h0c: if(idle) byte_count<=reg_wdata;else rejected<=1;
     32'h10: case(reg_wdata)
      1: if(idle&&transport_ready&&quiet&&valid_range) begin
       prepare<=1;state<=PREPARING;timer<=0;
      end else rejected<=1;
      2: if(state==SNAPSHOT&&transport_complete&&quiet) begin
       release_snapshot<=1;state<=RELEASING;timer<=0;
      end else rejected<=1;
      4: if(idle) rejected<=0;else rejected<=1;
      // Includes reset requests: never reset an in-flight transport locally.
      default: rejected<=1;
     endcase
     default: rejected<=1;
    endcase
   end
  end
 end
endmodule
`default_nettype wire
