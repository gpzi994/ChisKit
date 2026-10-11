ChisBurner — smoke.53：Chisflash 完整 GBA ROM dump

这是实机待验证的测试包，不是已经在 Pocket 上确认通过的版本。
适用：已经写入指定“逆转裁判.gba”的 Chisflash GBA 卡。
只读取卡带 ROM；不操作卡带存档，不烧写/擦除卡带，不切换电压。
程序只允许写入本包独立的 TF 输出文件 dump53.gba。

安装与运行
1. 首次安装：把 SD_ROOT 内全部内容合并到 TF 卡根目录，同名文件全部替换。
   必须一起覆盖核心位流、core.json、data.json、os.bin 和新 ELF/INI。
   不要只覆盖 ELF。本包没有原始游戏或存档；dump53.gba 是全零占位文件。
2. Pocket 完全关机后插入 Chisflash，开机进入：
   openFPGA → ChisBurner → Play Cartridge。
   不选旧的 TF Stream 52 / TF Guard 51 / 其他旧 JSON。
3. 屏幕必须显示 GBA DUMP smoke.53。先等待 BOOT TF SCAN 完成：
   BOOT CONTENT: ZERO SEED；BYTES 8705280/8705280；TF WRITE 0 CART READ 0。
   CART LIFE 应为 ready 状态。若 STOP CART CAPS，请拍屏，不要换旧包反复试。
4. 松开所有按键半秒；L+R 按住 2 秒；松开半秒；按 A；松开半秒；
   START（+）按住 1 秒确认。B 取消。每次开机只授权一轮，没有自动重试。
5. 等待进度完成，期间不要拔卡或断电。先检查五个 ROM 特征窗口，
   再读完整 ROM；每个 TF 块写完立即读回比较，最后重新扫描 TF 全文件。
   总计 2126 块，最后一块 1280 字节。运行可能数分钟；以进度为准。

本轮成功标准
ROM DUMP PASS RC 0
BYTES 8705280/8705280 BLOCKS 2126/2126
SAMPLES 5/5 SAFE RC 0
CART MD5 与 TF MD5 均为：
07E851B1DC49009EBD00982514EBFE92

持久化复核
正常 Quit 退出核心，再完全关机/开机，重新进同一个 Play Cartridge。
不要重新覆盖 dump53.gba！程序会只读校验它，应显示：
BOOT CONTENT: ROM VERIFIED，TF WRITE 0 CART READ 0，MD5 同上。
这验证正常退出后的持久化，不证明突然断电安全。

输出路径（TF 卡）
Assets/chisburner/ChisBurner.ChisBurner/dump53.gba
可复制到电脑，用包内 Compare-ROM53.ps1 与原文件检查大小及 MD5。
输出必须是 8,705,280 字节；不能截成 8 MiB 或填充成 16/32 MiB。

出错时
请发最终屏幕：STAGE、RC、STOP OFFSET、BYTES、MD5。
失败可能留下部分输出；程序看到非空且 MD5 不符的文件时会拒绝覆盖。
不要把失败/部分输出当成备份。重新测试需先保留失败文件，再按后续指示重置种子。
若屏幕不是 smoke.53，覆盖或启动入口不对；不要继续按确认键。
已经 dump 成功后，再覆盖整个 SD_ROOT 会把 dump53.gba 重置为全零！
请先将结果复制到电脑保存，再考虑安装其他包。

边界与验证说明
本轮不调用 FLUSH；此前 FLUSH -12 未修复，也没有改成假成功。
沿用 smoke.52 已通过实机的静态槽位 TF 写入路径和已知启动加载器。
FPGA 加回 ROM 只读电路；与早期 GBA 探测相同的保守 90 MHz 时序。
硬件限制 32 字节对齐、地址 0..0x0084D4FF、最多 272045 次读取。
WR# 始终非写状态；卡带存档片选不激活。读取出错后停止并回安全空闲。
SDK 应用不直接操作 MMIO，所有访问经过 OS 服务。
本地测试包括：MD5 标准向量与源文件校验；完整读写故障流程；
异步回调/边界检查；实际服务机器码 ABI 检查；完整次数 RTL 仿真。
这些测试不替代 Pocket 固件、TF 卡及真实卡带的综合验证。

参考基准（原文件未修改）
ROM MD5 07E851B1DC49009EBD00982514EBFE92
ROM SHA256 0D0945FFF2EDD4D4AE627ED003EBB0050937EE689D606CCFF89C4FF2C2638571
SAV 32768 字节，MD5 86AE87048C08D4EAE306FE4C4D7E6607，仅留作后续参考。

官方文档核对
https://www.analogue.co/developer/docs/core-definition-files/core-json
  cartridge_adapter bit24：Play Cartridge 入口；此模式跳过 slot0 及其派生文件。
https://www.analogue.co/developer/docs/core-definition-files/data-json
  slot4 parameters=2：独立固定文件名、core-specific、可写。
  deferload：仅先绑定 ID/大小，后续通过 target commands 读写；size_exact 检查长度。
https://www.analogue.co/developer/docs/host-target-commands
  文件 WRITE 与 FLUSH 为不同命令；本包只使用 READ/WRITE。
