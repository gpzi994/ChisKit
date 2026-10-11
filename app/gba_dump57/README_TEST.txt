ChisBurner — smoke.57：32 MiB 绿宝石识别后精确导出

这是待实机验证的新包。适用对象仅为当前写入指定汉化绿宝石的 ChisFlash：
[1MSRAM][FTC]口袋妖怪 - 宝石_中文典藏版[漫游&TGB联合汉化].gba
源文件长度 33554432 字节，MD5：872A1A05E39006F3BEA69EFAA641B3A4。

安全边界
1. 启动先重复读取并核对 GBA 头部，再查内容数据库，再执行一次固定 NOR ID/CFI 探测。
2. 只有 BPEE 数据库命中、数据库逻辑大小 16 MiB、且硬件确认为兼容
   S29GL256N 的 32 MiB NOR 时，才允许进入本包的 32 MiB 导出流程。
3. 插入正版 Wario AWAC 时应识别为数据库 8 MiB、Generic ROM、无受支持 NOR，
   然后在任何 TF 写入前停止；请勿用本包导出 Wario。
4. 卡带始终只读：无卡带程序/擦除 API，无 SAVE/RTC 访问，无电压切换。
   NOR 探测只含固定复位/ID/CFI/复位序列，不含程序或擦除操作码。
5. 不调用 FLUSH；此前观察到的 FLUSH RC -12 没有被伪装成成功。

安装
将本包 SD_ROOT 内全部内容合并到 TF 根目录，同名文件全部替换。
必须复制整个包，不能只复制 ELF、os.bin 或 rbf_r。
首次必须一起复制 32 MiB 全零占位文件 dump57-32m.gba。
包内不含原始 ROM 或 SAV 内容；该占位文件仅为全零。

运行
1. 完全关机，插入当前绿宝石 ChisFlash，开机进入：
   主界面 -> openFPGA -> ChisBurner -> Play Cartridge。
2. 必须看到 GBA DUMP smoke.57。程序会先识别卡带，再扫描 slot 4。
   首次应显示：
   CONTENT BPEE DB 16 MiB
   HARDWARE S29GL256N 32 MiB
   DUMP PLAN 32 MiB
   BOOT CONTENT: ZERO SEED
3. 确认无误后：松开全部按键 0.5 秒 -> L+R 按住 2 秒 -> 松开 0.5 秒 ->
   按 A -> 松开 0.5 秒 -> START（+）按住 1 秒。
4. 导出期间保持供电，不拔卡、不退出。共有 1024 个 32 KiB TF 块；
   卡带总线仍按每次 32 字节读取，命令总预算恰好 1048605 次。
   最后还会独立重读 TF 全部 32 MiB，不要在进度到 1024 后立即关机。

成功画面
RESULT PAGE 1/2 应显示：ROM DUMP PASS RC 0、BYTES 33554432/33554432、
BLOCKS 1024/1024、SAMPLES 5/5、SAFE RC 0，卡带与 TF 的 MD5 均为：
872A1A05E39006F3BEA69EFAA641B3A4
SOURCE PREFIX CRC 应为 A03DC885；数据库原版参考 CRC 1F1C08FB 只用于识别
BPEE 条目，不等于这份汉化 ROM 的实际前 16 MiB CRC。

请拍 RESULT PAGE 1/2；再按左/右到 TIMING PAGE 2/2，拍下全部计时。
完成后正常 Quit，完全关机再开机，重新进入本核心但不要再次确认导出。
应显示 BOOT CONTENT: ROM VERIFIED，并禁止重写。请再拍这一页。

电脑复核
输出路径：Assets/chisburner/ChisBurner.ChisBurner/dump57-32m.gba
包内 Compare-ROM57.ps1 可只读比较导出文件与 Downloads 中指定源 ROM 的长度、
MD5、SHA256；脚本不会修改文件。

失败处理
出现 IDENTIFICATION STOP、UNKNOWN、任何 RC 非 0、STOP OFFSET 或 MD5 不符时，
停止并拍完整画面；不要循环重试，不要把部分文件当作备份。
如果仍显示旧 smoke 编号、WRONG OS 或 bridge not responding，说明整包未正确覆盖，
先不要继续。重新覆盖本包会重新复制全零占位文件，因此会清除已有 dump57 输出。

本地验收范围
已完成 SDK 构建、真实 32 MiB 源文件全流程及故障门测试、实际链接 OS 服务测试、
1,048,605 次 RTL 完整读取预算、32 MiB 边界、安全引脚、识别一次性限制、
Quartus 全编译和正 setup/hold 时序、OS 四段补丁白名单、RBF 位反转往返及 ZIP 校验。
这些不能代替 Pocket、卡带与 TF 的本次实机验证。

