#include "analysis.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static int safes,fail_safe,probes,available=1,probe_rc;
static of_nor_info_t fixture;
static int safe(void*x){(void)x;return ++safes==fail_safe?-7:0;}
static int old_flash(void*x,of_cart_flash_caps_t*f){(void)x;(void)f;assert(0);return -1;}
static int read_old(void*x,unsigned b,unsigned s,uint32_t a,void*p,unsigned n){
 (void)x;(void)b;(void)s;(void)a;(void)p;(void)n;assert(0);return -1;
}
static int caps(void*x,of_cart_caps_t*c){(void)x;(void)c;assert(0);return -1;}
static int dynamic_available(void){return available;}
static int dynamic_probe(uint32_t bus,of_nor_info_t*n){assert(bus==2);probes++;*n=fixture;return probe_rc;}
#define of_nor_available dynamic_available
#define of_nor_probe dynamic_probe
#include "../app/ui98/nor108_analysis.h"
static struct cb_analysis run(void){
 struct cb_cart c={.platform=CB_GBA,.result=CB_READY};struct cb_analysis a;
 struct cb_analysis_ops ops={0,read_old,safe,old_flash,caps};
 safes=probes=0;cb_analyze(&a,&c,&ops);analyze_nor108(&a,&c);
 assert(!a.legacy_programmer);return a;
}
int main(void){
 fixture.struct_size=sizeof(fixture);fixture.version=OF_NOR_VERSION;fixture.bus=2;
 fixture.flags=OF_NOR_ID_OK|OF_NOR_WRITE_OK|OF_NOR_CFI_OK;
 fixture.flash_bytes=0x2000000;fixture.buffer_bytes=512;strcpy(fixture.model,"S29GL256S");
 struct cb_analysis a=run();assert(!a.rc&&a.writable&&a.matched&&probes==1&&safes==2&&a.header_ok);
 strcpy(fixture.model,"JS28F256");fixture.buffer_bytes=1024;
 a=run();assert(!a.rc&&a.writable&&a.buffer==1024&&!strcmp(a.model,"JS28F256"));
 probe_rc=OF_ERR_NOT_SUPPORTED;a=run();assert(!a.rc&&!a.writable&&!a.matched&&probes==1);
 probe_rc=-7;a=run();assert(a.rc==-7&&!a.writable);
 probe_rc=0;available=0;a=run();assert(a.rc==OF_ERR_NOT_SUPPORTED&&!a.writable&&!probes);
 available=1;fail_safe=1;a=run();assert(a.rc==-7&&!probes&&!a.writable);
 fail_safe=2;a=run();assert(a.rc==-7&&!probes&&!a.writable);
 fail_safe=0;fixture.plan_rc=-2;a=run();assert(!a.writable&&a.nor_rc==-2);
 fixture.plan_rc=0;fixture.flash_bytes=0x4000000;a=run();assert(!a.writable);
 fixture.flash_bytes=0x2000000;fixture.version=0;a=run();assert(a.rc&&!a.writable);
 fixture.version=OF_NOR_VERSION;
 struct cb_cart empty={.platform=CB_GBA,.result=CB_EMPTY};
 a=(struct cb_analysis){.ran=1};probes=0;analyze_nor108(&a,&empty);
 assert(probes==1&&a.writable&&!a.header_ok&&!a.legacy_programmer);
 fixture.flags=OF_NOR_ID_OK;a=run();assert(a.matched&&!a.writable);
 puts("PASS GBA133: dynamic-only identification, retail, unsupported/missing service, safe failures, geometry/version/capacity gates; no legacy probe");
}
