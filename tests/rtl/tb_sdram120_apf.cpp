// SPDX-License-Identifier: Apache-2.0
#define main unused_upstream_main
#include "../../.integration/openfpgaos/src/fpga/test/tb_core_bridge_cmd_main.cpp"
#undef main
int main(int argc,char **argv){
 Verilated::commandArgs(argc,argv);tb=new Vtb_core_bridge_cmd;
 tb->storage_block_in=0;init_inputs();
 tb->storage_block_in=1;tb->target_dataslot_flush_in=1;
 for(int i=0;i<20;i++)tick();
 /* FLUSH has no bridge payload and must bypass the SDRAM admission lock.
  * This is specifically required after an SDRAM-backed DS_WRITE: the CPU
  * can observe final unlock before its bridge-domain blocked flag settles. */
 check_eq("blocked payload-less flush launched",saw_target_cmd(0x636d0188),1);
 check_eq("launched flush reports active",tb->storage_busy_out,1);
 tb->storage_block_in=0;
 check_eq("queued command retained",wait_target_cmd(0x636d0188),1);
 check_eq("target command busy",tb->storage_busy_out,1);
 tb->storage_block_in=1;ack_target_cmd();
 check_eq("active command drains despite block",tb->target_dataslot_done_out,1);
 tb->target_dataslot_flush_in=0;for(int i=0;i<8;i++)tick();
 check_eq("target command drained",tb->storage_busy_out,0);
 for(unsigned cmd: {0x80u,0x82u}){
  host_cmd(cmd);check_eq("blocked host rejected",read32(HOST_CMD)&0xffff,2);
  check_eq("rejected host not busy",tb->storage_busy_out,0);
  tb->storage_block_in=0;host_cmd(cmd);
  check_eq("host accepted",read32(HOST_CMD)&0xffff,0);
  check_eq("host busy after acceptance",tb->storage_busy_out,1);
  tb->storage_block_in=1;for(int i=0;i<30;i++)tick();
  check_eq("host busy retained until allcomplete",tb->storage_busy_out,1);
  host_cmd(0x8f);check_eq("host allcomplete drains",tb->storage_busy_out,0);
 }
 tb->storage_block_in=0;
 test_dataslot_flush_command();test_ready_reissued_after_warm_reset();
 test_reset_enter_clears_readonly_datatable_only();
 printf("SDRAM120 APF: %d passed, %d failed\n",passes,fails);
 delete tb;return fails?1:0;
}
