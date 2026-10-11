// SPDX-License-Identifier: Apache-2.0
// OS-private physical transaction engine. Never expose as an application API.
// All addresses are BYTE addresses. One GB byte or GBA halfword per command.
// OS owns ID/CFI algorithms and per-device authorization; FPGA owns bus timing.
// A one-shot arm must precede each write, including identification commands.
`default_nettype none
module chisburner_nor_bus108 #(
 parameter integer ADDRESS_CYCLES=18,
 parameter integer SELECT_CYCLES=9,
 parameter integer DATA_SETUP_CYCLES=18,
 parameter integer STROBE_CYCLES=36,
 parameter integer RELEASE_CYCLES=18
)(
 input wire clk, reset_n, lifecycle_ok, selected,
 input wire reg_wr, reg_rd,
 input wire [4:0] reg_addr, reg_wr_addr,
 input wire [31:0] reg_wdata,
 output reg [31:0] reg_rdata,
 input wire [7:0] gb_data_in,bank3_in,bank2_in,
 output reg [7:0] bank3_out,bank2_out,bank1_out,
 output reg bank3_dir,bank2_dir,bank1_dir,
 output reg [3:0] bank0_out,
 output wire bank0_dir,
 output wire pin30_out,pin30_dir,pin30_pwroff_reset,pin31_out,pin31_dir,
 output wire busy,write_active
);
localparam [3:0] IDLE=0,ADDRESS=1,SELECT=2,DATA=3,STROBE=4,RELEASE=5,RTC_LOW191=6,RTC_HIGH191=7;
// Private clock-only port: exactly 50 pulses, >=500 ns per half at <=128 MHz.
// No internal logic is clocked by this output; it remains in the existing clk domain.
`ifdef CHISBURNER_GB_RTC191
wire rtc_clock191=bus_reg==12;
localparam [31:0] RTC_CAP191=32'h2000;
localparam integer RTC_MIN191=64;
`else
wire rtc_clock191=1'b0;
localparam [31:0] RTC_CAP191=0;
localparam integer RTC_MIN191=1;
`endif
reg [5:0] rtc_left191;
localparam [31:0] KEY1=32'h43424e31,KEY2=32'h43424e32;
localparam TIMING_OK=ADDRESS_CYCLES>=1 && SELECT_CYCLES>=1 &&
 DATA_SETUP_CYCLES>=1 && STROBE_CYCLES>=1 && RELEASE_CYCLES>=1;
reg [3:0] state;
reg [31:0] bus_reg,address_reg,timeout_reg,status_reg,error_reg;
reg [15:0] data_reg,rx_reg;
reg [1:0] arm;
// 135: opt-in width-only experiment; stage thresholds and watchdog stay unchanged.
`ifdef CHISBURNER_TIMERS135
function integer timer_max135;
 input integer a,b;
 begin timer_max135=(a>b)?a:b; end
endfunction
localparam integer TIMER_MAX135=timer_max135(timer_max135(timer_max135(timer_max135(timer_max135(RTC_MIN191,ADDRESS_CYCLES),SELECT_CYCLES),DATA_SETUP_CYCLES),STROBE_CYCLES),RELEASE_CYCLES);
// Counter holds 0..MAX-1. Keep at least one bit for zero/one-cycle parameters.
localparam integer TIMER_BITS135=(TIMER_MAX135<=1)?1:$clog2(TIMER_MAX135);
reg [TIMER_BITS135-1:0] ticks;
`else
reg [31:0] ticks;
`endif
reg [31:0] watchdog;
reg is_write,is_gba,is_bank_control,is_gb_ram;
reg [24:0] active_addr;
reg [15:0] active_data;
reg rx_valid;
wire gate=reset_n && lifecycle_ok && selected;
`ifdef CHISBURNER_GB_RESTORE185
wire mbc7_write185=bus_reg==11;
localparam [31:0] MBC7_WRITE_CAP185=32'h1000;
reg [4:0] ee_count185;
reg [10:0] ee_command185;
reg [1:0] ee_opcode185;
reg ee_clock185,ee_enable185;
wire ee_edge185=data_reg[6]&&!ee_clock185;
wire ee_prefix185=!ee_edge185 ||
 (ee_count185==0 ? data_reg[1] :
  ee_count185==1 ? 1'b1 :
  ee_count185==2 ? ((!ee_command185[0]&&!data_reg[1]) ||
                    (ee_command185[0]&&!data_reg[1]) ||
                    (!ee_command185[0]&&data_reg[1]&&ee_enable185)) :
  ee_count185==3 ? (ee_opcode185!=1 || !data_reg[1]) :
  ee_count185==4 ? (ee_opcode185!=0 || data_reg[1]==ee_command185[0]) :
  ee_count185<11 ? (ee_opcode185!=0 || !data_reg[1]) :
  ee_count185<27 ? (ee_opcode185==1 || (ee_opcode185==2&&!data_reg[1])) : 1'b0);
wire ee_pins185=data_reg==0 ||
 ((data_reg==16'h80||data_reg==16'h82||data_reg==16'hc0||data_reg==16'hc2)&&ee_prefix185);
`else
wire mbc7_write185=1'b0;
localparam [31:0] MBC7_WRITE_CAP185=0;
wire ee_pins185=1'b0;
`endif
`ifdef CHISBURNER_GB_RESTORE180
wire gb_write180=bus_reg==10;
localparam [31:0] GB_WRITE_CAP180=32'h800;
`else
wire gb_write180=1'b0;
localparam [31:0] GB_WRITE_CAP180=0;
`endif
`ifdef CHISBURNER_GB_SAVE179
wire mbc7_requested=bus_reg==9;
localparam [31:0] MBC7_CAP179=32'h400;
reg [3:0] ee_bits179;
reg ee_clock179;
// An OS-private serial port with a READ-only prefix guard. Allow start+10,
// eight address bits, then only zero DI for data clocks. No EWEN/erase/write.
wire ee_edge179=data_reg[6] && !ee_clock179;
wire ee_prefix179=!ee_edge179 ||
 (ee_bits179<2 ? data_reg[1] : ee_bits179==2 ? !data_reg[1] :
  ee_bits179<11 ? 1'b1 : !data_reg[1]);
wire ee_pins179=data_reg==0 ||
 ((data_reg==16'h80 || data_reg==16'h82 || data_reg==16'hc0 || data_reg==16'hc2) && ee_prefix179);
`else
wire mbc7_requested=1'b0;
localparam [31:0] MBC7_CAP179=0;
wire ee_pins179=1'b0;
`endif
`ifdef CHISBURNER_GB_SAVE175
wire gb_save_requested=bus_reg==8;
`ifdef CHISBURNER_GB_SAVE177
localparam [31:0] GB_SAVE_CAP175=32'h300;
wire gb_mode_ok=address_reg==32'h6000 && data_reg<=1;
`else
localparam [31:0] GB_SAVE_CAP175=32'h100;
wire gb_mode_ok=1'b0;
`endif
`else
wire gb_save_requested=1'b0;
localparam [31:0] GB_SAVE_CAP175=0;
wire gb_mode_ok=1'b0;
`endif
assign busy=state!=IDLE;
assign write_active=gate && busy && is_write;
`ifdef CHISBURNER_SRAM138
wire save_bank_requested=bus_reg==5;
localparam [31:0] SAVE_LIMIT=32'h00010000;
`else
wire save_bank_requested=1'b0;
localparam [31:0] SAVE_LIMIT=32'h00008000;
`endif
wire gba_requested=bus_reg==2 || save_bank_requested;
// 139 opt-in OS-private save probe port. Raw values are required to restore
// saved bytes; this is an address/width guard, NOT a Flash command firewall.
// OS must own the full probe/cleanup sequence and never expose this to apps.
`ifdef CHISBURNER_SAVE139
wire save_probe_requested=bus_reg==6;
`else
wire save_probe_requested=1'b0;
`endif
// OS-private SRAM data writes. This electrical guard cannot identify SRAM;
// the OS must freshly classify the cartridge before enabling this build.
`ifdef CHISBURNER_SAVE158
wire save_data_requested=bus_reg==7;
`else
wire save_data_requested=1'b0;
`endif
// GBA SRAM-style writes are exclusively for verified board bank registers.
// This is not a save-memory API. Reads and every other address are rejected.
wire bank_requested=bus_reg==3;
// SRAM137: a separate READ-ONLY 32 KiB aperture. Never reuse bus 3's
// board-register write permission for cartridge saves.
`ifdef CHISBURNER_SRAM137
wire save_requested=bus_reg==4;
`else
wire save_requested=1'b0;
`endif
// Dedicated GB-only aperture: header/RAM reads, RAM enable and bank controls.
// Never permits save-data writes, RTC latch writes, or old ROM/NOR commands.
// OS narrows bank values to the freshly checked mapper's legal RAM banks.
wire range_ok=mbc7_write185 ?
 ((reg_wdata==2&&((address_reg>=32'h100&&address_reg<32'h150)||address_reg==32'ha080)) ||
 (reg_wdata==7&&((address_reg==0&&(data_reg==0||data_reg==10)) ||
 (address_reg==32'h4000&&(data_reg==0||data_reg==64)) || (address_reg==32'ha080&&ee_pins185)))) : gb_write180 ?
 ((reg_wdata==2 && ((address_reg>=32'h100&&address_reg<32'h150)||
 (address_reg>=32'ha000&&address_reg<32'hc000))) ||
 (reg_wdata==7 && ((address_reg>=32'ha000&&address_reg<32'hc000&&data_reg[15:8]==0)||
 (address_reg==0&&(data_reg==0||data_reg==10)) || (address_reg==32'h4000&&data_reg<16) ||
 (address_reg==32'h6000&&data_reg<=1)))) : mbc7_requested ?
 ((reg_wdata==2 && ((address_reg>=32'h100 && address_reg<32'h150)||address_reg==32'ha080)) ||
 (reg_wdata==7 && ((address_reg==0&&(data_reg==0||data_reg==10)) ||
 (address_reg==32'h4000&&(data_reg==0||data_reg==64)) || (address_reg==32'ha080&&ee_pins179)))) :
 gb_save_requested ?
 ((reg_wdata==2 && ((address_reg>=32'h100 && address_reg<32'h150) ||
 (address_reg>=32'ha000 && address_reg<32'hc000))) ||
 (reg_wdata==7 && ((address_reg==0 && (data_reg==0 || data_reg==10)) ||
 (address_reg==32'h4000 && data_reg<16) || gb_mode_ok))) :
 save_data_requested ? reg_wdata==7 && address_reg<SAVE_LIMIT &&
 data_reg[15:8]==0 :
 save_probe_requested ? reg_wdata==7 && data_reg[15:8]==0 &&
 (address_reg==0 || address_reg==4 || address_reg==32'h5555 || address_reg==32'h2aaa) :
 save_bank_requested ? reg_wdata==7 && address_reg==32'h01000000 && data_reg<=1 :
 gba_requested ? address_reg<32'h02000000 && !address_reg[0] :
 save_requested ? reg_wdata==2 && address_reg<SAVE_LIMIT :
 bank_requested ? reg_wdata==7 && (address_reg==2 || address_reg==3 ||
 address_reg==4 || address_reg==6) && data_reg[15:8]==0 :
 bus_reg==1 && address_reg<32'h00008000;
wire control=reg_wr && reg_wr_addr==3 &&
 (reg_wdata==1 || reg_wdata==3 || reg_wdata==6);
wire [23:0] word_address=active_addr[24:1];
assign bank0_dir=1'b1;
assign pin30_out=!(gate && busy && is_bank_control && state!=ADDRESS);
assign pin30_dir=gate && busy && (is_gba || is_bank_control);
// Keep GB reset released, matching the proven GB engine. No voltage switching.
assign pin30_pwroff_reset=bus_reg==1 || gb_save_requested || mbc7_requested || gb_write180 || mbc7_write185 || rtc_clock191;
assign pin31_out=1'b0;
assign pin31_dir=1'b0;
always @(*) begin
 reg_rdata=0;
 case(reg_addr)
  0:reg_rdata=32'h384e4243; // CBN8: private ABI signature
`ifdef CHISBURNER_SAVE158
  1:reg_rdata=32'h000100bf | GB_SAVE_CAP175 | MBC7_CAP179 | GB_WRITE_CAP180 | MBC7_WRITE_CAP185 | RTC_CAP191; // private write capabilities
`elsif CHISBURNER_SAVE139
  1:reg_rdata=32'h0001003f | GB_SAVE_CAP175 | MBC7_CAP179 | GB_WRITE_CAP180 | MBC7_WRITE_CAP185;
`elsif CHISBURNER_SRAM138
  1:reg_rdata=32'h0001001f | GB_SAVE_CAP175 | MBC7_CAP179 | GB_WRITE_CAP180 | MBC7_WRITE_CAP185;
`elsif CHISBURNER_SRAM137
  1:reg_rdata=32'h0001000f | GB_SAVE_CAP175 | MBC7_CAP179 | GB_WRITE_CAP180 | MBC7_WRITE_CAP185;
`else
  1:reg_rdata=32'h00010007 | GB_SAVE_CAP175 | MBC7_CAP179 | GB_WRITE_CAP180 | MBC7_WRITE_CAP185;
`endif
  2:reg_rdata=status_reg;
  4:reg_rdata=bus_reg;
  5:reg_rdata=address_reg;
  7:reg_rdata=timeout_reg;
  8:reg_rdata={16'd0,rx_reg};
  9:reg_rdata=rx_valid ? (is_gba?32'd2:32'd1):32'd0;
  10:reg_rdata=error_reg;
  19:reg_rdata={30'd0,arm};
  default:begin end
 endcase
end
always @(*) begin
 bank3_out=0;bank2_out=0;bank1_out=0;
 bank3_dir=0;bank2_dir=0;bank1_dir=0;bank0_out=4'hf;
 if(gate && busy && (state==RTC_LOW191 || state==RTC_HIGH191))begin
  bank0_out[3]=(state==RTC_HIGH191);
 end else if(gate && busy)begin
  if(is_gba)begin
   bank1_out=word_address[23:16];bank1_dir=1;
   if(state==ADDRESS || state==SELECT)begin
    bank3_out=word_address[7:0];bank2_out=word_address[15:8];
    bank3_dir=1;bank2_dir=1;
   end else if(is_write)begin
    bank3_out=active_data[7:0];bank2_out=active_data[15:8];
    bank3_dir=1;bank2_dir=1;
   end
   if(state!=ADDRESS)bank0_out=4'b1110;
  end else begin
   bank3_out=active_addr[7:0];bank2_out=active_addr[15:8];
   bank3_dir=1;bank2_dir=1;
   bank1_out=active_data[7:0];bank1_dir=is_write;
   if(is_gb_ram && state!=ADDRESS)bank0_out[0]=0;
  end
  if(state==STROBE)begin
   if(is_write)bank0_out[2]=0;else bank0_out[1]=0;
  end
 end
end
always @(posedge clk or negedge reset_n)begin
 if(!reset_n)begin
  state<=IDLE;bus_reg<=0;address_reg<=0;timeout_reg<=0;
  status_reg<=1;error_reg<=0;data_reg<=0;rx_reg<=0;arm<=0;
  ticks<=0;watchdog<=0;is_write<=0;is_gba<=0;is_bank_control<=0;is_gb_ram<=0;active_addr<=0;
  active_data<=0;rx_valid<=0;rtc_left191<=0;
 end else if(!lifecycle_ok || !selected)begin
  state<=IDLE;arm<=0;rx_valid<=0;is_write<=0;ticks<=0;watchdog<=0;
  status_reg<=lifecycle_ok?32'd1:32'd9;error_reg<=lifecycle_ok?32'd0:32'd3;
 end else if(control)begin
  state<=IDLE;arm<=0;rx_valid<=0;is_write<=0;ticks<=0;watchdog<=0;
  status_reg<=reg_wdata==3?32'd17:32'd1;error_reg<=0;
 end else if(busy && watchdog>=timeout_reg-1)begin
  state<=IDLE;arm<=0;rx_valid<=0;is_write<=0;
  status_reg<=41;error_reg<=4;
 end else begin
  if(reg_rd && reg_addr==8 && !busy)rx_valid<=0;
  if(busy)begin
   watchdog<=watchdog+1;
   ticks<=ticks+1;
   case(state)
    RTC_LOW191:if(ticks>=63)begin state<=RTC_HIGH191;ticks<=0;end
    RTC_HIGH191:if(ticks>=63)begin
     ticks<=0;
     if(rtc_left191==1)begin state<=IDLE;status_reg<=5;rx_valid<=0;end
     else begin rtc_left191<=rtc_left191-1'b1;state<=RTC_LOW191;end
    end
    ADDRESS:if(ticks>=ADDRESS_CYCLES-1)begin state<=SELECT;ticks<=0;end
    SELECT:if(ticks>=SELECT_CYCLES-1)begin state<=DATA;ticks<=0;end
    DATA:if(ticks>=DATA_SETUP_CYCLES-1)begin state<=STROBE;ticks<=0;end
    STROBE:if(ticks>=STROBE_CYCLES-1)begin
     if(!is_write)rx_reg<=is_gba?{bank2_in,bank3_in}:{8'd0,gb_data_in};
     state<=RELEASE;ticks<=0;
    end
    RELEASE:if(ticks>=RELEASE_CYCLES-1)begin
     state<=IDLE;ticks<=0;status_reg<=5;rx_valid<=!is_write;is_write<=0;
    end
    default:begin state<=IDLE;status_reg<=9;error_reg<=1;arm<=0;end
   endcase
   // No configuration/arm changes can be queued while a transaction is active.
   if(reg_wr)arm<=0;
  end else if(reg_wr)begin
   case(reg_wr_addr)
    4:begin bus_reg<=reg_wdata;arm<=0;end
    5:begin address_reg<=reg_wdata;arm<=0;end
    7:begin timeout_reg<=reg_wdata;arm<=0;end
    17:begin data_reg<=reg_wdata[15:0];arm<=0;end
    19:begin
     if(arm==0 && reg_wdata==KEY1)arm<=1;
     else if(arm==1 && reg_wdata==KEY2)arm<=2;
     else arm<=0;
    end
    3:begin
     arm<=0;rx_valid<=0;rx_reg<=0;
     if(rtc_clock191 && reg_wdata==8 && arm==2 && timeout_reg!=0 && TIMING_OK)begin
      state<=RTC_LOW191;status_reg<=2;error_reg<=0;watchdog<=0;ticks<=0;
      rtc_left191<=50;is_write<=0;is_gba<=0;is_bank_control<=0;is_gb_ram<=0;
     end
     else if((reg_wdata!=2 && reg_wdata!=7) || !TIMING_OK)begin status_reg<=9;error_reg<=1;end
     else if(!range_ok || timeout_reg==0)begin status_reg<=9;error_reg<=7;end
     else if(reg_wdata==7 && arm!=2)begin status_reg<=9;error_reg<=6;end
     else begin
      state<=ADDRESS;status_reg<=2;error_reg<=0;watchdog<=0;ticks<=0;
      active_addr<=address_reg[24:0];active_data<=data_reg;
      is_gb_ram<=(gb_save_requested && reg_wdata==2 && address_reg>=32'ha000) ||
       (gb_write180 && address_reg>=32'ha000) ||
       ((mbc7_requested || mbc7_write185) && address_reg==32'ha080);
      is_gba<=gba_requested;is_bank_control<=bank_requested||save_requested||save_probe_requested||save_data_requested;is_write<=reg_wdata==7;
     end
    end
    default:arm<=0;
   endcase
  end
 end
end
`ifdef CHISBURNER_GB_SAVE179
always @(posedge clk or negedge reset_n)begin
 if(!reset_n)begin ee_bits179<=0;ee_clock179<=0;end
 else if(!lifecycle_ok||!selected||control||(reg_wr&&!busy&&reg_wr_addr==4))begin
  ee_bits179<=0;ee_clock179<=0;
 end else if(reg_wr&&!busy&&reg_wr_addr==3&&reg_wdata==7&&mbc7_requested&&
   range_ok&&arm==2&&timeout_reg!=0&&TIMING_OK&&address_reg==32'ha080)begin
  if(!data_reg[7])begin ee_bits179<=0;ee_clock179<=0;end
  else begin
   ee_clock179<=data_reg[6];
   if(ee_edge179&&ee_bits179<11)ee_bits179<=ee_bits179+1'b1;
  end
 end
end
`endif
`ifdef CHISBURNER_GB_RESTORE185
always @(posedge clk or negedge reset_n)begin
 if(!reset_n)begin ee_count185<=0;ee_command185<=0;ee_opcode185<=0;ee_clock185<=0;ee_enable185<=0;end
 else if(!lifecycle_ok||!selected||control||(reg_wr&&!busy&&reg_wr_addr==4))begin
  ee_count185<=0;ee_command185<=0;ee_opcode185<=0;ee_clock185<=0;ee_enable185<=0;
 end else if(reg_wr&&!busy&&reg_wr_addr==3&&reg_wdata==7&&mbc7_write185&&
   range_ok&&arm==2&&timeout_reg!=0&&TIMING_OK&&address_reg==32'ha080)begin
  if(!data_reg[7])begin
   if(ee_count185==11&&ee_command185==11'h4c0)ee_enable185<=1;
   if(ee_count185==11&&ee_command185==11'h400)ee_enable185<=0;
   ee_count185<=0;ee_command185<=0;ee_opcode185<=0;ee_clock185<=0;
  end else begin
   ee_clock185<=data_reg[6];
   if(ee_edge185&&ee_count185<27)begin
    ee_count185<=ee_count185+1'b1;
    if(ee_count185<11)ee_command185<={ee_command185[9:0],data_reg[1]};
    if(ee_count185==2)ee_opcode185<={ee_command185[0],data_reg[1]};
   end
  end
 end
end
`endif
endmodule
`default_nettype wire
