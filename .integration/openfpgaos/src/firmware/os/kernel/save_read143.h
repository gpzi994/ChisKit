/* One owner/session; only the existing read-only bus4 byte helper is used. */
static int save_read143_run(of_save143_readonly *out,of_save144_readonly *scan,of_save145_banks *banks){
 memset(out,0,sizeof(*out));out->first_round=out->first_address=UINT32_MAX;
 if(busy){out->rc=CB_NOR_IO;return out->rc;}
 busy=true;job.authorized=false;job_token=0;mapper_kind=0;
 int rc=enter(2);
 if(!rc&&(CART_CAPS&0x00010008u)!=0x00010008u)rc=CB_NOR_UNSUPPORTED;
 if(banks&&!rc){
  for(unsigned b=0;b<2&&!rc;b++){
   rc=banks->bank_rc[b]=sram138_bank(b);
   for(unsigned a=0;a<65536&&!rc;a++){
    uint8_t value=0;rc=save139_read(NULL,a,&value);
    if(!rc){banks->scanned[b]++;banks->sum[b]+=value;if(a<32)banks->head[b][a]=value;}
   }
  }
  /* Never send recovery commands to a removed/replaced cartridge. */
  banks->restore_rc=alive(NULL)?sram138_bank(0):CB_NOR_CANCELLED;
  if(!rc)rc=banks->restore_rc;
 }
 if(scan)for(unsigned a=0;a<65536&&!rc;a++){
  uint8_t value=0;rc=save139_read(NULL,a,&value);
  if(!rc){scan->scanned++;scan->sum+=value;if(a<32)scan->head[a]=value;}
 }
 for(unsigned p=0;p<64&&!rc;p++){
  for(unsigned i=0;i<32&&!rc;i++)rc=save139_read(NULL,i,&out->block[p][i]);
  if(!rc)rc=save139_read(NULL,4,&out->single[p]);
  if(rc)break;
  out->completed++;
  for(unsigned i=0;i<33;i++){
   unsigned a=i==32?4:i;uint8_t v=i==32?out->single[p]:out->block[p][i];
   if(v!=out->block[0][a]){
    if(!out->differences){out->first_round=p;out->first_address=a;}
    out->differences++;
   }
  }
 }
 out->rc=rc;out->cleanup_rc=private_owned?leave():0;busy=false;
 if(scan&&out->completed)for(unsigned i=0;i<32;i++)scan->head_diff+=scan->head[i]!=out->block[0][i];
 if(banks&&out->completed)for(unsigned i=0;i<32;i++)banks->head_diff+=banks->head[0][i]!=out->block[0][i];
 return rc?rc:out->cleanup_rc;
}
static int save_read143_kernel(of_save143_readonly *out){return save_read143_run(out,NULL,NULL);}
static int save_read144_kernel(of_save144_readonly *out){
 memset(out,0,sizeof(*out));return save_read143_run(&out->sample,out,NULL);
}
static int save_read145_kernel(of_save145_banks *out){
 memset(out,0,sizeof(*out));out->bank_rc[0]=out->bank_rc[1]=out->restore_rc=-100;
 return save_read143_run(&out->sample,NULL,out);
}
