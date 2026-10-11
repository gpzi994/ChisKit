// smoke.47: observation only. SDK identity read arms once per FPGA boot.
// OPEN and FLUSH edges start separate accounting windows without pausing them.
// First read / changed TX value per canonical register is logged. Every F8
// request, TX load and write is counted even after the bounded log fills.
// OPENFILE diagnostics additionally retain every read/TX pair from the fixed
// 264-byte CRAM0 parameter window.  This is observation-only: it lets software
// prove which path/flags/size words were queued for the Pocket host.
module pct_bridge_audit #(parameter ADDR_BITS=8) (
    input wire clk_bridge, open_request, flush_request,
    input wire bridge_rd, bridge_wr,
    input wire [31:0] bridge_addr, bridge_wr_data_wire,
    input wire tx_valid, tx_paired, tx_done, tx_abort,
    input wire [31:0] tx_address, tx_current_address, tx_data_wire,
    input wire clk_cpu, reset_cpu, arm_read, freeze_read, data_pop,
    output wire [31:0] status_cpu, count_cpu, data_cpu
);
localparam DEPTH=(1<<ADDR_BITS), HEADER_WORDS=72;
(* ramstyle="M10K, no_rw_check" *) reg [127:0] events[0:DEPTH-1];
(* ramstyle="M10K, no_rw_check" *) reg [31:0] last_tx[0:127];
// One observation-only pipeline stage gives the per-field history a synchronous
// RAM read. Every input (including phase edges) is delayed together. Nothing in
// the actual command/serializer path waits for or consumes this pipeline.
reg open_q=0, flush_q=0, rd_q=0, wr_q=0, freeze_q=0;
reg tx_valid_q=0, tx_paired_q=0, tx_done_q=0, tx_abort_q=0;
reg [31:0] bridge_addr_q=0, bridge_data_q=0;
reg [31:0] tx_address_q=0, tx_current_q=0, tx_data_q=0;
reg [31:0] previous_tx_ram, previous_tx_bypass_data=0;
reg previous_tx_bypass=0;
reg [127:0] rd_seen=0, tx_seen=0;
reg [31:0] stats[0:63];
integer n;
initial for(n=0;n<64;n=n+1) stats[n]=0;
reg armed=0, frozen=0, full=0, collision=0, time_saturated=0;
reg [1:0] phase=0;
reg [31:0] elapsed=0;
reg [ADDR_BITS:0] used=0;
reg open_prev=0, flush_prev=0;
reg arm_request=0, freeze_request=0;
(* altera_attribute="-name SYNCHRONIZER_IDENTIFICATION FORCED" *)
reg arm_s1=0, arm_s2=0, freeze_s1=0, freeze_s2=0;
wire open_edge=open_q && !open_prev;
wire flush_edge=flush_q && !flush_prev;
wire phase_change=(open_edge && phase==0) || (flush_edge && phase!=2);
wire [1:0] new_phase=flush_edge ? 2'd2 : 2'd1;
wire [1:0] accounting_phase=phase_change ? new_phase : phase;
wire [5:0] base=accounting_phase==2 ? 6'd32 : 6'd0;
wire [31:0] rd_addr=bridge_addr_q & 32'hff00ffff;
wire [31:0] tx_addr=tx_address_q & 32'hff00ffff;
// Exact non-aliasing keys: target 32 words, host 32 words, slot table 64 words.
function [7:0] key_for;
    input [31:0] a;
    begin
        if(a[31:24]!=8'hf8 || a[1:0]!=0) key_for=0;
        else if(a[15:7]==9'h20) key_for={1'b1,2'b00,a[6:2]};
        else if(a[15:7]==0) key_for={1'b1,2'b01,a[6:2]};
        else if(a[15:8]==8'h20) key_for={1'b1,1'b1,a[7:2]};
        else key_for=0;
    end
endfunction
function [31:0] inc_sat;
    input [31:0] v;
    begin inc_sat=&v ? v : v+1'b1; end
endfunction
wire [7:0] rd_key=key_for(rd_addr), tx_key=key_for(tx_addr);
wire [7:0] next_tx_key=key_for(tx_address & 32'hff00ffff);
wire rd_param=bridge_addr_q>=32'h20400000 && bridge_addr_q<=32'h20400107;
wire tx_param=(tx_address_q>=32'h20400000 && tx_address_q<=32'h20400107) ||
              (tx_current_q>=32'h20400000 && tx_current_q<=32'h20400107);
wire rd_f8=rd_q && bridge_addr_q[31:24]==8'hf8;
wire tx_f8=tx_valid_q && (tx_address_q[31:24]==8'hf8 || tx_current_q[31:24]==8'hf8);
wire rd_any=rd_f8 || (rd_q && rd_param);
wire tx_any=tx_f8 || (tx_valid_q && tx_param);
wire wr_any=wr_q && bridge_addr_q[31:24]==8'hf8;
wire history_write=armed && !frozen && !freeze_q &&
                   (phase!=0 || phase_change) && tx_any && tx_key[7] && tx_paired_q;
wire [31:0] previous_tx=previous_tx_bypass ? previous_tx_bypass_data : previous_tx_ram;
always @(posedge clk_bridge) begin
    open_q<=open_request; flush_q<=flush_request;
    // Drain the input stage before declaring the snapshot immutable.
    freeze_q<=freeze_s2;
    rd_q<=bridge_rd; wr_q<=bridge_wr;
    bridge_addr_q<=bridge_addr; bridge_data_q<=bridge_wr_data_wire;
    tx_valid_q<=tx_valid; tx_paired_q<=tx_paired;
    tx_done_q<=tx_done; tx_abort_q<=tx_abort;
    tx_address_q<=tx_address; tx_current_q<=tx_current_address; tx_data_q<=tx_data_wire;
    previous_tx_ram<=last_tx[next_tx_key[6:0]];
    if(history_write) last_tx[tx_key[6:0]]<=tx_data_q;
    // Explicit read-during-write forwarding makes adjacent same-key samples
    // deterministic without relying on device-specific RAM collision behavior.
    previous_tx_bypass<=history_write && next_tx_key==tx_key;
    previous_tx_bypass_data<=tx_data_q;
end
wire rd_event=rd_any && (!rd_key[7] || !rd_seen[rd_key[6:0]]);
wire tx_event=tx_any && (!tx_paired_q || !tx_key[7] || !tx_seen[tx_key[6:0]] || previous_tx!=tx_data_q);
wire [2:0] candidates={2'd0,wr_any}+{2'd0,tx_event}+{2'd0,rd_event}+{2'd0,tx_abort_q};
wire [2:0] lost_now=(used<DEPTH) ? (candidates!=0 ? candidates-1'b1 : 3'd0) : candidates;
reg [31:0] size_id_addr=0;
reg [3:0] event_kind;
reg [31:0] event_addr,event_data,event_next;
always @(*) begin
    event_kind=1; event_addr=bridge_addr_q; event_data=0; event_next=0;
    if(tx_event) begin
        event_kind=tx_paired_q ? 2 : 4; event_addr=tx_address_q;
        event_data=tx_data_q; event_next=tx_current_q;
    end
    if(tx_abort_q) begin
        event_kind=5; event_addr=tx_address_q; event_data=tx_data_q;
        event_next=tx_current_q;
    end
    if(wr_any) begin
        event_kind=3; event_addr=bridge_addr_q; event_data=bridge_data_q; event_next=0;
    end
end
// A single syntactic write port is required for Quartus dual-clock M10K
// inference. All marker/event arbitration is before that one write enable.
wire arm_start=arm_s2 && !armed && !freeze_q;
wire record_write=arm_start || (armed && !frozen && !freeze_q && used<DEPTH &&
                  (phase_change || (phase!=0 && candidates!=0)));
wire [ADDR_BITS-1:0] record_addr=arm_start ? {ADDR_BITS{1'b0}} : used[ADDR_BITS-1:0];
wire [127:0] record_data=arm_start ? {96'd0,4'hf,2'd0,26'd0} :
    phase_change ? {32'd0,(flush_edge?32'h188:32'h192),32'd0,4'he,new_phase,elapsed[31:6]} :
    {event_next,event_data,event_addr,event_kind,phase,elapsed[31:6]};
always @(posedge clk_bridge) begin
    if(record_write) events[record_addr]<=record_data;
end
always @(posedge clk_bridge) begin
    arm_s1<=arm_request; arm_s2<=arm_s1;
    freeze_s1<=freeze_request; freeze_s2<=freeze_s1;
    open_prev<=open_q; flush_prev<=flush_q;
    if(arm_s2 && !armed && !freeze_q) begin
        armed<=1; used<=1;
    end else if(freeze_q) frozen<=1;
    else if(armed && !frozen) begin
        elapsed<=inc_sat(elapsed);
        if(&elapsed) time_saturated<=1;
        if(phase_change) begin
            phase<=new_phase; rd_seen<=0; tx_seen<=0; size_id_addr<=0;
            if(new_phase==1) stats[18]<=elapsed;
            else stats[50]<=elapsed;
            if(used<DEPTH) begin
                used<=used+1'b1;
            end else full<=1;
            // A marker has priority, but simultaneous traffic is not hidden.
            if(rd_any || tx_any || wr_any || tx_abort_q || tx_done_q) collision<=1;
        end
        if(phase!=0 || phase_change) begin
            stats[base+19]<=elapsed;
            if(rd_any) begin
                if(rd_f8) begin
                    stats[base+0]<=inc_sat(stats[base+0]); stats[base+22]<=bridge_addr_q;
                end
                if(rd_key[7]) begin if(!phase_change) rd_seen[rd_key[6:0]]<=1; end
                else if(rd_f8) stats[base+20]<=inc_sat(stats[base+20]);
                if(rd_addr>=32'hf8002000 && rd_addr<=32'hf80020ff) stats[base+3]<=inc_sat(stats[base+3]);
                if(rd_addr==32'hf8001000) stats[base+4]<=inc_sat(stats[base+4]);
                if(rd_addr==32'hf8001020) stats[base+5]<=inc_sat(stats[base+5]);
            end
            if(tx_any) begin
                if(tx_f8) begin
                    stats[base+1]<=inc_sat(stats[base+1]); stats[base+23]<=tx_address_q; stats[base+28]<=tx_data_q;
                end
                if(tx_key[7] && tx_paired_q) begin
                    if(!phase_change) tx_seen[tx_key[6:0]]<=1;
                end
                else if(tx_f8) stats[base+21]<=inc_sat(stats[base+21]);
                if(tx_paired_q && tx_addr==32'hf8001000) begin
                    stats[base+10]<=inc_sat(stats[base+10]); stats[base+12]<=tx_data_q;
                    if(!stats[base+15][1]) stats[base+24]<=tx_data_q;
                    stats[base+15][1]<=1;
                    if(tx_data_q==(accounting_phase==1 ? 32'h636d0192 : 32'h636d0188)) stats[base+11]<=inc_sat(stats[base+11]);
                end
                if(tx_paired_q && tx_addr==32'hf8001020) begin stats[base+13]<=tx_data_q; stats[base+15][0]<=1; end
                if(tx_paired_q && tx_addr>=32'hf8002000 && tx_addr<=32'hf80020ff) begin
                    if(!tx_addr[2] && tx_data_q[15:0]==16'd4) size_id_addr<=tx_addr;
                    if(size_id_addr!=0 && tx_addr==size_id_addr+4) begin stats[base+14]<=tx_data_q; stats[base+15][2]<=1; end
                end
            end
            if(wr_any) begin
                stats[base+2]<=inc_sat(stats[base+2]); stats[base+27]<=bridge_addr_q; stats[base+29]<=bridge_data_q;
                if(rd_addr==32'hf8001000) begin
                    stats[base+6]<=inc_sat(stats[base+6]); stats[base+9]<=bridge_data_q;
                    if(stats[base+6]==0) stats[base+25]<=bridge_data_q;
                    if(bridge_data_q[31:16]==16'h6275) stats[base+7]<=inc_sat(stats[base+7]);
                    if(bridge_data_q[31:16]==16'h6f6b) stats[base+8]<=inc_sat(stats[base+8]);
                end
            end
            // Serializer outcomes cover all address regions in this phase.
            if(tx_valid_q) stats[base+26]<=inc_sat(stats[base+26]);
            if(tx_done_q) stats[base+16]<=inc_sat(stats[base+16]);
            if(tx_abort_q) stats[base+17]<=inc_sat(stats[base+17]);
            if(candidates>1) collision<=1;
            if(lost_now!=0 || (phase_change && candidates!=0)) begin
                if(stats[base+30]>32'hfffffff8) stats[base+30]<=32'hffffffff;
                else stats[base+30]<=stats[base+30]+(phase_change ? {29'd0,candidates} : {29'd0,lost_now});
            end
            if(candidates!=0 && !phase_change) begin
                if(used<DEPTH) begin
                    used<=used+1'b1;
                end else full<=1;
            end
        end
    end
end

// Metadata and RAM writer are immutable before 4-stage frozen ACK is visible.
(* altera_attribute="-name SYNCHRONIZER_IDENTIFICATION FORCED" *)
reg [3:0] frozen_sync=0, armed_sync=0;
reg [ADDR_BITS+2:0] read_word=0;
wire ready=frozen_sync[3];
wire [ADDR_BITS+2:0] event_word=read_word-HEADER_WORDS;
wire [ADDR_BITS+2:0] total_words=HEADER_WORDS+{used,2'b00};
reg [127:0] read_entry;
always @(posedge clk_cpu) begin
    read_entry<=events[event_word[ADDR_BITS+1:2]];
    if(reset_cpu) begin
        arm_request<=0; freeze_request<=0; frozen_sync<=0; armed_sync<=0; read_word<=0;
    end else begin
        if(arm_read) arm_request<=1;
        if(freeze_read) freeze_request<=1;
        frozen_sync<={frozen_sync[2:0],frozen}; armed_sync<={armed_sync[2:0],armed};
        if(data_pop && ready && read_word<total_words) read_word<=read_word+1'b1;
    end
end
wire [31:0] flags={12'd0,time_saturated,armed,collision,full,16'd0};
assign status_cpu={31'd0,ready};
assign count_cpu={armed_sync[3] && !freeze_request,31'd0} | (ready ? (flags|{{(31-ADDR_BITS){1'b0}},used}) : 32'd0);
reg [31:0] header_data;
always @(*) begin
    case(read_word)
        0: header_data=32'h41554437;
        1: header_data=1;
        2: header_data=HEADER_WORDS;
        3: header_data={{(31-ADDR_BITS){1'b0}},used};
        4: header_data=elapsed;
        5: header_data=flags;
        6: header_data={30'd0,phase};
        7: header_data=0;
        default: header_data=stats[(read_word-8)&63];
    endcase
end
assign data_cpu=(!ready || read_word>=total_words) ? 32'd0 :
    read_word<HEADER_WORDS ? header_data :
    read_word[1:0]==0 ? read_entry[31:0] :
    read_word[1:0]==1 ? read_entry[63:32] :
    read_word[1:0]==2 ? read_entry[95:64] : read_entry[127:96];
endmodule
