#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../app/ui98/ui.h"
#include "of_output.h"
#include "of_gb175.h"
#include "of_input.h"
#include "../app/ui98/backup_name.h"
static struct of_services_table services;
const struct of_services_table *_of_svc_ptr=&services;
of_input_state_t _of_input_p0,_of_input_p1;
static struct cb_ui state,*ui=&state;
static uint8_t buffer[65536],disk[131072];
static uint32_t began;
static unsigned captures,restores,opens,confirms,backup_writes;
static int open_rc,bad_size,bad_source,bad_backup,capture_rc,restore_rc,allow;
static uint32_t now(void){return 1;}
static void paint(void){}
static void input(void *p){memset(p,0,sizeof(of_input_state_t));}
static long size(const char *p){assert(!strcmp(p,"slot:5")||!strcmp(p,"slot:6"));return state.gb_save175_bytes+bad_size;}
static int file_read(unsigned slot,uint32_t off,void *p,uint32_t n){
 assert(slot==6&&off+n<=state.gb_save175_bytes);memset(p,0x39,n);
 if(bad_source&&p==buffer)((uint8_t*)p)[0]^=1;
 return 0;
}
static int backup_read(uint32_t off,void *p,uint32_t n){memcpy(p,disk+off,n);if(bad_backup)((uint8_t*)p)[0]^=1;return 0;}
static int open_file(const char *p,uint32_t n){assert(strstr(p,"-backup.sav")&&n==state.gb_save175_bytes);opens++;return open_rc;}
static int write_file(uint32_t off,const void *p,uint32_t n){backup_writes++;memcpy(disk+off,p,n);return 0;}
static int capture(const uint8_t *h,uint32_t selected,void *p,uint32_t n,void *out,uint32_t z){
 (void)h;assert(selected==n&&z==sizeof(of_gb175_report));captures++;
 of_gb175_report *r=out;r->compared=n;r->rc=capture_rc;memset(p,0x5a,n);return capture_rc;
}
static int confirm(void){confirms++;assert(captures==1&&opens==1);return allow;}
static int restore(const uint8_t *h,const void *original,const void *source,uint32_t n,void *out,uint32_t z){
 (void)h;assert(confirms&&z==sizeof(of_gb175_report));restores++;
 for(unsigned i=0;i<n;i++){assert(((const uint8_t*)original)[i]==0x5a);assert(((const uint8_t*)source)[i]==0x39);assert(disk[i]==0x5a);}
 of_gb175_report *r=out;r->written=n;r->verified=restore_rc?0:n;r->rc=restore_rc;return restore_rc;
}
#include "../app/ui98/gb_save175_transfer.h"
#include "../app/ui98/gb_restore180_transfer.h"
static void init(void){
 memset(&state,0,sizeof state);state.cart.platform=CB_GB;state.cart.result=CB_READY;
 state.gb_save175_ready=1;state.gb_save175_bytes=131072;strcpy(state.cart.title,"ZELDA");
 memcpy(state.gb_save175_header+4,cb_gb_logo,48);state.gb_save175_header[0x47]=0x1b;state.gb_save175_header[0x49]=3;
 for(unsigned i=0x34;i<=0x4c;i++)state.gb_save175_header[0x4d]-=state.gb_save175_header[i]+1;
 captures=restores=opens=confirms=backup_writes=0;memset(disk,0,sizeof disk);
 open_rc=bad_size=bad_source=bad_backup=capture_rc=restore_rc=0;allow=1;
}
int main(void){
 services.magic=OF_SVC_MAGIC;services.count=1000;services.input_poll_p0=input;services.file_size=size;
 services.output_open=open_file;services.output_write=write_file;services.gb179_capture=capture;services.gb180_restore=restore;
 (void)backup_gb175;
 init();assert(cb_action_enabled(ui,5));assert(!restore_gb180()&&restores==1&&state.done==131072);
 init();open_rc=1;assert(!restore_gb180()&&restores==1&&backup_writes==4);
 init();open_rc=1;memset(disk,0x5a,sizeof disk);assert(!restore_gb180()&&restores==1&&!backup_writes);
 init();open_rc=-77;assert(restore_gb180()==-77&&!restores&&!confirms);
 init();bad_size=16;assert(restore_gb180()==OF_ERR_BAD_RANGE&&!captures);
 init();bad_source=1;assert(restore_gb180()==OF_ERR_IO&&!captures);
 init();bad_backup=1;assert(restore_gb180()==OF_ERR_IO&&!restores&&!confirms);
 init();capture_rc=-77;assert(restore_gb180()==-77&&!opens);
 init();allow=0;assert(restore_gb180()==OF_ERR_DENIED&&!restores);
 init();restore_rc=-77;assert(restore_gb180()==-77&&strstr(ui->notice,"partial"));
 init();ui->cart.platform=CB_GBA;assert(restore_gb180()==OF_ERR_NOT_SUPPORTED&&!captures);
 init();ui->gb_save175_ready=0;assert(!cb_action_enabled(ui,5)&&restore_gb180()==OF_ERR_NOT_SUPPORTED);
 init();ui->gb_save175_bytes=512;assert(!cb_action_enabled(ui,5)&&restore_gb180()==OF_ERR_NOT_SUPPORTED);
 init();ui->gb_save175_bytes=512;ui->gb_save175_header[0x47]=6;ui->gb_save175_header[0x49]=0;
 ui->gb_save175_header[0x4d]=0;
 for(unsigned i=0x34;i<=0x4c;i++)ui->gb_save175_header[0x4d]-=ui->gb_save175_header[i]+1;
 assert(cb_action_enabled(ui,5)&&!restore_gb180()&&restores==1&&ui->done==512);
 captures=restores=opens=confirms=backup_writes=0;open_rc=1;
 assert(!restore_gb180()&&restores==1&&!backup_writes);
#if CB_GB_RESTORE185
 init();ui->gb_save175_bytes=256;
 memcpy(ui->gb_save175_header+0x34,"KORO2 KIRBYKKKJ",15);
 ui->gb_save175_header[0x47]=0x22;ui->gb_save175_header[0x48]=5;ui->gb_save175_header[0x49]=0;
 ui->gb_save175_header[0x4d]=0;
 for(unsigned i=0x34;i<=0x4c;i++)ui->gb_save175_header[0x4d]-=ui->gb_save175_header[i]+1;
 assert(cb_action_enabled(ui,5)&&!restore_gb180()&&restores==1&&ui->done==256);
 assert(strstr(ui->task,"MBC7")&&strstr(ui->notice,"MBC7"));
 captures=restores=opens=confirms=backup_writes=0;open_rc=1;
 assert(!restore_gb180()&&restores==1&&!backup_writes);
 ui->gb_save175_bytes=512;captures=0;
 assert(!cb_action_enabled(ui,5)&&restore_gb180()==OF_ERR_NOT_SUPPORTED&&!captures);
#endif
 init();services.gb180_restore=NULL;assert(restore_gb180()==OF_ERR_NOT_SUPPORTED&&!captures);
 puts("PASS GB180 app: exact file size, double source read, verified fixed backup/+1 reuse, confirm gate, failure messages, GBA isolation");
 return 0;
}
