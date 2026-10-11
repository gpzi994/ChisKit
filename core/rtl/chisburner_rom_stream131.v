// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Linear ROM payload/compare engine. All interfaces are in clk's domain.
// The NOR adapter must perform the selected chip recipe (not just write data)
// and acknowledge only AFTER programming/status polling has completed.
// This module does not authorize writes, switch mapper banks or own cart pins.
// The owner must drain/reset outstanding adapters before starting another job
// after cancellation or timeout; late responses must not cross job boundaries.
module chisburner_rom_stream131 #(
 parameter integer TIMEOUT_CYCLES = 90000000
)(
 input wire clk, reset_n, lifecycle_ok, write_allowed, abort,
 input wire start, compare_mode, unit16,
 input wire [25:0] base_bytes, length_bytes, image_bytes, nor_bytes,
 output reg busy, done,
 output reg [3:0] error,
 output reg [25:0] completed_bytes, fault_address,
 output reg [15:0] fault_expected, fault_actual,
 output wire mem_req,
 output wire [22:0] mem_word_address,
 input wire mem_ready, mem_done, mem_error,
 input wire [31:0] mem_data,
 output wire nor_req, nor_write,
 output wire [24:0] nor_address,
 output wire [15:0] nor_data,
 input wire nor_ready, nor_done, nor_error,
 input wire [15:0] nor_read_data
);
 localparam [2:0] IDLE=0, FETCH=1, MEM_REQ=2, MEM_WAIT=3,
                  NOR_REQ=4, NOR_WAIT=5;
 localparam [3:0] E_RANGE=1, E_ABORT=2, E_MEMORY=3, E_NOR=4,
                  E_TIMEOUT=5, E_COMPARE=6;
 reg [2:0] state;
 reg cmp, wide, cached;
 reg [25:0] address, ending, image_end;
 reg [22:0] cached_address;
 reg [31:0] cached_data, watchdog;
 reg [15:0] expected;
 wire permitted = lifecycle_ok && (cmp || write_allowed) && !abort;
 wire [26:0] requested_end = {1'b0,base_bytes}+{1'b0,length_bytes};
 wire [31:0] shifted = cached_data >> {address[1:0],3'b000};
 wire [15:0] actual = wide ? nor_read_data : {8'd0,nor_read_data[7:0]};
 assign mem_req = busy && permitted && state==MEM_REQ;
 assign mem_word_address = address[24:2];
 assign nor_req = busy && permitted && state==NOR_REQ;
 assign nor_write = !cmp;
 assign nor_address = address[24:0];
 assign nor_data = expected;

 task stop;
  input [3:0] reason;
  begin busy<=0; done<=1; error<=reason; state<=IDLE;
   fault_address<=address; cached<=0;
  end
 endtask

 always @(posedge clk or negedge reset_n) begin
  if(!reset_n) begin
   state<=IDLE;busy<=0;done<=0;error<=0;cmp<=1;wide<=0;
   cached<=0;cached_address<=0;cached_data<=0;watchdog<=0;
   address<=0;ending<=0;image_end<=0;expected<=0;
   completed_bytes<=0;fault_address<=0;fault_expected<=0;fault_actual<=0;
  end else if(busy) begin
   if(!permitted) stop(E_ABORT);
   else if(watchdog>=TIMEOUT_CYCLES-1) stop(E_TIMEOUT);
   else begin
    watchdog<=watchdog+1'b1;
    case(state)
     FETCH: begin
      if(address>=ending) begin busy<=0;done<=1;state<=IDLE;end
      else if(address>=image_end) begin
       expected<=wide?16'hffff:16'h00ff;state<=NOR_REQ;
      end else if(cached && cached_address==address[24:2]) begin
       expected<=wide?shifted[15:0]:{8'd0,shifted[7:0]};state<=NOR_REQ;
      end else state<=MEM_REQ;
     end
     MEM_REQ: if(mem_ready) begin state<=MEM_WAIT;watchdog<=0;end
     MEM_WAIT: if(mem_done) begin
      watchdog<=0;
      if(mem_error) stop(E_MEMORY);
      else begin cached<=1;cached_address<=address[24:2];
       cached_data<=mem_data;state<=FETCH;end
     end
     NOR_REQ: if(nor_ready) begin state<=NOR_WAIT;watchdog<=0;end
     NOR_WAIT: if(nor_done) begin
      watchdog<=0;
      if(nor_error) stop(E_NOR);
      else if(cmp && actual!=expected) begin
       fault_expected<=expected;fault_actual<=actual;stop(E_COMPARE);
      end else begin
       address<=address+(wide?26'd2:26'd1);
       completed_bytes<=completed_bytes+(wide?26'd2:26'd1);
       state<=FETCH;
      end
     end
     default: stop(E_ABORT);
    endcase
   end
  end else if(start) begin
   cmp<=compare_mode;wide<=unit16;address<=base_bytes;
   ending<=requested_end[25:0];image_end<=image_bytes;
   completed_bytes<=0;fault_address<=base_bytes;fault_expected<=0;fault_actual<=0;
   cached<=0;watchdog<=0;error<=0;done<=0;
   if(!lifecycle_ok || abort || (!compare_mode && !write_allowed)) begin
    done<=1;error<=E_ABORT;
   end else if(length_bytes==0 || image_bytes==0 || nor_bytes==0 ||
     nor_bytes>26'h2000000 || image_bytes>nor_bytes ||
     requested_end>{1'b0,nor_bytes} ||
     (!compare_mode && requested_end>{1'b0,image_bytes}) ||
     (unit16 && (base_bytes[0] || length_bytes[0] || image_bytes[0]))) begin
    done<=1;error<=E_RANGE;
   end else begin busy<=1;state<=FETCH;end
  end
 end
endmodule
`default_nettype wire
