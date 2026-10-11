#ifndef CB_SAVE_LOG148_H
#define CB_SAVE_LOG148_H
#include "of_save139.h"
#include <stdio.h>
/* Pure formatting; no bus operation, retry or recovery. */
static int cb_save148_format(char *out,size_t cap,const char *title,int rc,const of_save141_diagnostic *d){
 int n=snprintf(out,cap,
  "chiskit dev.149-auto\nROM=%.32s\nrc=%d type=%ld bytes=%lu basis=%lu\n"
  "stage=%lu id=%04lX id_read=%lu\nprobe_rc=%ld cleanup_rc=%ld direct_rc=%ld restore_rc=%ld leave_rc=%ld\n"
  "original=%02lX target=%02lX observed=%02lX restored=%02lX\n"
  "exit_address=%08lX expected=%02lX actual=%02lX\n"
  "stage: 1=enter 2=header 3=special 4=bank0 5=bank1 6=bank-restore 7=byte-probe 8=flash-id 9=eeprom 10=leave 11=parallel-driver\n"
  "Fields from stages not reached are not evidence of success.\nEND dev.149-auto\n",
  title,rc,(long)d->result.type,(unsigned long)d->result.bytes,(unsigned long)d->result.basis,
  (unsigned long)d->stage,(unsigned long)d->id,(unsigned long)d->id_read,
  (long)d->probe_rc,(long)d->cleanup_rc,(long)d->direct_rc,(long)d->restore_rc,(long)d->leave_rc,
  (unsigned long)d->original,(unsigned long)d->target,(unsigned long)d->observed,(unsigned long)d->restored,
  (unsigned long)d->mismatch_address,(unsigned long)d->expected,(unsigned long)d->actual);
 return n<0||(size_t)n>=cap?-1:n;
}
#endif
