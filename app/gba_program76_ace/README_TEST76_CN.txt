smoke.76 S29GL256S 烧写前置识别测试

变化
----
1. 将全部目标只读识别放到任何 ROM 槽访问之前。
2. 保留 smoke.74 已验证的卡带能力、协议、PING、ID/CFI、SAFE IDLE 顺序。
3. 如果识别失败，显示 CART RC、CART MASK、原始 ABI/BUS/OPS/WRITE/
   READ/SPACE/LIFE 和 FLASH MASK，不再只显示笼统的 RC -4。
4. 只有识别通过后才检查、扫描 ROM；只有完成双重人工确认后才擦除。

运行
----
整体覆盖 SD_ROOT，冷启动，运行 Play Cartridge，选择指定的 8705280
字节逆转裁判 ROM。若出现 TARGET IDENTIFY FAIL，请拍完整屏幕。若出现
TARGET IDENTIFIED 或确认界面，请先拍照，不要立即完成最终按键确认。

烧写规则保持不变：识别为 32 MiB NOR 后，全擦 256 个 128 KiB 扇区，
再以 32 字节操作烧写实际 ROM，最后读取卡带并计算 MD5。
