/* SPDX-License-Identifier: Apache-2.0 */
#ifndef CHISBURNER_SDRAM_DRIVER117_H
#define CHISBURNER_SDRAM_DRIVER117_H
#include <stdint.h>
#include "control116.h"
/* Relative MMIO offsets. Caller serializes operations, cleans caches and
 * fences before prepare. No live base address or reset is assumed here. */
struct cb_sd117_io {
 void *ctx;
 uint32_t (*read)(void *, uint32_t);
 void (*write)(void *, uint32_t, uint32_t);
};
enum { CB_SD117_OK=0, CB_SD117_UNAVAILABLE=-1, CB_SD117_BUSY=-2,
 CB_SD117_FAULT=-3, CB_SD117_TIMEOUT=-4, CB_SD117_RANGE=-5 };
static inline int cb_sd117_wait(const struct cb_sd117_io *io,uint32_t mask,uint32_t polls) {
 while(polls--) {
  uint32_t s=io->read(io->ctx,4);
  if(s&(CB_SD116_LOCKED|CB_SD116_REJECTED|CB_SD116_TIMEOUT)) return CB_SD117_FAULT;
  if((s&mask)==mask) return CB_SD117_OK;
 }
 /* Timeout must not force-release or reset outstanding transactions. */
 return CB_SD117_TIMEOUT;
}
static inline int cb_sd117_prepare(const struct cb_sd117_io *io,uint32_t address,uint32_t bytes,uint32_t polls) {
 if(!io||!io->read||!io->write||!polls) return CB_SD117_RANGE;
 if(address<0x10000000u||address>=0x14000000u||(address&3u)||
    !bytes||bytes>32768u||bytes>0x14000000u-address) return CB_SD117_RANGE;
 if(io->read(io->ctx,0)!=CB_SD116_SIGNATURE) return CB_SD117_UNAVAILABLE;
 uint32_t s=io->read(io->ctx,4);
 if(s&(CB_SD116_LOCKED|CB_SD116_TIMEOUT|CB_SD116_REJECTED)) return CB_SD117_FAULT;
 const uint32_t need=CB_SD116_IDLE|CB_SD116_TRANSPORT_READY|
  CB_SD116_INGRESS_DRAINED|CB_SD116_HOST_QUIET|CB_SD116_AXI_IDLE;
 if((s&need)!=need) return CB_SD117_BUSY;
 io->write(io->ctx,8,address);io->write(io->ctx,12,bytes);
 io->write(io->ctx,16,CB_SD116_PREPARE);
 return cb_sd117_wait(io,CB_SD116_SNAPSHOT,polls);
}
static inline int cb_sd117_release(const struct cb_sd117_io *io,uint32_t polls) {
 if(!io||!io->read||!io->write||!polls) return CB_SD117_RANGE;
 if(io->read(io->ctx,0)!=CB_SD116_SIGNATURE) return CB_SD117_UNAVAILABLE;
 uint32_t s=io->read(io->ctx,4);
 if(s&(CB_SD116_LOCKED|CB_SD116_REJECTED|CB_SD116_TIMEOUT)) return CB_SD117_FAULT;
 const uint32_t need=CB_SD116_SNAPSHOT|CB_SD116_HOST_QUIET|
  CB_SD116_AXI_IDLE|CB_SD116_INGRESS_DRAINED;
 if((s&need)!=need) return CB_SD117_BUSY;
 io->write(io->ctx,16,CB_SD116_RELEASE);
 return cb_sd117_wait(io,CB_SD116_IDLE,polls);
}
#endif
