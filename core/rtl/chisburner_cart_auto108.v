// SPDX-License-Identifier: Apache-2.0
// NOR108 wrapper preserves auto98 on the legacy path. Space 31 is OS-private.
// Register 31 is a read-only extension signature, not a write authorization.
`default_nettype none
module chisburner_cart_auto108(
`ifdef CHISBURNER_PROGRAM131
 input wire memory_clk,memory_reset_n,stream_authorized,stream_psram_granted,
 input wire stream_lease_busy,
 output reg stream_lease_request,
 output wire stream_hold_owner,
 output wire stream_p0_rd,stream_p1_rd,
 output wire [21:0] stream_p0_address,stream_p1_address,
 input wire [31:0] stream_p0_data,stream_p1_data,
 input wire stream_p0_valid,stream_p1_valid,
`endif
 input wire clk,reset_n,lifecycle_ok,
 input wire [31:0] lifecycle_status,host_b1_raw,host_diag,
 input wire reg_wr,reg_rd,
 input wire [4:0] reg_addr,
 input wire [31:0] reg_wdata,
 output wire [31:0] reg_rdata,
 input wire [7:0] gb_data_in,bank3_in,bank2_in,
 output wire [7:0] bank3_out,
 output wire  bank3_dir,
 output wire [7:0] bank2_out,
 output wire  bank2_dir,
 output wire [7:0] bank1_out,
 output wire  bank1_dir,
 output wire [3:0] bank0_out,
 output wire  bank0_dir,
 output wire  pin30_out,
 output wire  pin30_dir,
 output wire  pin30_pwroff_reset,
 output wire  pin31_out,
 output wire  pin31_dir,
 output wire writes_enabled
);
reg private_mode,legacy_pending,raw_wr_q;
reg lifecycle_previous;
reg [31:0] lifecycle_epoch;
reg [2:0] legacy_delay;
reg [4:0] read_addr_q,raw_addr_q;
reg [31:0] raw_data_q;
reg [31:0] read_mux;
wire [31:0] legacy_rdata,raw_rdata;
wire raw_busy,raw_write_active,legacy_writes;
wire selector=reg_wr && reg_addr==16;
wire enter_private=selector && reg_wdata==31;
wire legacy_wr=reg_wr && !private_mode && !enter_private;
wire raw_queued=raw_wr_q && raw_addr_q==3;
`ifdef CHISBURNER_PROGRAM131
wire lease_register=reg_addr==25;
wire lease_idle=!stream_lease_busy;
reg stream_arm_first,stream_permit,stream_was_busy;
always @(posedge clk or negedge reset_n)begin
 if(!reset_n)begin stream_arm_first<=0;stream_permit<=0;stream_was_busy<=0;end
 else begin
  stream_was_busy<=raw_busy;
  if(!lifecycle_ok || !private_mode || !stream_psram_granted || !stream_authorized ||
     (stream_was_busy && !raw_busy) ||
     (reg_wr && reg_addr==22 && reg_wdata==2))begin
   stream_arm_first<=0;stream_permit<=0;
  end else if(reg_wr && reg_addr==26)begin
   stream_arm_first<=0;stream_permit<=0;
   if(!raw_busy)begin
    if(reg_wdata==32'h53413131)stream_arm_first<=1;
    else if(reg_wdata==32'h53413132 && stream_arm_first)stream_permit<=1;
   end
  end
 end
end
always @(posedge clk or negedge reset_n)
 if(!reset_n)stream_lease_request<=0;
 else if(!lifecycle_ok || !private_mode)stream_lease_request<=0;
 else if(reg_wr && lease_register && (reg_wdata==0 || reg_wdata==1))
  stream_lease_request<=reg_wdata[0];
`else
wire lease_register=1'b0;
wire lease_idle=1'b1;
`endif
always @(posedge clk or negedge reset_n)begin
 if(!reset_n)begin
  private_mode<=0;legacy_pending<=0;legacy_delay<=0;
  read_addr_q<=0;raw_wr_q<=0;raw_addr_q<=0;raw_data_q<=0;
  lifecycle_previous<=0;lifecycle_epoch<=0;
 end else begin
  lifecycle_previous<=lifecycle_ok;
  if(lifecycle_previous && !lifecycle_ok)lifecycle_epoch<=lifecycle_epoch+1'b1;
  read_addr_q<=reg_addr;
  raw_wr_q<=reg_wr && private_mode && !selector && !lease_register && lifecycle_ok
`ifdef CHISBURNER_PROGRAM131
    && reg_addr!=26
`endif
    ;
  if(reg_wr)begin raw_addr_q<=reg_addr;raw_data_q<=reg_wdata;end
  if(legacy_delay!=0)legacy_delay<=legacy_delay-1'b1;
  if(legacy_wr && reg_addr==3)begin legacy_pending<=1;legacy_delay<=4;end
  else if(legacy_pending && legacy_delay==0 && reg_rd &&
          read_addr_q==2 && !legacy_rdata[1])legacy_pending<=0;
  // Switching engines while a command is queued/running is not permitted.
  // SAFE/ABORT must be observed complete before the OS changes owners.
  if(selector)begin
   if(private_mode && !raw_busy && !raw_wr_q && lease_idle && reg_wdata!=31)private_mode<=0;
   else if(!private_mode && !legacy_pending && reg_wdata==31)private_mode<=1;
  end
  if(!lifecycle_ok)begin private_mode<=0;legacy_pending<=0;legacy_delay<=0;raw_wr_q<=0;end
 end
end
always @* begin
 read_mux=private_mode?raw_rdata:legacy_rdata;
 if(private_mode)begin
  if(read_addr_q==16)read_mux=32'd31;
  else if(read_addr_q==14)read_mux=lifecycle_epoch;
  else if(raw_queued && read_addr_q==2)read_mux=32'd2;
 end
 case(read_addr_q)
  31:read_mux=32'h384e4243;
`ifdef CHISBURNER_PROGRAM131
 // Drain visibility must survive lifecycle loss/private deselection. Otherwise
 // the OS could mistake legacy zero readback for completed memory release.
  25:read_mux={16'h5313,13'd0,stream_psram_granted,stream_lease_busy,stream_lease_request};
  26:read_mux={31'd0,stream_permit};
`endif
  11:read_mux=lifecycle_status;
  12:read_mux=host_b1_raw;
  13:read_mux=host_diag;
 endcase
end
assign reg_rdata=read_mux;
// Private engine is not a legacy S29GL256 capability. OS owns new job tokens.
assign writes_enabled=!private_mode && legacy_writes;
wire [7:0] old_bank3_out,new_bank3_out;
assign bank3_out=private_mode?new_bank3_out:old_bank3_out;
wire  old_bank3_dir,new_bank3_dir;
assign bank3_dir=private_mode?new_bank3_dir:old_bank3_dir;
wire [7:0] old_bank2_out,new_bank2_out;
assign bank2_out=private_mode?new_bank2_out:old_bank2_out;
wire  old_bank2_dir,new_bank2_dir;
assign bank2_dir=private_mode?new_bank2_dir:old_bank2_dir;
wire [7:0] old_bank1_out,new_bank1_out;
assign bank1_out=private_mode?new_bank1_out:old_bank1_out;
wire  old_bank1_dir,new_bank1_dir;
assign bank1_dir=private_mode?new_bank1_dir:old_bank1_dir;
wire [3:0] old_bank0_out,new_bank0_out;
assign bank0_out=private_mode?new_bank0_out:old_bank0_out;
wire  old_bank0_dir,new_bank0_dir;
assign bank0_dir=private_mode?new_bank0_dir:old_bank0_dir;
wire  old_pin30_out,new_pin30_out;
assign pin30_out=private_mode?new_pin30_out:old_pin30_out;
wire  old_pin30_dir,new_pin30_dir;
assign pin30_dir=private_mode?new_pin30_dir:old_pin30_dir;
wire  old_pin30_pwroff_reset,new_pin30_pwroff_reset;
assign pin30_pwroff_reset=private_mode?new_pin30_pwroff_reset:old_pin30_pwroff_reset;
wire  old_pin31_out,new_pin31_out;
assign pin31_out=private_mode?new_pin31_out:old_pin31_out;
wire  old_pin31_dir,new_pin31_dir;
assign pin31_dir=private_mode?new_pin31_dir:old_pin31_dir;
chisburner_cart_auto98 legacy(
 .clk(clk),.reset_n(reset_n),.lifecycle_ok(lifecycle_ok && !private_mode),
 .lifecycle_status(lifecycle_status),.host_b1_raw(host_b1_raw),.host_diag(host_diag),
 .reg_wr(legacy_wr),.reg_rd(reg_rd && !private_mode),.reg_addr(reg_addr),.reg_wdata(reg_wdata),
 .reg_rdata(legacy_rdata),.gb_data_in(gb_data_in),.bank3_in(bank3_in),.bank2_in(bank2_in),
 .bank3_out(old_bank3_out),
 .bank3_dir(old_bank3_dir),
 .bank2_out(old_bank2_out),
 .bank2_dir(old_bank2_dir),
 .bank1_out(old_bank1_out),
 .bank1_dir(old_bank1_dir),
 .bank0_out(old_bank0_out),
 .bank0_dir(old_bank0_dir),
 .pin30_out(old_pin30_out),
 .pin30_dir(old_pin30_dir),
 .pin30_pwroff_reset(old_pin30_pwroff_reset),
 .pin31_out(old_pin31_out),
 .pin31_dir(old_pin31_dir),
 .writes_enabled(legacy_writes)
);
`ifdef CHISBURNER_PROGRAM131
// Preserve one queued write stage and the independent registered read address.
// Authorization/grant come from the parent, never from descriptor keys alone.
assign stream_hold_owner=raw_busy || raw_wr_q;
chisburner_program_path131 raw(
 .memory_clk(memory_clk),.memory_reset_n(memory_reset_n),
 .write_allowed(stream_permit && stream_authorized),.psram_granted(stream_psram_granted),
 .hold_owner(raw_busy),
 .p0_rd(stream_p0_rd),.p1_rd(stream_p1_rd),
 .p0_address(stream_p0_address),.p1_address(stream_p1_address),
 .p0_data(stream_p0_data),.p1_data(stream_p1_data),
 .p0_valid(stream_p0_valid),.p1_valid(stream_p1_valid),
 .reg_address(raw_addr_q),.reg_read_address(read_addr_q),
 .reg_data(raw_data_q),.reg_read_data(raw_rdata),
`else
chisburner_nor_bus108 raw(
 .reg_addr(read_addr_q),.reg_wr_addr(raw_addr_q),
 .reg_wdata(raw_data_q),.reg_rdata(raw_rdata),.busy(raw_busy),
`endif
 .clk(clk),.reset_n(reset_n),.lifecycle_ok(lifecycle_ok),.selected(private_mode),
 .reg_wr(raw_wr_q),.reg_rd(reg_rd && private_mode),
 .gb_data_in(gb_data_in),.bank3_in(bank3_in),.bank2_in(bank2_in),
 .bank3_out(new_bank3_out),
 .bank3_dir(new_bank3_dir),
 .bank2_out(new_bank2_out),
 .bank2_dir(new_bank2_dir),
 .bank1_out(new_bank1_out),
 .bank1_dir(new_bank1_dir),
 .bank0_out(new_bank0_out),
 .bank0_dir(new_bank0_dir),
 .pin30_out(new_pin30_out),
 .pin30_dir(new_pin30_dir),
 .pin30_pwroff_reset(new_pin30_pwroff_reset),
 .pin31_out(new_pin31_out),
 .pin31_dir(new_pin31_dir),
 .write_active(raw_write_active)
);
endmodule
`default_nettype wire
