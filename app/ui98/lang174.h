#ifndef CB_LANG174_H
#define CB_LANG174_H
#include <string.h>
struct cb_translation174 {const char *en,*zh;};
static const struct cb_translation174 cb_translations174[]={
 {"Theme","主题"},{"Dark","深色"},{"Light","浅色"},
 {"Settings saved","设置已保存"},
 {"Up/Down Select; Left/Right Change","上下选择，左右切换"},
 {"GB RTC","GB 时钟"},
 {"RTC halted","时钟已暂停"},
 {"RTC running","时钟运行中"},
 {"Backup retained; RTC follows SAV","已保留备份，存档恢复后更新时钟"},
 {"Restore GB RTC and verify","恢复 GB 时钟并校验"},
 {"GB SAV and RTC restored / verified","GB 存档和时钟已恢复并校验"},
 {"SAV restored; GB RTC failed; keep backup","存档已恢复，时钟失败，请保留备份"},
 {"Backup SAV only","仅备份存档"},
 {"Backup SAV including RTC","备份存档和时钟"},
 {"Up/Down: Choose  A: OK  B: Back","上下选择  A确认  B返回"},
 {"Backup cancelled","已取消备份"},
 {"Checking GB RTC","检查 GB 时钟"},
 {"Include GB RTC with backup?","备份是否包含 GB 时钟？"},
 {"Read GB RTC for backup","读取 GB 时钟用于备份"},
 {"Write and verify GB RTC footer","写入并校验 GB 时钟附加数据"},
 {"GB SAV + 48-byte RTC verified","GB 存档及时钟附加数据校验通过"},
 {"GB RTC adds 48 bytes to SAV","GB 时钟在存档尾部增加 48 字节"},
 {"RTC detected and readable","检测到可读取的时钟"},
 {"A: SAV including RTC","A：存档包含时钟"},
 {"B: SAV only","B：仅备份存档"},
 {"Restore MBC7 EEPROM","恢复 MBC7 EEPROM 存档"},
 {"Read original MBC7 EEPROM twice","重复读取原始 MBC7 存档"},
 {"Restore MBC7 EEPROM and compare","恢复 MBC7 存档并完整校验"},
 {"MBC7 restored; full compare PASS","MBC7 存档恢复及完整校验通过"},
 {"MBC7 unchanged; full compare PASS","MBC7 存档相同；完整校验通过"},
 {"GBA Cartridge Info","GBA 卡带信息"},{"GB/GBC Cartridge Info","GB/GBC 卡带信息"},{"Cartridge Info","卡带信息"},
 {"ROM title","游戏名称"},{"Code/Maker","代码/厂商"},{"Revision","版本"},{"Logo","标志"},
 {"Header sum","头校验"},{"ROM sum","ROM 校验"},{"ROM size","ROM 容量"},{"Save","存档"},
 {"NOR model","NOR 型号"},{"NOR size","NOR 容量"},{"Profile","配置"},{"Not computed","未计算"},
 {"Nintendo / valid","Nintendo / 有效"},{"Write limit","写入范围"},{"Buffer","缓冲大小"},
 {"Region 0","区域 0"},{"Region 1","区域 1"},{"Region 2","区域 2"},{"Region 3","区域 3"},
 {"Start","起始地址"},{"Cmd set","命令集"},{"Write","写入"},{"Probe / Plan","探测/方案"},
 {"Test / fresh ID required","写入前复核 ID"},{"Disabled","禁用"},{"Not confirmed","未确认"},
 {"NOR capacity selected for backup","按 NOR 容量备份"},{"Analyze cartridge","请分析卡带"},
 {"Mori / normal","Mori / 标准"},{"Unconfirmed","未确认"},
 {"Header OK / NOR analysis pending","头校验通过；待分析 NOR"},
 {"Up/Down NOR  A Analyze    B Functions","上下 NOR  A分析  B功能"},
 {"Up/Down Regions A Analyze B Functions","上下区域  A分析  B功能"},
 {"Up/Down Info  A Analyze  B Functions","上下信息  A分析  B功能"},
 {"Transfer Status","传输状态"},{"No active task","暂无任务"},{"Analyze cartridge first","请先分析卡带"},
 {"Progress","进度"},{"Elapsed","已用时间"},{"No transfer running","当前没有传输任务"},
 {"L/R Pages              B Functions","L/R翻页  B返回功能"},
 {"Keep power on; do not remove cart","请保持供电，勿拔出卡带"},
 {"Busy: refresh / removal not allowed","传输中：请勿刷新或拔卡"},
 {"Year","年"},{"Month","月"},{"Day","日"},{"Hour (24h)","小时（24时制）"},{"Minute","分"},{"Second","秒"},
 {"Left/Right field; Up/Down value","左右选择项目；上下调整数值"},
 {"X: Use Pocket current time","X：使用 Pocket 当前时间"},{"A: Review   B: Cancel","A：确认预览  B：取消"},
 {"CHANGES CARTRIDGE CLOCK","将修改卡带时钟"},{"Save data / ROM unchanged","不修改存档及 ROM"},
 {"A: Write RTC   B: Edit again","A：写入时钟  B：继续编辑"},
 {"RTC detected and readable","已检测到可读取的时钟"},{"A: SAV including RTC","A：备份存档并附带时钟"},
 {"B: SAV only","B：仅备份存档"},{"RTC adds 16 bytes to SAV","时钟在存档尾部追加 16 字节"},
 {"Restore SAV only","仅恢复存档"},{"Restore SAV + original RTC","恢复存档及原始时钟"},
 {"Restore SAV + elapsed RTC","恢复存档并补偿经过的时间"},{"Cancel restore","取消恢复"},
 {"Up/Down: choose RTC handling","上下：选择时钟处理方式"},{"A: Select   B: Cancel all","A：选择  B：全部取消"},
 {"SAV first, then update RTC","先恢复存档，再更新时钟"},{"Game ROM and RTC unchanged","不修改游戏 ROM 和时钟"},
 {"A: Confirm save restore","A：确认恢复存档"},{"Release A before confirming","请先松开 A 再确认"},
 {"B: Cancel save restore","B：取消恢复存档"},{"Press A again to confirm","再次按 A 确认"},
 {"Release first A before confirming","请先松开 A 再确认"},{"B: Cancel without starting","B：取消，不执行操作"},
 {"ERASES ALL 32 MiB OF CARTRIDGE","将擦除卡带全部 32 MiB 内容"},
 {"Reset","重置"},{"Analyze Cart","分析卡带"},{"Backup ROM","备份 ROM"},
 {"Backup Save","备份存档"},{"Write ROM","写入 ROM"},{"Write SAV","恢复存档"},
 {"Write Save","恢复存档"},{"Cart Info","卡带信息"},{"Options","设置"},
 {"Platform: Auto","平台：自动"},{"Detecting...","正在检测"},{"Not analyzed","尚未分析"},
 {"Ready","就绪"},{"No cartridge","未检测到卡带"},
 {"Header verified","卡带头校验通过"},{"No valid header","未发现有效卡带头"},
 {"Read unstable","读取不稳定"},{"Header invalid","卡带头无效"},
 {"Read failed","读取失败"},{"Select Play Cartridge","请选择运行卡带"},
 {"Check SAV","检查存档"},{"Prep SAV","准备存档"},
 {"D-pad Select  A OK  X RTC settings","方向键选择  A确认  X时钟"},
 {"Analyze NOR before write; backup is read only","写入前请分析卡带；备份只读"},
 {"Analyze GBA cartridge first","请先分析 GBA 卡带"},
 {"Language","语言"},{"Save type settings","存档类型设置"},
 {"Up/Down Select; Left/Right language","上下选择；左右切换语言"},
 {"A Apply/Open   B Cancel","A保存或进入  B取消"},
 {"Language saved","语言已保存"},{"Language save failed","语言保存失败"},
 {"Save type / Left-Right","存档类型：左右选择"},{"Automatic","自动"},
 {"ROM / RTC settings unchanged","不修改 ROM 或时钟设置"},
 {"A Apply   B Cancel","A应用  B取消"},{"Reset clears selection","重置会清除手动选择"},
 {"Unknown","未知"},{"Hardware","硬件检测"},{"Database reference","数据库参考"},
 {"Flashcart default","烧录卡默认"},{"Manual selection","手动选择"},
 {"I/O error: analyze again","读取错误：请重新分析"},{"Conflicts with hardware","与硬件检测结果冲突"},
 {"No supported write backend","此类型尚不支持写入"},{"Confirm type before restore","恢复前请确认类型"},
 {"Existing hardware path","使用已检测的硬件类型"}
};
static inline const char *cb_translate174(const char *s){
 for(unsigned i=0;i<sizeof cb_translations174/sizeof *cb_translations174;i++)
  if(!strcmp(s,cb_translations174[i].en))return cb_translations174[i].zh;
 return s;
}
/* Consume malformed UTF-8 safely; unsupported codepoints render a box. */
static inline unsigned cb_utf174(const unsigned char **p){
 unsigned c=*(*p)++;if(c<128)return c;
 unsigned n=c>=0xc2&&c<=0xdf?1:c>=0xe0&&c<=0xef?2:c>=0xf0&&c<=0xf4?3:0;
 if(!n)return 0xfffd;
 unsigned v=c&((1u<<(6-n))-1),min=n==1?128:n==2?2048:65536;
 for(unsigned i=0;i<n;i++){unsigned b=**p;if((b&0xc0)!=0x80)return 0xfffd;(*p)++;v=(v<<6)|(b&63);}
 return v<min||v>0x10ffff||(v>=0xd800&&v<=0xdfff)?0xfffd:v;
}
#endif
