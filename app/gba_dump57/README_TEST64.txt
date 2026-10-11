ChisBurner - smoke.64 中文测试说明

一、版本目的

本包在已经通过实机验证的 smoke.57 卡带读取路径和 smoke.62 动态创建
文件功能上，修改了 dump 数据流程：

1. 卡带 ROM 仍然严格按每次 32 字节同步读取。
2. TF 写入和最终 TF 扫描改为每块 64 KiB。
3. 删除“每写一块立即回读并逐字节比较”的流程。
4. CRC32 从逐位循环改为 256 项逐字节查表。
5. dump 卡带时持续计算卡带 MD5、完整 CRC32 和前 16 MiB CRC32；
   写完后完整扫描一次 TF 文件，并再次计算相同校验值进行比较。

本包不会附带 32 MiB 全零占位文件。只有卡带识别全部通过后，程序才会
动态创建输出文件。

二、本包只支持当前这张卡和这个 ROM

[1MSRAM][FTC]口袋妖怪 - 宝石_中文典藏版[漫游&TGB联合汉化].gba
文件大小：33554432 字节（32 MiB）
MD5：872A1A05E39006F3BEA69EFAA641B3A4

卡带必须识别为：

- 游戏代码 BPEE
- 数据库 ROM 大小 16 MiB
- S29GL256N 兼容 NOR
- NOR 物理容量 32 MiB
- 最终 dump 计划 32 MiB

三、安全边界

1. 程序先重复读取并检查 GBA 头部、查询 BPEE 数据库记录，再执行一次
   固定的 NOR ID/CFI 探测。
2. 只有 BPEE、S29GL256N 兼容 NOR 和 32 MiB 物理容量全部通过，程序
   才会打开或创建 TF 输出文件。
3. 识别完成前，不会打开、创建、调整大小或写入目标 TF 文件。
4. 卡带 ROM 只读；不会访问存档或 RTC；本构建不提供卡带编程、擦除、
   电压切换接口。
5. 不调用 APF 0x0188 FLUSH。之前的 smoke.62 已确认，通过正常退出并
   冷启动后，动态写入的数据可以持久保存。

四、安装

把压缩包中 SD_ROOT 目录里的全部内容合并复制到 TF 卡根目录，覆盖同名
文件。必须复制整个包，不能只复制 ELF、OS 或 RBF 中的某一个文件。

本包故意不包含以下输出文件：

  /Saves/chisburner/common/dump57-32m.gba

输出文件名仍叫 dump57-32m.gba，是因为已经通过实机验证的 OS 把 slot 4
硬性绑定到这个名称；文件名中的 57 不表示仍在运行 smoke.57。

五、首次测试前必须处理旧文件

如果 TF 卡上已经存在以下文件，请关机、取出 TF 卡，然后在电脑上把它
移动到备份目录或删除：

  /Saves/chisburner/common/dump57-32m.gba

只处理这个文件，不要删除其他文件。

如果保留的是之前已经校验正确的 dump，smoke.64 会先完整扫描它并显示
ROM VERIFIED，然后禁止重写；这样无法测试新的 64 KiB dump 流程。

六、首次运行

1. Pocket 完全关机。
2. 插入当前这张 BPEE ChisFlash 卡带和准备好的 TF 卡。
3. 开机后进入：

   主菜单 -> openFPGA -> ChisBurner -> Play Cartridge

4. 确认屏幕标题为：

   GBA DYNAMIC DUMP smoke.64

5. 随后应看到：

   CONTENT BPEE DB 16 MiB
   HARDWARE S29GL256N 32 MiB
   DUMP PLAN 32 MiB
   OPEN HOST 1  TF SIZE 33554432
   TF STATE: NEW FILE - SCAN SKIPPED

这些内容表示：程序已经先完成卡带识别，然后才动态创建最终大小为
32 MiB 的输出文件；首次运行不会扫描 32 MiB 全零占位文件。

6. 按以下顺序解锁并启动一次 dump：

   - 松开所有按键 0.5 秒；
   - 按住 L+R 2 秒；
   - 松开所有按键 0.5 秒；
   - 按一下 A；
   - 松开所有按键 0.5 秒；
   - 按住 START（+）1 秒。

7. 开始后不要断电，不要拔卡带，也不要拔 TF 卡。

运行期间：

- 卡带仍按每次 32 字节读取；
- TF 每次写入 64 KiB；
- 32 MiB 一共写入 512 块；
- 每块写完后不再立即回读；
- 全部写完后，程序只进行一次完整 TF 扫描验证。

七、成功结果

结果第 1 页应显示：

  ROM DUMP PASS RC 0
  STAGE COMPLETE
  BYTES 33554432/33554432
  BLOCKS 512/512
  SAMPLES 5/5 SAFE RC 0
  STOP OFFSET: NONE

卡带 MD5 和 TF MD5 都必须是：

  872A1A05E39006F3BEA69EFAA641B3A4

使用左/右方向键切换到计时第 2 页。首次动态创建并成功 dump 时，计数
通常应为：

  TF W 512 R 512
  TF 64 KiB; CART 32 B
  BLOCK READBACK  DISABLED

这里的 TF R 512 是最后一次完整 TF 验证产生的 512 次读取，不是逐块
写后回读。

请拍摄结果第 1 页和计时第 2 页。

八、冷启动持久性检查

成功后正常退出核心，Pocket 完全关机，再重新开机进入 smoke.64。
程序会先识别卡带，然后打开已经存在的输出文件（HOST 0）并完整扫描。

预期显示：

  TF STATE: ROM VERIFIED
  NO REWRITE; DYNAMIC PERSISTENCE PASS

这一步不要再次启动 dump。请拍摄该页面。

九、在电脑上只读校验

把 TF 卡接入电脑，运行包内的 Compare-ROM64.ps1，并用 -DumpPath 指定：

  /Saves/chisburner/common/dump57-32m.gba

脚本会只读检查源 ROM 和 dump 的文件大小、MD5、SHA256，不会修改任何
一个文件。

示例：

  .\Compare-ROM64.ps1 -DumpPath "X:\Saves\chisburner\common\dump57-32m.gba"

把 X: 替换为 TF 卡实际盘符。

十、失败处理

出现以下任一情况时，请停止操作、拍摄完整屏幕，不要连续重试：

- IDENTIFICATION STOP
- DYNAMIC OPEN STOP
- INVALID
- 任意非零 RC
- 文件大小不符合预期
- 出现 STOP OFFSET
- MD5 或 CRC 不一致

部分写入或校验失败的 dump57-32m.gba，必须关机后在电脑上移动或删除，
才能进行下一次有意安排的重试。

十一、后续待办，不属于 smoke.64

1. Verify Cartridge：单独选择后，再完整读取一次卡带和 TF 文件并逐字节
   比较。
2. 异步卡带读取：作为后续 2.0/3.0 数据流水线功能研究。

