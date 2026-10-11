smoke.77 S29GL256S 生命周期稳定识别测试

变化
----
1. 不接受 smoke.76 捕获的 LIFE=00000400，也不放宽写入安全门槛。
2. 在只读前置阶段最多等待约 1.5 秒，每 10 ms 重读一次能力结构。
3. 只有精确的 READY/PLAY/POWER/READ_READY 状态连续两次一致
   才继续协议、PING 和 NOR ID/CFI 检查。
4. ABI、BUS、OPS、WRITE、READ 或 SPACE 任一字段错误都立即停止。
5. 失败页新增尝试次数、连续稳定次数和首末 LIFE 原始值。

测试
----
整体覆盖 SD_ROOT，完全关机后重新开机，运行 Play Cartridge，
选择指定的 8705280 字节《逆转裁判》ROM。

- 如果出现 TARGET IDENTIFY FAIL，请拍下完整屏幕，尤其是 L TRY、
  L FIRST/LAST、PROTO/PING/SAFE 和 FLASH MASK。
- 如果出现 TARGET IDENTIFIED 或确认界面，先拍照，不要完成
  最后的擦除/烧写确认。

本版未改变破坏性规则：只有全部识别与 ROM 扫描通过、且完成
双重人工确认后，才会按识别到的 32 MiB NOR 全擦 256 个
128 KiB 扇区，然后以 32 字节操作烧写实际 ROM，最后回读 MD5。
