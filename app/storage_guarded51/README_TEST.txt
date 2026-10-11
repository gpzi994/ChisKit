ChisBurner — smoke.51：独立文件与写入确认

本轮先只检查启动首屏，不必解锁，不需要挂载 TF 卡给开发端。
旧 smoke.50 未主动按 A 却显示 A VERIFIED 的原因尚未证实。
本包用于把文件内容、重复读取、启动误触分开观察，不宣称已修复 FLUSH。

安装与第一次检查
1. 保持卡槽空。将 SD_ROOT 内的 Assets、Cores、Platforms 合并复制到 TF 卡根目录，允许覆盖同名核心配置。
   不要把整个 SD_ROOT 文件夹放到根目录。无需删除任何旧测试文件或存档。
2. 完全关机再开机，在该核心文件列表选择 TF Guard 51.json，勿选旧的 49/50 项。
   完整路径：Assets/chisburner/ChisBurner.ChisBurner/TF Guard 51.json
3. 进入后松开所有按钮，先拍一张首屏。不要同时按 L+R，不要解锁。
   应看到：
       TF GUARD smoke.51
       BOOT READ 0 / 0 REPEAT OK
       BOOT CONTENT: INITIAL
       CRC 3B942626 / 3B942626
       H0/H1: 46 47 42 58 35 31 3C C3
       MATCH I/A/B 512/6/6 OF512
       WRITE CALLS 0  FLUSH 0
       BIND 0 READ 2
       WRITE LOCK: LOCKED
4. 若首屏已为 A DATA/B DATA/UNKNOWN、读错误、两次读取不一致或写入次数非零，停止操作并拍首屏。
   即使显示 A DATA，也仅表示启动读回的 512 字节匹配 A 图样，不能单凭它确定数据来源或 FLUSH 成功。

覆盖不生效/选错入口的判断
- 标题不是 smoke.51：可能仍在旧核心会话、选择了旧入口、复制目录层级不对或漏覆盖配置；不要继续测。
- 本包应用为 common/chisburner51.elf，配置为 common/chisburner51.ini，测试文件为核心目录下 static51.bin。
  不复用旧 chisburnersmoke.elf/ini 或 static50.bin。本包不会附带旧的 Saves 测试文件。
- 重新复制本包的 static51.bin 会把该专用 512 字节测试文件重置为 INITIAL；
  后续做关机重开持久化检查时，不要再复制这个文件，否则证据被重置。

后续写入操作（首屏确认正常后再进行）
所有步骤只操作专用 static51.bin 的 512 字节，不访问卡带。
启动只读两次，不自动绑定写入服务，不自动 WRITE 或 FLUSH。
单独 A/B/START、长按启动 A，都不能解锁写入。
解锁流程：松开所有键约 0.5 秒 → 同时按 L、R 两肩键至少 2 秒 → 松开约 0.5 秒 →
选择 A 或 B → 松开约 0.5 秒 → 按住 START（+）至少 1 秒确认。
解锁后 10 秒内未完成流程会重新上锁；过早松开 START 或错误组合会取消。

A 模式：绑定静态槽位的 OS 内存状态 → WRITE512 → READ512 → 逐字节比较；不发 FLUSH。
B 模式：同上；仅在写入、读回及 512 字节比较均成功后发一次 FLUSH。
任何错误立即中止后续步骤；每次启动最多一组写入，无重试。
结果页左/右翻页，A 回摘要。结果页显示 SEQUENCE PASS 仍不等于关机后文件已经持久化。
持久化检查需正常退出核心，完全关机再开机，选 51 并记录首屏 A DATA/B DATA 或其他结果。
两次启动之间不要重新复制 static51.bin。若 FLUSH 超时，仅记录返回码和第二页观测，勿反复重试。

图样标记（均校验全 512 字节，而非只比对文件头）
INITIAL: CBRN51 3C C3，CRC32 3B942626
A DATA : CBRN51 A6 59，CRC32 920617F3
B DATA : CBRN51 59 A6，CRC32 354037A6
H0/H1 是两次 SDK 读取缓冲区的前 8 字节；CRC 是两次读回数据的 CRC32。
读取返回错误时这些缓冲区值不是有效文件内容；写入一直被禁用。
WRITE/FLUSH/BIND/READ 是本应用服务调用计数，不是主机实际落盘次数。

不变部分与验证边界
- OS、FPGA、加载器、显示时序等二进制均与 smoke.50 逐字节一致；此次不改启动链。
- 新程序复用已验证的静态槽位服务；内部 static50.bin 参数只是旧服务的授权令牌，
  该服务不按名称 OPEN 文件，仅绑定已由实例选择的 slot 4，实际文件为 static51.bin。
- 无卡带访问，无电压切换，无主机 OPEN/扩容，无数据槽大小回写。
- 本地检查包含：SDK RV32 编译、C 地址/未定义行为检查、按键序列/单次权限/故障中止、
  双读门槛、全 512 字节图样破坏测试、原读回辅助函数回归、OS CRC 和 ZIP 全文件 SHA256。
- UI_FIXTURES_SYNTHETIC.txt 是排版用合成数据，不是 Pocket 实机结果。
  本地测试不模拟 Analogue 主机固件实际的文件系统/电气时序；实机结果待用户反馈。
