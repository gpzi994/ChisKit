// SPDX-License-Identifier: Apache-2.0
// UI98 router: unchanged GBA programmer; bounded GB header/mapper/CFI engines.
// Private space 5 is OS-only mapper sampling. No GB erase/program path exists.
`default_nettype none
module chisburner_cart_auto98(
input wire clk, reset_n, lifecycle_ok,
input wire [31:0] lifecycle_status, host_b1_raw, host_diag,
input wire reg_wr, reg_rd,
input wire [4:0] reg_addr,
input wire [31:0] reg_wdata,
output wire [31:0] reg_rdata,
input wire [7:0] gb_data_in, bank3_in, bank2_in,
output wire [7:0] bank3_out,
output wire bank3_dir,
output wire [7:0] bank2_out,
output wire bank2_dir,
output wire [7:0] bank1_out,
output wire bank1_dir,
output wire [3:0] bank0_out,
output wire bank0_dir,
output wire pin30_out,
output wire pin30_dir,
output wire pin30_pwroff_reset,
output wire pin31_out,
output wire pin31_dir,
output wire writes_enabled
);

// One-cycle write stage isolates the AXI address decoder from four engines.
// AXI completion precedes its next request; status also reports a queued command
// as busy. No cartridge cycle timing or engine command sequence is changed.
reg wr_q;
reg [4:0] addr_q;
reg [4:0] read_addr_q;
reg [31:0] data_q;
always @(posedge clk or negedge reset_n) begin
 if(!reset_n) begin wr_q<=0;addr_q<=0;read_addr_q<=0;data_q<=0;end
 else begin
  wr_q<=reg_wr && lifecycle_ok;
  // AXI peripheral reads already have WAIT then LATCH cycles.  Capture the
  // registered AXI request address here so no direct req_addr path enters an
  // engine state machine while preserving the existing read response timing.
  read_addr_q<=reg_addr;
  if(reg_wr) begin addr_q<=reg_addr;data_q<=reg_wdata;end
 end
end
wire issue_wr = wr_q && lifecycle_ok;
wire [31:0] issue_wdata = data_q;
reg [3:0] engine_wr_q;
reg [4:0] e1_addr_q, e2_addr_q, e3_addr_q;
reg [31:0] e1_data_q, e2_data_q, e3_data_q;
wire issue_rd = reg_rd && !wr_q && !(|engine_wr_q);
// Every command is delayed by both router stages.  Report BUSY for the whole
// interval so software cannot mistake the previous engine status for the
// result of a just-issued SAFE, ABORT, query, DISARM, read, erase or program.
wire queued_input_command = wr_q && addr_q==3;
wire queued_engine_command =
    (engine_wr_q[1] && e1_addr_q==3) ||
    (engine_wr_q[2] && e2_addr_q==3) ||
    (engine_wr_q[3] && e3_addr_q==3);
wire queued_command = queued_input_command || queued_engine_command;
reg [31:0] bus_sel, space_sel;
reg [1:0] engine_sel;
reg pending, restart_mapper, reset_gba;
wire [1:0] selected = engine_sel;
wire [31:0] r1,r2,r3;
wire [31:0] active_rdata = selected==1?r1:selected==2?r2:r3;
wire selector_write = addr_q==4 || addr_q==16;
wire accepted_wr = issue_wr && !(pending && selector_write);
wire safe_command = issue_wr && addr_q==3 && (issue_wdata==1 || issue_wdata==3);
wire common_write = (addr_q>=4 && addr_q<=7) || addr_q==14 || addr_q==16;
wire [3:0] selected_mask = 4'b0001 << selected;
wire [3:0] dispatch_mask = (common_write || safe_command) ? 4'b1110 : selected_mask;
always @(posedge clk or negedge reset_n) begin
 if(!reset_n) begin
  engine_wr_q<=0;
  e1_addr_q<=0;e2_addr_q<=0;e3_addr_q<=0;
  e1_data_q<=0;e2_data_q<=0;e3_data_q<=0;
 end else begin
  engine_wr_q<=accepted_wr ? dispatch_mask : 4'b0000;
  if(accepted_wr) begin
   e1_addr_q<=addr_q;e2_addr_q<=addr_q;e3_addr_q<=addr_q;
   e1_data_q<=addr_q==4 ? 32'd1 : addr_q==16 ? 32'd4 : issue_wdata;
   e2_data_q<=addr_q==4 ? 32'd1 : addr_q==16 ?
              (issue_wdata>=16&&issue_wdata<=23?issue_wdata:issue_wdata==5 ? 32'd5 : issue_wdata==6 ? 32'd6 : 32'd0) : issue_wdata;
   e3_data_q<=addr_q==4 ? 32'd2 : issue_wdata;
  end
 end
end
always @(posedge clk or negedge reset_n) begin
 if(!reset_n) begin bus_sel<=1;space_sel<=0;engine_sel<=2;pending<=0;restart_mapper<=0;reset_gba<=0;end
 else begin
  restart_mapper<=0; reset_gba<=0;
  if(issue_rd && read_addr_q==2 && !active_rdata[1]) pending<=0;
  if(issue_wr && addr_q==3 && (issue_wdata==2 || issue_wdata==4 || issue_wdata==5)) pending<=1;
  // SAFE is the boundary of a new scan.  Always reset the private mapper
  // sequence counter, even if the previous command is still marked pending.
  // The old !pending guard made only the first GB/GBC analysis possible.
  if(safe_command) pending<=0;
  if(issue_wr && addr_q==3 && issue_wdata==1) begin
   restart_mapper<=1; reset_gba<=1;
  end
  if(accepted_wr && addr_q==4 && (issue_wdata==1 || issue_wdata==2)) begin
   bus_sel<=issue_wdata;
   engine_sel<=issue_wdata==2 ? 2'd3 :
               space_sel==4 ? 2'd1 : 2'd2;
   if(bus_sel==2 && issue_wdata==1) reset_gba<=1;
  end
  if(accepted_wr && addr_q==16) begin
   space_sel<=issue_wdata;
   if(bus_sel==1)
    engine_sel<=issue_wdata==4 ? 2'd1 : 2'd2;
  end
  if(!lifecycle_ok) begin bus_sel<=1;space_sel<=0;engine_sel<=2;pending<=0;restart_mapper<=1;reset_gba<=1;end
 end
end
`ifdef CHISBURNER_SRAM138
localparam [31:0] CART_CAPS137=32'h06713f03; // bit 26: SRAM138; space mask unchanged
`elsif CHISBURNER_SRAM137
localparam [31:0] CART_CAPS137=32'h02713f03; // GBA SAVE_PARALLEL read only
`else
localparam [31:0] CART_CAPS137=32'h02313f03;
`endif
assign reg_rdata = queued_command && read_addr_q==2 ? 32'd2 : read_addr_q==1 ? CART_CAPS137 : read_addr_q==4 ? bus_sel :
                   read_addr_q==16 ? space_sel : active_rdata;
wire gba_writes_enabled;
assign writes_enabled = selected==3 && gba_writes_enabled;
wire [7:0] p1_bank3_out,p2_bank3_out,p3_bank3_out;
assign bank3_out=selected==1?p1_bank3_out:selected==2?p2_bank3_out:p3_bank3_out;
wire p1_bank3_dir,p2_bank3_dir,p3_bank3_dir;
assign bank3_dir=selected==1?p1_bank3_dir:selected==2?p2_bank3_dir:p3_bank3_dir;
wire [7:0] p1_bank2_out,p2_bank2_out,p3_bank2_out;
assign bank2_out=selected==1?p1_bank2_out:selected==2?p2_bank2_out:p3_bank2_out;
wire p1_bank2_dir,p2_bank2_dir,p3_bank2_dir;
assign bank2_dir=selected==1?p1_bank2_dir:selected==2?p2_bank2_dir:p3_bank2_dir;
wire [7:0] p1_bank1_out,p2_bank1_out,p3_bank1_out;
assign bank1_out=selected==1?p1_bank1_out:selected==2?p2_bank1_out:p3_bank1_out;
wire p1_bank1_dir,p2_bank1_dir,p3_bank1_dir;
assign bank1_dir=selected==1?p1_bank1_dir:selected==2?p2_bank1_dir:p3_bank1_dir;
wire [3:0] p1_bank0_out,p2_bank0_out,p3_bank0_out;
assign bank0_out=selected==1?p1_bank0_out:selected==2?p2_bank0_out:p3_bank0_out;
wire p1_bank0_dir,p2_bank0_dir,p3_bank0_dir;
assign bank0_dir=selected==1?p1_bank0_dir:selected==2?p2_bank0_dir:p3_bank0_dir;
wire p1_pin30_out,p2_pin30_out,p3_pin30_out;
assign pin30_out=selected==1?p1_pin30_out:selected==2?p2_pin30_out:p3_pin30_out;
wire p1_pin30_dir,p2_pin30_dir,p3_pin30_dir;
assign pin30_dir=selected==1?p1_pin30_dir:selected==2?p2_pin30_dir:p3_pin30_dir;
wire p1_pin30_pwroff_reset,p2_pin30_pwroff_reset,p3_pin30_pwroff_reset;
// Hold the GB reset/power-off policy safe even before engine_sel receives its
// first reset clock edge; all GB engines drive this high in normal operation.
assign pin30_pwroff_reset=!reset_n ? 1'b1 :
                           selected==1?p1_pin30_pwroff_reset:
                           selected==2?p2_pin30_pwroff_reset:p3_pin30_pwroff_reset;
wire p1_pin31_out,p2_pin31_out,p3_pin31_out;
assign pin31_out=selected==1?p1_pin31_out:selected==2?p2_pin31_out:p3_pin31_out;
wire p1_pin31_dir,p2_pin31_dir,p3_pin31_dir;
assign pin31_dir=selected==1?p1_pin31_dir:selected==2?p2_pin31_dir:p3_pin31_dir;
`ifdef CHISBURNER_GB_IDENTIFY134
// Retired fixed-query slot: fail closed without any cartridge pin activity.
// Space 5 mapper samples and dump apertures still route to engine2.
assign r1=read_addr_q==2 ? 32'd9 : 32'd0;
assign p1_bank3_out=0; assign p1_bank3_dir=0;
assign p1_bank2_out=0; assign p1_bank2_dir=0;
assign p1_bank1_out=0; assign p1_bank1_dir=0;
assign p1_bank0_out=4'hf; assign p1_bank0_dir=0;
assign p1_pin30_out=1; assign p1_pin30_dir=0;
assign p1_pin30_pwroff_reset=1;
assign p1_pin31_out=1; assign p1_pin31_dir=0;
`else
chisburner_cart_gb_mbc5_identify #(.GEOMETRY_PROFILE(1)) engine1 (
 .clk(clk),.reset_n(reset_n),.lifecycle_ok(lifecycle_ok),
 .lifecycle_status(lifecycle_status),.host_b1_raw(host_b1_raw),.host_diag(host_diag),
 .reg_wr(engine_wr_q[1] && lifecycle_ok),
 .reg_rd(issue_rd && selected==1),.reg_addr(read_addr_q),.reg_wr_addr(e1_addr_q),
 .reg_wdata(e1_data_q),
 .reg_rdata(r1),.bank3_in(gb_data_in),.bank2_in(8'd0),
 .bank3_out(p1_bank3_out),
 .bank3_dir(p1_bank3_dir),
 .bank2_out(p1_bank2_out),
 .bank2_dir(p1_bank2_dir),
 .bank1_out(p1_bank1_out),
 .bank1_dir(p1_bank1_dir),
 .bank0_out(p1_bank0_out),
 .bank0_dir(p1_bank0_dir),
 .pin30_out(p1_pin30_out),
 .pin30_dir(p1_pin30_dir),
 .pin30_pwroff_reset(p1_pin30_pwroff_reset),
 .pin31_out(p1_pin31_out),
 .pin31_dir(p1_pin31_dir),.writes_enabled()
);
`endif
chisburner_cart_gb_mbc5_fullrom #(.DISCARD_FIRST_BLOCK(1),
 .MAPPER_AUTO_PROFILE(1),.SHARED_HEADER_PROFILE(1),.SHARED_DUMP_PROFILE(1),.GENERAL_DUMP_PROFILE(1)) engine2 (
 .clk(clk),.reset_n(reset_n && !restart_mapper),.lifecycle_ok(lifecycle_ok),
 .lifecycle_status(lifecycle_status),.host_b1_raw(host_b1_raw),.host_diag(host_diag),
 .reg_wr(engine_wr_q[2] && lifecycle_ok),
 .reg_rd(issue_rd && selected==2),.reg_addr(read_addr_q),.reg_wr_addr(e2_addr_q),
 .reg_wdata(e2_data_q),
 .reg_rdata(r2),.bank3_in(gb_data_in),.bank2_in(8'd0),
 .bank3_out(p2_bank3_out),
 .bank3_dir(p2_bank3_dir),
 .bank2_out(p2_bank2_out),
 .bank2_dir(p2_bank2_dir),
 .bank1_out(p2_bank1_out),
 .bank1_dir(p2_bank1_dir),
 .bank0_out(p2_bank0_out),
 .bank0_dir(p2_bank0_dir),
 .pin30_out(p2_pin30_out),
 .pin30_dir(p2_pin30_dir),
 .pin30_pwroff_reset(p2_pin30_pwroff_reset),
 .pin31_out(p2_pin31_out),
 .pin31_dir(p2_pin31_dir),.writes_enabled()
);
`ifdef CHISBURNER_PROGRAM131
// Stream131 is the only destructive GBA path. Keep legacy selection 3 for
// header reads/analysis, but remove the superseded S29-only writer/FIFO.
chisburner_cart_gba_readonly #(
 .READS_ENABLED(1),.ADDRESS_SETUP_CYCLES(12),.SELECT_SETUP_CYCLES(6),
 .TURNAROUND_CYCLES(3),.DATA_WAIT_CYCLES(18),.RELEASE_CYCLES(6),
 .MAX_READ_BYTES(32),.READ_WINDOW_BYTES(32'h02000000),
 .REQUIRED_READ_BYTES(32),
`ifdef CHISBURNER_GBA_IDENTIFY133
 .IDENTIFY_ENABLED(0)
`else
 .IDENTIFY_ENABLED(1)
`endif
) engine3 (
`else
chisburner_cart_gba_s29gl256 engine3 (
`endif
 .clk(clk),.reset_n(reset_n && !reset_gba),.lifecycle_ok(lifecycle_ok),
 .lifecycle_status(lifecycle_status),.host_b1_raw(host_b1_raw),.host_diag(host_diag),
 .reg_wr(engine_wr_q[3] && lifecycle_ok),
 .reg_rd(issue_rd && selected==3),.reg_addr(engine_wr_q[3] ? e3_addr_q : read_addr_q),
 .reg_wdata(e3_data_q),
 .reg_rdata(r3),.bank3_in(bank3_in),.bank2_in(bank2_in),
 .bank3_out(p3_bank3_out),
 .bank3_dir(p3_bank3_dir),
 .bank2_out(p3_bank2_out),
 .bank2_dir(p3_bank2_dir),
 .bank1_out(p3_bank1_out),
 .bank1_dir(p3_bank1_dir),
 .bank0_out(p3_bank0_out),
 .bank0_dir(p3_bank0_dir),
 .pin30_out(p3_pin30_out),
 .pin30_dir(p3_pin30_dir),
 .pin30_pwroff_reset(p3_pin30_pwroff_reset),
 .pin31_out(p3_pin31_out),
 .pin31_dir(p3_pin31_dir),.writes_enabled(gba_writes_enabled)
`ifdef CHISBURNER_PROGRAM131
 ,.busy(),.profile_verified()
`endif
);
endmodule
`default_nettype wire
