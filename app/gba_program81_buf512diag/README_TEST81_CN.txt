smoke.81：512 字节缓冲写能力只读诊断

本版本不会发送解锁、擦除或写入命令，不会改变卡带内容。

测试方法：
1. 将 SD_ROOT 的内容覆盖到 Pocket SD 卡根目录。
2. 使用 Play Cartridge 启动 ChisBurner。
3. 如果第 2 页未显示 ROM READY 1，请在 Core Settings 的 Program ROM
   槽位加载 8,705,280 字节的“逆转裁判.gba”，然后按 A 重新扫描。
4. 用左右键查看五页。重点拍摄第 4、5 页；如有异常，请拍摄全部五页。

通过标准：
- 第 4 页 GEOM 行末显示 B512；
- 第 5 页显示 S29GL256S TARGET PASS；
- 第 5 页显示 ALL READ-ONLY PREFLIGHTS PASS。

确认通过后，下一版本才会执行按识别容量的整片分区擦除，并用 512 字节
缓冲写入 8,705,280 字节测试 ROM（最后一批为 256 字节）。
