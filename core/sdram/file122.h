/* SPDX-License-Identifier: Apache-2.0 */
#ifndef CHISBURNER_FILE122_H
#define CHISBURNER_FILE122_H
#include "driver117.h"
/* Boot-diagnostic only. Buffers must be exclusively owned and uncached.
 * No reset/force-release is attempted on failure. Caller latches failure. */
struct cb_sd122_io {
 struct cb_sd117_io mmio;
 int (*transfer)(void *, int write, uint32_t slot, uint32_t offset,
                 uint32_t bridge_address, uint32_t bytes);
};
static inline int cb_sd122_lock(const struct cb_sd117_io *io,int acquire,uint32_t polls) {
 if(io->read(io->ctx,0)!=CB_SD116_SIGNATURE) return CB_SD117_UNAVAILABLE;
 uint32_t s=io->read(io->ctx,24);
 if(s&28u) return CB_SD117_FAULT;
 if(!(s&(acquire?1u:2u))) return CB_SD117_BUSY;
 io->write(io->ctx,20,acquire?1u:2u);
 while(polls--) {
  s=io->read(io->ctx,24);
  if(s&28u) return CB_SD117_FAULT;
  if(s&(acquire?2u:1u)) return 0;
 }
 return CB_SD117_TIMEOUT;
}
/* Optional phase output is diagnostic only; it never changes bus operations. */
enum { CB_SD122_VALIDATE=0, CB_SD122_ACQUIRE=1, CB_SD122_PREPARE=2,
 CB_SD122_UNLOCK=3, CB_SD122_APF=4, CB_SD122_REACQUIRE=5,
 CB_SD122_RELEASE=6, CB_SD122_FINAL_UNLOCK=7, CB_SD122_DONE=8 };
static inline int cb_sd122_transfer_trace(const struct cb_sd122_io *io,int write,
 uint32_t slot,uint32_t offset,uint32_t address,uint32_t bytes,uint32_t polls,
 uint32_t *phase) {
 if(phase) *phase=CB_SD122_VALIDATE;
 if(!io||!io->mmio.read||!io->mmio.write||!io->transfer||!polls||
    (write!=0&&write!=1)||!bytes||bytes>32768u||(address&63u)||
    address<0x10000000u||address>=0x14000000u||
    ((bytes+63u)&~63u)>0x14000000u-address||bytes>UINT32_MAX-offset)
  return CB_SD117_RANGE;
 const struct cb_sd117_io *m=&io->mmio;
 if(phase) *phase=CB_SD122_ACQUIRE;
 int rc=cb_sd122_lock(m,1,polls);
 if(rc) return rc;
 if(write) {
  if(phase) *phase=CB_SD122_PREPARE;
  rc=cb_sd117_prepare(m,address,bytes,polls);
  if(rc) return rc;
 }
 /* Never wait for a new APF command with admission blocked. */
 if(phase) *phase=CB_SD122_UNLOCK;
 rc=cb_sd122_lock(m,0,polls);
 if(rc) return rc;
 if(phase) *phase=CB_SD122_APF;
 rc=io->transfer(m->ctx,write,slot,offset,write?0x21000000u:address,bytes);
 if(rc) return rc; /* host may still be active: retain snapshot */
 if(phase) *phase=CB_SD122_REACQUIRE;
 rc=cb_sd122_lock(m,1,polls);
 if(rc) return rc;
 if(write) {
  if(phase) *phase=CB_SD122_RELEASE;
  rc=cb_sd117_release(m,polls);
  if(rc) return rc;
 }
 if(phase) *phase=CB_SD122_FINAL_UNLOCK;
 rc=cb_sd122_lock(m,0,polls);
 if(!rc&&phase) *phase=CB_SD122_DONE;
 return rc;
}
static inline int cb_sd122_transfer(const struct cb_sd122_io *io,int write,
 uint32_t slot,uint32_t offset,uint32_t address,uint32_t bytes,uint32_t polls) {
 return cb_sd122_transfer_trace(io,write,slot,offset,address,bytes,polls,0);
}
#endif
