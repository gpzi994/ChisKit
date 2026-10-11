#ifndef CB_RTC167_UI_H
#define CB_RTC167_UI_H
#include "of_rtc167_data.h"
#include <stdio.h>
#include <string.h>
static void cb_rtc167_format(char *s,unsigned n,int rc,const of_rtc167_report *r){
 if(rc){snprintf(s,n,"Read failed rc %d",rc);return;}
 if(r->version!=167){snprintf(s,n,"RTC service unavailable");return;}
 if(!(r->flags&OF_RTC167_GPIO)){snprintf(s,n,"Not detected");return;}
 if(r->status&0x15){snprintf(s,n,"GPIO / RTC unconfirmed");return;}
 if(r->flags&OF_RTC167_POWER_FLAG){snprintf(s,n,"RTC power flag / invalid");return;}
 if(!(r->status&0x40)){snprintf(s,n,"RTC 12h / see raw log");return;}
 if(!(r->flags&OF_RTC167_TIME_OK)){snprintf(s,n,"RTC date invalid");return;}
 snprintf(s,n,"20%02X-%02X-%02X %02X:%02X:%02X",r->date[0],r->date[1],r->date[2],r->date[4]&0x3f,r->date[5],r->date[6]);
}
#endif
