ChisBurner — smoke.52：1 MiB 分块存储测试

目的：在 smoke.51 已验证的静态文件读写基础上，检查大文件偏移、分块写入、全量读回及正常关机重开后的内容。
不调用 FLUSH，不把超时当作成功；FLUSH 的根因仍未确定。本包还不是卡带 ROM dump 包。
只操作独立的 static52.bin（1,048,576 字节），卡槽全程保持空置。

安装（首次）
1. 将 SD_ROOT 内 Assets、Cores、Platforms 合并复制到 TF 卡根目录，允许覆盖同名文件。
   不能只复制 ELF：本包的 common/os.bin、核心 data.json、chisburner52.ini 都必须更新。
   不要把 SD_ROOT 这个文件夹本身放进根目录。无需删除旧测试文件或用户存档。
2. 完全关机再开机，选该核心中的 TF Stream 52.json：
   Assets/chisburner/ChisBurner.ChisBurner/TF Stream 52.json
3. 标题应为 TF STREAM smoke.52。等待 BOOT READ 读完 256 块，期间有进度，不要关机。
   BOOT CONTENT: INITIAL
   BOOT RC 0 BLOCKS 256/256
   BYTES CHECKED 1048576/1048576
   CRC 502B18CD
   HEAD: 46 47 42 58 35 32 3D C2
   WRITE CALLS 0；BIND 0 READ 256；FLUSH NOT USED
   首次安装不是这些结果：停止操作，拍下结果屏，不要解锁或反复重试。

写入与校验（首屏符合上面预期即可继续，无需等另一个包）
4. 松开所有键约 0.5 秒 → 同时按 L+R 两肩键至少 2 秒 → 松开约 0.5 秒 →
   按 A → 松开约 0.5 秒 → 按住 START（+）至少 1 秒确认。
   解锁后 10 秒内完成确认；B 是取消，不是第二种测试模式。
   单独 A/B/START 无法启动写入；每次启动仅允许执行一次，失败不重试。
5. 等待 WRITE 256 块、VERIFY READ 256 块。每块 4096 字节，每 8 块更新进度。
   测试期间不要退出核心、拔卡或断电。单阶段有 120 秒软件预算，单次服务另有超时。
   预期结果：
   STREAM PASS RC 0
   BIND RC 0
   WRITE RC 0 BLOCKS 256/256
   VERIFY RC 0 BLOCKS 256/256
   CHECKED 1048576/1048576 BAD 0
   CRC CC3FA633 EXPECT CC3FA633
   FIRST BAD: NONE
   WRITE CALLS 256 READ 512
   FLUSH: NOT USED
   先拍下这个结果。PASS 此时仅表示本次读回一致，还没有完成重启验证。

持久化验证（仍用本包）
6. 正常退出核心，完全关机再开机，再选 TF Stream 52.json。
   不要重新复制 static52.bin，也不要再次覆盖整个包，否则会重置测试内容。
7. 等 256 块启动只读检查完成，预期：
   BOOT CONTENT: DATA VERIFIED
   BOOT RC 0 BLOCKS 256/256
   BYTES CHECKED 1048576/1048576
   CRC CC3FA633
   HEAD: 46 47 42 58 35 32 A7 58
   WRITE CALLS 0；BIND 0 READ 256；FLUSH NOT USED
   拍下这屏即可。DATA VERIFIED 时应用禁止再次写入。
   若仍为 INITIAL、UNKNOWN 或任何失败，拍屏停止，不要重新写来覆盖失败证据。
   此流程验证正常退出和重启后的内容，不证明写入中突然断电也安全。

错误与覆盖检查
- 标题不是 smoke.52：选错旧入口、未完整覆盖或旧核心会话尚未退出；不要继续测。
- STOP WRONG OS：OS 与应用不配套；完整覆盖本包后冷启动，勿只换 ELF。
- STOP SLOT4 SIZE：slot4 应恰好 1048576 字节；检查本包实例、data.json 和测试文件是否齐全。
- READ/WRITE/VERIFY 任一步失败即停止后续服务；结果屏会保留阶段、返回码和停止字节位置。
  不会自动重试、扩容、创建别的文件或调用 FLUSH；未知/部分文件也不会自动重写。
- 测試入口、应用与配置分别为 TF Stream 52.json、chisburner52.elf、chisburner52.ini，不与旧包共用应用名。
- 本包更新了公共 OS。测试期间只选 52，旧诊断入口不保证兼容；回退需完整恢复旧包。
- 不附带 Saves 文件，不访问卡带，不切换电压。

构建与验证边界
- FPGA、加载器、启动代码与 smoke.51 相同。OS 仅替换静态槽位绑定服务区域
  0xF180..0xF3FF，以及最后 4 字节内部 CRC；长度和其他字节不变。
- 新服务仅接受 static52.bin/1MiB 参数及已绑定的 1MiB slot4，不发主机 OPEN/扩容/数据槽回写。
- 数据包含各 4KiB 块的编号和基于绝对偏移的图样；逐字节比较全部 1MiB，并检查 CRC32。
  可检测常见块重复、64KiB 回绕及首尾偏移错误，不只是比较文件头。
- 本地测试包括 ASan/UBSan、写入锁/单次授权、偏移和故障注入、回调令牌与 DMA 边界、
  编译后绑定服务的 RV32 指令/ABI/访存白名单、SDK 编译、OS CRC、ZIP 全载荷 SHA256。
- UI_FIXTURES_SYNTHETIC.txt 是排版用合成输出，不是 Pocket 实机测试；本地测试不模拟
  Analogue 主机实际文件系统或电气时序。本包实机结论等待你的两张结果屏。
