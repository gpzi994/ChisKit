// EEPROM serial block sequencer. Wrapper must drive word address FFFF80
// before CS falls, retain A16..23, and route serial data to GBA AD0.
// Write emits opcode 10, address, 64 data bits MSB first, then a zero stop.
// Outputs are gated immediately on owner loss.
`default_nettype none
module chisburner_eeprom139 #(
 parameter integer HALF_CYCLES=18,
 parameter integer TIMEOUT_CYCLES=20000,
 // One-based CPU edge in RX_HIGH. Default preserves the existing timing.
 parameter integer RX_CAPTURE_CYCLES=HALF_CYCLES
)(input wire clk,reset_n,owner_ok,start,cancel,
 input wire mode_large,write_mode,input wire [13:0] block_address,
 input wire [63:0] write_data,input wire data_in,
 output wire busy,output reg done,error,output reg [63:0] data,
 output wire cs_n,rd_n,wr_n,data_oe,data_out,cart_clk,
 output wire address_hold);
localparam [3:0] IDLE=0,CLK_H=1,CLK_L=2,TX_SETUP=3,TX_LOW=4,TX_HIGH=5,
 TURN=6,RX_LOW=7,RX_HIGH=8,END_H=9,END_L=10,ADDR_SETUP=11;
localparam integer T_BITS=HALF_CYCLES<=1?1:$clog2(HALF_CYCLES);
localparam integer W_BITS=TIMEOUT_CYCLES<=1?1:$clog2(TIMEOUT_CYCLES);
reg [3:0] state;reg [T_BITS-1:0] ticks;reg [W_BITS-1:0] watchdog;
reg [16:0] command;reg [4:0] tx_left;reg [6:0] rx_count;
reg [63:0] payload;reg [6:0] write_left;reg [1:0] tx_phase;
wire gate=reset_n&&owner_ok&&!cancel;
assign busy=state!=IDLE;
// Drive the full FFFF80 word address during setup and the initial CS/CLK
// sequence. Only switch AD0 to serial data once TX_SETUP is reached.
assign address_hold=gate&&(state==ADDR_SETUP||state==CLK_H||state==CLK_L);
wire tx=state==CLK_H||state==CLK_L||state==TX_SETUP||state==TX_LOW||state==TX_HIGH;
assign cs_n=!(gate&&(tx||state==RX_LOW||state==RX_HIGH));
assign wr_n=!(gate&&state==TX_LOW);
assign rd_n=!(gate&&state==RX_LOW);
assign data_oe=gate&&tx;
assign data_out=tx_phase==1?payload[63]:tx_phase==2?1'b0:command[16];
assign cart_clk=gate&&(state==CLK_H||state==END_H);
always @(posedge clk or negedge reset_n)begin
 if(!reset_n)begin state<=IDLE;ticks<=0;watchdog<=0;done<=0;error<=0;data<=0;command<=0;tx_left<=0;rx_count<=0;payload<=0;write_left<=0;tx_phase<=0;end
 else if(!owner_ok||cancel)begin state<=IDLE;ticks<=0;watchdog<=0;done<=0;error<=busy;data<=0;end
 else if(state==IDLE)begin
  if(start)begin
   done<=0;error<=0;data<=0;ticks<=0;watchdog<=0;rx_count<=0;tx_phase<=0;
   if(HALF_CYCLES<1||TIMEOUT_CYCLES<1||RX_CAPTURE_CYCLES<1||RX_CAPTURE_CYCLES>HALF_CYCLES||(!mode_large&&block_address>=64)||(mode_large&&block_address>=1024))error<=1;
   else begin
    command<=mode_large?{write_mode?2'b10:2'b11,block_address,1'b0}:
      {write_mode?2'b10:2'b11,block_address[5:0],1'b0,8'b0};
    tx_left<=mode_large?(write_mode?16:17):(write_mode?8:9);
    payload<=write_data;write_left<=64;state<=ADDR_SETUP;
   end
  end
 end else if(watchdog>=TIMEOUT_CYCLES-1)begin
  state<=IDLE;error<=1;done<=0;data<=0;
 end else begin
  watchdog<=watchdog+1'b1;
  // Capture once, independently of the unchanged bus-phase transition.
  if(state==RX_HIGH&&ticks==RX_CAPTURE_CYCLES-1&&rx_count>=4)
   data<={data[62:0],data_in};
  if(ticks<HALF_CYCLES-1)ticks<=ticks+1'b1;
  else begin
   ticks<=0;
   case(state)
    ADDR_SETUP:state<=CLK_H;
    CLK_H:state<=CLK_L;
    CLK_L:state<=TX_SETUP;
    TX_SETUP:state<=TX_LOW;
    TX_LOW:state<=TX_HIGH;
    TX_HIGH:begin
     if(tx_phase==0)begin
      if(tx_left==1)begin
       if(write_mode)begin tx_phase<=1;state<=TX_SETUP;end
       else state<=TURN;
      end else begin command<={command[15:0],1'b0};tx_left<=tx_left-1'b1;state<=TX_SETUP;end
     end else if(tx_phase==1)begin
      if(write_left==1)begin tx_phase<=2;state<=TX_SETUP;end
      else begin payload<={payload[62:0],1'b0};write_left<=write_left-1'b1;state<=TX_SETUP;end
     end else state<=END_H;
    end
    TURN:state<=RX_LOW;
    RX_LOW:state<=RX_HIGH;
    RX_HIGH:begin
     if(rx_count==67)state<=END_H;
     else begin rx_count<=rx_count+1'b1;state<=RX_LOW;end
    end
    END_H:state<=END_L;
    END_L:begin state<=IDLE;done<=1;end
    default:begin state<=IDLE;done<=0;error<=1;end
   endcase
  end
 end
end
endmodule
`default_nettype wire
