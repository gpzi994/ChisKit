ChisBurner — smoke.54：ROM 导出分阶段计时 + 32 KiB TF 分块

这是待实机验证的新包；不能把 smoke.53 的实机通过当作本包已通过。
适用：已写入指定“逆转裁判.gba”的 Chisflash GBA 卡。
只读卡带 ROM；不读写存档，不烧写/擦除卡带，不切换电压。

本包变化
1. TF 分块由 4096 增至 32768 字节：2126 块变为 266 块。
   265 个完整块 + 最后 21760 字节；文件仍为 8705280 字节，绝不裁剪或填充。
2. 仍保留每块写入后的完整回读比较、卡带全 ROM MD5、独立 TF 全文件 MD5。
3. FPGA 位流与 smoke.53 逐字节相同；卡带总线时序、32 字节 FIFO、
   ROM 地址边界、272045 次硬件读取预算、安全空闲和写禁止均不变。
   SDK/OS 卡带服务与底层 TF 读写实现不变；OS 只更新输出名和版本识别。
4. 没有擅自放大卡带读取命令：硬件现有上限为 32 字节。
   本次减少的是 TF 调用次数；具体加速幅度需实机计时，不能承诺 8 倍。
5. 独立输出 dump54.gba。不包含 dump53.gba，因此安装本包不会覆盖它。
6. 继续不调用 FLUSH；此前 -12 未修复，也没有将错误改成成功。

安装
将本包 SD_ROOT 内全部内容合并到 TF 根目录，同名文件替换。
这会更新核心配置、os.bin 和应用；不要只复制 ELF。
首次需一起复制全零占位文件 dump54.gba；本包不含原始 ROM 或 SAV 内容。
保留已成功的 dump53.gba，勿再复制旧 smoke.53 包的全零占位文件覆盖它。

运行
1. 完全关机后插入 Chisflash，开机进入：
   主界面 → openFPGA → ChisBurner → Play Cartridge。
   不是主界面直接运行游戏的 Play Cartridge，也不选择旧 TF 测试 JSON。
2. 必须显示 GBA DUMP smoke.54；等待 BOOT TF SCAN 完成。
   首次应为 BOOT CONTENT: ZERO SEED，TF WRITE 0 CART READ 0。
   若仍显示 smoke.53 或 WRONG OS，说明覆盖不完整或入口不对，先停止。
3. 松开按键半秒 → L+R 按住 2 秒 → 松开半秒 → 按 A → 松开半秒 →
   START（+）按住 1 秒确认。B 取消。每次开机仅一轮，不自动重试。
4. 进度现在是 266 块，不是 2126；每块 32 KiB。
   进度每 8 块刷新一次；最后还要独立重读 TF 全文件计算 MD5。
   测试中保持供电，不要拔卡。整段导出限时 15 分钟，单次 TF 读取 5 秒；
   启动/最终全文件扫描各限时 3 分钟。限时检查在相应操作边界执行。

请发两页结果
第一页：ROM DUMP PASS RC 0、BLOCKS 266/266、BYTES 8705280/8705280，
SAMPLES 5/5 SAFE RC 0；卡带与 TF 的 MD5 均应为：
07E851B1DC49009EBD00982514EBFE92
按方向键左/右，第二页为 TIMING PAGE 2/2；请拍下这页的全部计时。
A 回第一页；结果页按键不会再次启动导出。

计时含义（单位都是毫秒 MS）
CART READ：五个识别窗口和完整卡带 ROM 读取，含 SDK 调用及轮询等待。
TF WRITE：TF 写入调用总耗时。
BLOCK READBACK：逐块 TF 回读调用总耗时（含回调等待、搬运和边界检查）。
HASH+COMPARE：运行中的卡带 MD5 和逐块字节比较耗时。
SAFE IDLE：返回安全空闲的调用耗时。
FINAL TF MD5：最终独立 TF 扫描，包含其读取、MD5、画面刷新。
OTHER / UI：识别窗口 CRC、配置、画面刷新、计时等剩余开销。
DUMP TOTAL：从导出确认后进入作业，到最终校验和安全空闲结束。
BOOT (OUTSIDE)：进入程序后的全文件扫描，不计入 DUMP TOTAL。
计时范围不重叠；计时只在阶段/TF 块边界进行，没有每次 32 字节额外读时钟。
smoke.53 的 431 秒基准同样不含启动扫描。不同 TF 状态仍会影响比较。

正常退出后的持久化复核
先拍两页结果，再正常 Quit 退出核心，完全关机/开机，重新进入相同入口。
不要重新复制 dump54.gba！等待 BOOT CONTENT: ROM VERIFIED，MD5 同上。
这只验证正常退出关机后的保留情况，不保证突然断电安全。
输出路径：Assets/chisburner/ChisBurner.ChisBurner/dump54.gba
包内 Compare-ROM54.ps1 可在电脑上只读检查源文件与导出文件的长度及 MD5。

失败处理
请发结果和计时两页，特别是 STAGE、RC、STOP OFFSET。
任何读写失败或比较不符都会停止；部分文件不是有效备份。
程序不会覆盖非零且 MD5 不符的输出，也不会重写已验证的 ROM。
不要自行循环重试；先保留结果和部分文件。成功后再覆盖整个本包会清零 dump54.gba。

验证边界
本地运行 SDK 编译、源 ROM/MD5 向量、完整 266 块流程、DMA/回调/短尾边界、
故障立即停止、最终校验故障、计时分账/时钟回绕、实际链接的 OS 服务机器码
和沿用 RTL 的完整读取预算测试；这些不能替代 Pocket 固件/TF/实物卡带测试。
原始 ROM 和 32768 字节 SAV 均不修改。卡带存档后续单独测试。
官方协议依据：https://www.analogue.co/developer/docs/host-target-commands
READ/WRITE 的参数含 slot、offset、bridge address、length；FLUSH 是独立命令。
