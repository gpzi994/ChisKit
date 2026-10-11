// SPDX-License-Identifier: AGPL-3.0-only
`default_nettype none
// Integrated private programming path, source clock to physical PSRAM/NOR ports.
// Parent acquires BOTH PSRAM controllers before submitting and retains the grant
// until hold_owner is low. CPU register input must already be OS-private.
// Common reset must also reset physical PSRAM controllers; synchronize release
// separately in clk and memory_clk domains. Not yet selected by Pocket top.
module chisburner_program_path131(
 input wire clk,reset_n,memory_clk,memory_reset_n,
 input wire lifecycle_ok,selected,write_allowed,psram_granted,
 input wire reg_wr,reg_rd,input wire [4:0] reg_address,reg_read_address,input wire [31:0] reg_data,
 output wire [31:0] reg_read_data,
 output wire hold_owner,busy,done,output wire [3:0] error,
 output wire rejected,
 output wire p0_rd,p1_rd,output wire [21:0] p0_address,p1_address,
 input wire [31:0] p0_data,p1_data,input wire p0_valid,p1_valid,
 input wire [7:0] gb_data_in,bank3_in,bank2_in,
 output wire [7:0] bank3_out,bank2_out,bank1_out,
 output wire bank3_dir,bank2_dir,bank1_dir,
 output wire [3:0] bank0_out,output wire bank0_dir,
 output wire pin30_out,pin30_dir,pin30_pwroff_reset,pin31_out,pin31_dir,
 output wire write_active
);
 wire live=lifecycle_ok && selected;
 wire allowed=write_allowed && psram_granted;
 wire mr,mready,mbusy,mdone,merror;wire [22:0] ma;wire [31:0] mdata;
 wire tr,tw,tready,tdone,terror,adapter_busy;wire [24:0] ta;wire [15:0] td,tdata;
 wire aw,ar,raw_busy;wire [4:0] aa;wire [31:0] ad,raw_data,frontend_data;
 reg cancelled,cpu_rejected;
 wire cancel_now=cancelled || (reg_wr && reg_address==22 && reg_data==2);
 wire stop=cancel_now || !live || !allowed;
 wire private_register=reg_address>=20 && reg_address<=24;
 wire private_read=reg_read_address>=20 && reg_read_address<=24;
 wire ee_hold,ee_write,ee_read;
 wire [31:0] ee_data;
 assign hold_owner=busy || mbusy || adapter_busy || raw_busy || ee_hold;
 // No CPU raw access during an accepted job or adapter drain, including gaps
 // between NOR cycles. Frontend submission itself is not a raw register write.
 wire stream_owns=busy || mbusy || adapter_busy;
 // Legacy raw commands still accept SAFE/ABORT while their own cycle runs.
 // Only an autonomous stream job excludes CPU writes here.
 wire cpu_ok=!stream_owns && !ee_hold && live;
 assign reg_read_data=ee_read?ee_data:private_read?
  (frontend_data | ((reg_read_address==23 && cpu_rejected)?32'd8:32'd0)):
`ifdef CHISBURNER_EEPROM139
  (reg_read_address==1 ? (raw_data | 32'h00000040) : raw_data);
`else
  raw_data;
`endif
 wire front_rejected;
 assign rejected=front_rejected || cpu_rejected;
 always @(posedge clk or negedge reset_n)begin
  if(!reset_n)begin cancelled<=0;cpu_rejected<=0;end
  else begin
   if(!hold_owner)cancelled<=0;
   if(busy && stop)cancelled<=1;
   if(reg_wr && ((!private_register && stream_owns) || (!ee_write && ee_hold)))cpu_rejected<=1;
  end
 end
 chisburner_stream_regs131 frontend(
  .clk(clk),.reset_n(reset_n),.lifecycle_ok(live),.write_allowed(allowed),
  .idle_adapters(!mbusy && !adapter_busy && !raw_busy && !ee_hold),
  .wr(reg_wr && private_register && !ee_hold),.address(reg_address),.read_address(reg_read_address),.data(reg_data),.read_data(frontend_data),
  .busy(busy),.done(done),.error(error),.rejected(front_rejected),
  .mem_req(mr),.mem_address(ma),.mem_ready(mready),.mem_done(mdone),.mem_error(merror),.mem_data(mdata),
  .tx_req(tr),.tx_write(tw),.tx_address(ta),.tx_data(td),.tx_ready(tready),
  .tx_done(tdone),.tx_error(terror),.tx_read_data(tdata));
 chisburner_psram_stream131 memory_path(
  .s_clk(clk),.s_reset_n(reset_n),.cancel(stop),.req(mr),.address(ma),
  .ready(mready),.busy(mbusy),.done(mdone),.error(merror),.data(mdata),
  .m_clk(memory_clk),.m_reset_n(memory_reset_n),
  .p0_rd(p0_rd),.p1_rd(p1_rd),.p0_address(p0_address),.p1_address(p1_address),
  .p0_data(p0_data),.p1_data(p1_data),.p0_valid(p0_valid),.p1_valid(p1_valid));
 chisburner_raw_adapter131 adapter(
  .clk(clk),.reset_n(reset_n),.lifecycle_ok(live),.write_allowed(allowed),.abort(stop),
  .req(tr),.write(tw),.address(ta),.data(td),.ready(tready),.busy(adapter_busy),
  .done(tdone),.fault(terror),.read_data(tdata),.reg_wr(aw),.reg_rd(ar),
  .reg_address(aa),.reg_data(ad),.reg_read_data(raw_data),.raw_busy(raw_busy));
 chisburner_nor_bus108 raw(
  .clk(clk),.reset_n(reset_n),.lifecycle_ok(lifecycle_ok),.selected(selected && !ee_hold),
  .reg_wr(adapter_busy?aw:(cpu_ok && reg_wr && !private_register && !ee_write)),
  .reg_rd(adapter_busy?ar:(live && !ee_hold && reg_rd && !private_read && !ee_read)),
  .reg_addr(adapter_busy?aa:reg_read_address),.reg_wr_addr(adapter_busy?aa:reg_address),
  .reg_wdata(adapter_busy?ad:reg_data),.reg_rdata(raw_data),
  .gb_data_in(gb_data_in),.bank3_in(bank3_in),.bank2_in(bank2_in),
  .bank3_out(raw_bank3_out),.bank2_out(raw_bank2_out),.bank1_out(raw_bank1_out),
  .bank3_dir(raw_bank3_dir),.bank2_dir(raw_bank2_dir),.bank1_dir(raw_bank1_dir),
  .bank0_out(raw_bank0_out),.bank0_dir(raw_bank0_dir),.pin30_out(raw_pin30_out),.pin30_dir(raw_pin30_dir),
  .pin30_pwroff_reset(raw_pin30_pwroff_reset),.pin31_out(raw_pin31_out),.pin31_dir(raw_pin31_dir),
  .busy(raw_busy),.write_active(write_active));
 wire [7:0] raw_bank3_out,ee_bank3_out;
 assign bank3_out=ee_hold?ee_bank3_out:raw_bank3_out;
 wire [7:0] raw_bank2_out,ee_bank2_out;
 assign bank2_out=ee_hold?ee_bank2_out:raw_bank2_out;
 wire [7:0] raw_bank1_out,ee_bank1_out;
 assign bank1_out=ee_hold?ee_bank1_out:raw_bank1_out;
 wire raw_bank3_dir,ee_bank3_dir;
 assign bank3_dir=ee_hold?ee_bank3_dir:raw_bank3_dir;
 wire raw_bank2_dir,ee_bank2_dir;
 assign bank2_dir=ee_hold?ee_bank2_dir:raw_bank2_dir;
 wire raw_bank1_dir,ee_bank1_dir;
 assign bank1_dir=ee_hold?ee_bank1_dir:raw_bank1_dir;
 wire [3:0] raw_bank0_out,ee_bank0_out;
 assign bank0_out=ee_hold?ee_bank0_out:raw_bank0_out;
 wire raw_bank0_dir,ee_bank0_dir;
 assign bank0_dir=ee_hold?ee_bank0_dir:raw_bank0_dir;
 wire raw_pin30_out,ee_pin30_out;
 assign pin30_out=ee_hold?ee_pin30_out:raw_pin30_out;
 wire raw_pin30_dir,ee_pin30_dir;
 assign pin30_dir=ee_hold?ee_pin30_dir:raw_pin30_dir;
 wire raw_pin30_pwroff_reset,ee_pin30_pwroff_reset;
 assign pin30_pwroff_reset=ee_hold?ee_pin30_pwroff_reset:raw_pin30_pwroff_reset;
 wire raw_pin31_out,ee_pin31_out;
 assign pin31_out=ee_hold?ee_pin31_out:raw_pin31_out;
 wire raw_pin31_dir,ee_pin31_dir;
 assign pin31_dir=ee_hold?ee_pin31_dir:raw_pin31_dir;
`ifdef CHISBURNER_EEPROM139
 assign ee_write=reg_address>=27 && reg_address<=30;
 assign ee_read=reg_read_address>=27 && reg_read_address<=30;
 wire [2:0] ee_ra=reg_read_address==27?3'd1:reg_read_address==28?3'd3:reg_read_address==29?3'd4:3'd5;
 chisburner_eeprom_path139 eeprom(
  .clk(clk),.reset_n(reset_n),.lifecycle_ok(lifecycle_ok),.selected(selected),
  .other_busy(stream_owns || raw_busy),.other_request(reg_wr && !ee_write),
  .wr(reg_wr && ee_write),.address(reg_address==27?3'd1:reg_address==28?3'd2:reg_address==29?3'd4:3'd5),
  .read_address(ee_ra),.data(reg_data),.read_data(ee_data),.hold_owner(ee_hold),.other_accept(),
  .bank3_in(bank3_in),
  .bank3_out(ee_bank3_out),.bank2_out(ee_bank2_out),.bank1_out(ee_bank1_out),.bank3_dir(ee_bank3_dir),.bank2_dir(ee_bank2_dir),.bank1_dir(ee_bank1_dir),.bank0_out(ee_bank0_out),.bank0_dir(ee_bank0_dir),.pin30_out(ee_pin30_out),.pin30_dir(ee_pin30_dir),.pin30_pwroff_reset(ee_pin30_pwroff_reset),.pin31_out(ee_pin31_out),.pin31_dir(ee_pin31_dir)
 );
`else
 assign ee_hold=0;assign ee_write=0;assign ee_read=0;assign ee_data=0;
 assign ee_bank3_out=0;
 assign ee_bank2_out=0;
 assign ee_bank1_out=0;
 assign ee_bank3_dir=0;
 assign ee_bank2_dir=0;
 assign ee_bank1_dir=0;
 assign ee_bank0_out=0;
 assign ee_bank0_dir=0;
 assign ee_pin30_out=0;
 assign ee_pin30_dir=0;
 assign ee_pin30_pwroff_reset=0;
 assign ee_pin31_out=0;
 assign ee_pin31_dir=0;
`endif
endmodule
`default_nettype wire
