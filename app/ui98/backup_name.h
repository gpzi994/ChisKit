#ifndef CB_BACKUP_NAME_H
#define CB_BACKUP_NAME_H
/* Header strings are untrusted: only portable filename characters survive. */
static void cb_name_part(char *out,unsigned cap,const char *in){
 unsigned n=0;
 while(*in&&n+1<cap){unsigned char c=(unsigned char)*in++;
  out[n++]=((c>='A'&&c<='Z')||(c>='a'&&c<='z')||
            (c>='0'&&c<='9')||c=='-'||c=='_')?(char)c:'_';
 }
 out[n]=0;
}
static void cb_backup_name(char *out,unsigned cap,const struct cb_cart *cart,
                           const char *ext,uint32_t stamp){
 char title[17],code[9];cb_name_part(title,sizeof(title),cart->title);
 cb_name_part(code,sizeof(code),cart->code);
 snprintf(out,cap,"/Saves/chiskit/common/%s-%s%s%s-%08lx.%s",
  cart->platform==CB_GBA?"GBA":"GB",title[0]?title:"UNKNOWN",
  code[0]?"-":"",code,(unsigned long)stamp,ext);
}
#endif
