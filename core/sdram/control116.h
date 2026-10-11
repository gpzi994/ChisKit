/* SPDX-License-Identifier: Apache-2.0 */
#ifndef CHISBURNER_SDRAM_CONTROL116_H
#define CHISBURNER_SDRAM_CONTROL116_H
/* Relative offsets only. No live MMIO base is assigned or enabled yet. */
#define CB_SD116_SIGNATURE_OFFSET 0x00u
#define CB_SD116_STATUS_OFFSET    0x04u
#define CB_SD116_SOURCE_OFFSET    0x08u
#define CB_SD116_BYTES_OFFSET     0x0cu
#define CB_SD116_COMMAND_OFFSET   0x10u
#define CB_SD116_SIGNATURE        0x53443116u
#define CB_SD116_TRANSPORT_READY  (1u << 0)
#define CB_SD116_IDLE             (1u << 1)
#define CB_SD116_SNAPSHOT         (1u << 2)
#define CB_SD116_LOCKED           (1u << 3)
#define CB_SD116_REJECTED         (1u << 4)
#define CB_SD116_TIMEOUT          (1u << 5)
#define CB_SD116_INGRESS_DRAINED  (1u << 6)
#define CB_SD116_HOST_QUIET        (1u << 7)
#define CB_SD116_AXI_IDLE          (1u << 8)
#define CB_SD116_PREPARE           1u
#define CB_SD116_RELEASE           2u
#define CB_SD116_CLEAR_REJECT      4u
/* Deliberately no local reset/force-release command. */
#endif
