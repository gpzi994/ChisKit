ChisBurner — smoke.50 固定文件 A/B 对照

这是 TF 文件写入/提交的诊断包，不操作卡带。卡槽保持空置。
不是 FLUSH 已修复的声明；需要你在 Pocket 2.6 上给出结果。

安装（只复制一次）
1. 将 SD_ROOT 内的文件夹合并到 TF 根目录，允许覆盖本项目的同名文件。
2. 完全关机再开机，进入 ChisBurner，选择 TF Static 50.json。
   路径：Assets/chisburner/ChisBurner.ChisBurner/TF Static 50.json
3. 屏幕必须显示 TF STATIC smoke.50 和 OS STATIC50 OK。
   显示旧编号：应用没有覆盖成功。WRONG OS：os.bin 没有覆盖成功。
   合并覆盖不会删除旧测试入口；不要再选 49 或更早的入口。

测试顺序
1. 首次启动自动只读 512 字节，应显示：
   BOOT READ RC 0
   BOOT CONTENT: INITIAL
   此时没有写入。按 A：写入 A 图样、直接读回、比较全部 512 字节。
   期望 WRITE512 RC 0 / READ512 RC 0 / BYTE MATCH 512/512 PASS。
   FLUSH NOT ISSUED 是 A 测试的预期行为。
2. 拍摄 A 结果页。按 Analogue 键正常 Quit 退出核心，再完全关机/开机。
   不重新复制任何文件！打开同一个 TF Static 50.json。
   请报告启动页 BOOT CONTENT 是否为 A VERIFIED；这一步检验重启后内容。
3. 在这次新启动中按 B：写入不同的 B 图样、读回逐字节比较，再发一次 FLUSH。
   FLUSH 可能仍需等待约 50 秒并返回 -12。无论成败，请拍摄结果页；
   按右键再拍 FLUSH OBSERVATION 页（总共只有两页）。
4. 正常 Quit 后完全关机/开机，不重新复制文件，报告 BOOT CONTENT 是否为 B VERIFIED。
   若任一步 STOP、读写失败或 UNKNOWN，先发该屏幕，不要继续操作。

说明
- A、B 使用同一静态绑定文件，相同大小、相同 WRITE/READ 流程；B 只多 FLUSH。
  两个不同的图样用来区分重启后保留的是哪轮数据。每字节比较，不只看返回码。
- 初始文件为 Assets/chisburner/ChisBurner.ChisBurner/static50.bin（512 字节）。
  重新覆盖它会恢复 INITIAL，破坏本轮持久性验证，所以两次重启中不要再解压覆盖。
- 正常 Quit 加关机后的校验，不等于拔电/崩溃一致性测试。不要在写入时强制断电。
- 一次启动只允许一次 A 或 B。任何超时后不重试文件命令，使用正常退出重新开始。
- 应用不发动态 OPEN、不扩容、不修改槽大小表；也不读写卡带。
- 保留 SDK/OS 路线。OS 只替换现有 output_open 服务函数所占的固定代码区，
  提供版本查询和受限的静态绑定：只接受 static50.bin、slot4、512 字节。
  它只设置原有 WRITE/FLUSH 服务的 RAM 状态，不发 OPEN、不创建文件。
  不是正式的动态 output API；不要混用其他版本的诊断应用。
- WRITE、READ、FLUSH 的底层代码保持旧版；FPGA、Chip32 loader、启动代码不变。
- slot4 为可写、core-specific、deferload、非 nonvolatile，禁止运行中手动换文件；
  不添加无地址的自动回存。slot4/5/6 的扩展名列表缩减到官方允许的范围。
- 原有 storage-smoke.41.bin 和 storage-seed46.bin 不放入本包，也不删除 TF 上旧文件。
- A 阶段没有 OPEN，因此原记录器的 OPEN 窗口不适用；只显示 B 的 FLUSH 窗口。
  TX 完成只说明 FPGA 内部发送结束，不等于主机已正确解码。

依据（官方资料）
https://github.com/open-fpga/core-example-kbmouse-targetdata
  采用静态可写 deferred slot 的直接 WRITE/READ 作为对照思路；没有移植独立核心。
https://www.analogue.co/developer/docs/core-definition-files/data-json
https://www.analogue.co/developer/docs/host-target-commands

本地验证
SDK RV32 应用编译；静态绑定边界检查、A/B 顺序、512 字节全部位置损坏检测、
异步返回/回调/超时/越界/迟到回调测试；ASan/UBSan；实际链接的 RV32 补丁指令
执行、ABI 保持、外部调用/内存写白名单；OS 变更范围和内部 CRC；ZIP 所有文件哈希；
UI 最宽 40 列、最高 18 行。主机解码/电气波形仿真未执行，实机结果待测。
