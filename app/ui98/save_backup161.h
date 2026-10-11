#ifndef CB_SAVE_BACKUP161_H
#define CB_SAVE_BACKUP161_H
static inline int cb_eeprom161_supported(int type,unsigned bytes){
 return (type==1&&bytes==512)||(type==2&&bytes==8192);
}
#endif
