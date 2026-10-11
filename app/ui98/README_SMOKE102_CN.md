# ChisBurner smoke.102 定位测试

此包用于定位 smoke.101 的 MBC3 analysis rc -4 和 GBA backup rc -2，
不宣称故障已修复。OS、FPGA 二进制与 smoke.101 完全相同；GBA 擦除、
烧写算法未修改，GB NOR 写入、存档和 RTC 写入仍锁定。

把 SD_ROOT 内容合并到 SD 卡根目录。进入核心后确认右上角 smoke.102。
请在备份重要文件后更新核心文件；不要删除已有 Saves 目录。

1. 插入带 RTC 的 MBC3 烧录卡，刷新并分析。拍摄 NOR 详情页，重点保留
   Probe、Stopped at、Analysis 和底部状态。再刷新、分析一次。
2. 换 MBC5 重复分析，确认未发生回退。换卡时不得有任务正在执行。
3. 用 GBA 卡运行 Backup ROM，仍需两次 A 确认。若失败，拍完整传输页。
   Checking output file end 失败表示新文件末尾读取失败，还没开始备份写入。
   Writing backup output 失败会显示请求偏移与长度，以及 S/O/N/T 原始寄存器。
   S=文件槽，O=偏移，N=长度，T=状态，均为十六进制。寄存器在清理前采样。
4. 若备份成功，应完成全部 32768 KiB 和文件回读 MD5+CRC32；仅创建文件、
   读取末尾或达到进度 100% 都不等于最终校验通过，以 Result PASS 为准。

新的备份名称为 /Saves/chisburner/common/cb102-时间标记.gba。
本版不会删除失败后留下的不完整备份，也不会覆盖已有同名文件。
本轮无需重复擦除或烧写卡带。请勿把采样相同直接视为硬件一定是 MBC5。
