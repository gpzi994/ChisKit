// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Count -> PSRAM payload -> confirm phase of buffered programming.
// Supervisor MUST first select the mapper/window, authorize fresh NOR ID,
// enter buffer mode (and poll Intel buffer-ready on each die). After done it
// MUST poll chip completion and restore read mode; done is NOT program PASS.
// Commands are already wire-encoded from cb_nor_recipe. Only count is encoded
// here, exactly as cb_nor_count. Payload bytes are never bit-swapped.
// Both adapters share clk. Drain outstanding requests after any failure.
module chisburner_nor_buffer131(
 input wire clk,reset_n,start,cancel,lifecycle_ok,write_allowed,
 input wire unit16,dual_intel,swapped,raw_count,
 input wire [24:0] source_address,physical_address,
 input wire [16:0] length_bytes,buffer_bytes,
 input wire [15:0] confirm_command,
 input wire [31:0] timeout_cycles,
 output reg busy,done,
 output reg [3:0] error,
 output reg [16:0] sent_bytes,
 output reg [15:0] last_payload,
 output wire mem_req,
 output wire [22:0] mem_address,
 input wire mem_ready,mem_done,mem_error,
 input wire [31:0] mem_data,
 output wire tx_req,
 output wire [24:0] tx_address,
 output wire [15:0] tx_data,
 input wire tx_ready,tx_done,tx_error
);
 localparam [2:0] TX=0,TX_WAIT=1,FETCH=2,MEM=3,MEM_WAIT=4;
 localparam [1:0] COUNT=0,PAYLOAD=1,CONFIRM=2;
 reg [2:0] state;
 reg [1:0] phase;
 reg wide,dual,die,cached;
 reg [24:0] source,physical;
 reg [16:0] length;
 reg [15:0] count_value,confirm_value,payload;
 reg [22:0] cached_address;
 reg [31:0] cached_data,ticks,limit;
 wire allowed=lifecycle_ok && write_allowed && !cancel;
 wire [25:0] source_end={1'b0,source_address}+length_bytes;
 wire [25:0] physical_end={1'b0,physical_address}+length_bytes;
 wire [17:0] in_buffer={1'b0,physical_address[16:0] & (buffer_bytes-17'd1)}+length_bytes;
 wire [16:0] units=length_bytes >> (dual_intel?2:unit16?1:0);
 wire [15:0] raw_units=units-17'd1;
 wire [15:0] encoded_count=swapped && !raw_count?{raw_units[15:2],raw_units[0],raw_units[1]}:raw_units;
 wire [25:0] current_source={1'b0,source}+sent_bytes;
 wire [31:0] shifted=cached_data >> {current_source[1:0],3'b000};
 assign mem_req=busy && allowed && state==MEM;
 assign mem_address=current_source[24:2];
 assign tx_req=busy && allowed && state==TX;
 assign tx_address=phase==PAYLOAD?physical+sent_bytes:physical+(die?25'd2:25'd0);
 assign tx_data=phase==COUNT?count_value:phase==CONFIRM?confirm_value:payload;
 task stop;
  input [3:0] code;
  begin busy<=0;done<=1;error<=code;end
 endtask
 always @(posedge clk or negedge reset_n)begin
  if(!reset_n)begin
   busy<=0;done<=0;error<=0;sent_bytes<=0;last_payload<=0;state<=TX;phase<=COUNT;
   wide<=0;dual<=0;die<=0;cached<=0;source<=0;physical<=0;length<=0;
   count_value<=0;confirm_value<=0;payload<=0;cached_address<=0;cached_data<=0;ticks<=0;limit<=0;
  end else if(busy)begin
   ticks<=ticks+1'b1;
   if(!allowed)stop(4'd2);
   else if(ticks>=limit-1)stop(4'd5);
   else case(state)
    TX:if(tx_ready)state<=TX_WAIT;
    TX_WAIT:if(tx_done)begin
     if(tx_error)stop(4'd4);
     else if(phase==COUNT)begin
      if(dual && !die)begin die<=1;state<=TX;end
      else begin die<=0;phase<=PAYLOAD;state<=FETCH;end
     end else if(phase==PAYLOAD)begin
      sent_bytes<=sent_bytes+(wide?17'd2:17'd1);last_payload<=payload;
      if(sent_bytes+(wide?17'd2:17'd1)==length)begin phase<=CONFIRM;state<=TX;end
      else state<=FETCH;
     end else if(dual && !die)begin die<=1;state<=TX;end
     else stop(4'd0);
    end
    FETCH:begin
     if(cached && cached_address==current_source[24:2])begin
      payload<=wide?shifted[15:0]:{8'd0,shifted[7:0]};state<=TX;
     end else state<=MEM;
    end
    MEM:if(mem_ready)state<=MEM_WAIT;
    MEM_WAIT:if(mem_done)begin
     if(mem_error)stop(4'd3);
     else begin cached<=1;cached_address<=current_source[24:2];cached_data<=mem_data;state<=FETCH;end
    end
    default:stop(4'd1);
   endcase
  end else if(start)begin
   done<=0;error<=0;sent_bytes<=0;last_payload<=0;ticks<=0;limit<=timeout_cycles;
   source<=source_address;physical<=physical_address;length<=length_bytes;
   wide<=unit16;dual<=dual_intel;die<=0;cached<=0;phase<=COUNT;state<=TX;
   count_value<=encoded_count;confirm_value<=confirm_command;
   if(!allowed)begin done<=1;error<=2;end
   else if(!timeout_cycles || !length_bytes || !buffer_bytes ||
     (buffer_bytes & (buffer_bytes-17'd1))!=0 || length_bytes>buffer_bytes ||
     source_end>26'h2000000 || physical_end>26'h2000000 || in_buffer>{1'b0,buffer_bytes} ||
     (unit16 && (source_address[0] || physical_address[0] || length_bytes[0])) ||
     (dual_intel && (!unit16 || physical_address[1:0]!=0 || length_bytes[1:0]!=0)) ||
     (!unit16 && (physical_end>26'h8000 ||
       ({1'b0,physical_address[13:0]}+length_bytes)>17'h4000)))begin
    done<=1;error<=1;
   end else busy<=1;
  end
 end
endmodule
`default_nettype wire
