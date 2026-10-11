smoke.83 S29GL256S 32 MiB 全容量烧写与末地址覆盖测试

警告：本版本会实际擦除并改写卡带全部 32 MiB NOR。确认后无法恢复卡带原有内容。

固定目标：
- NOR：S29GL256S，32 MiB
- 擦除：完整 32 MiB，共 256 个 128 KiB 扇区
- ROM：C:\Users\lin13\Desktop\game\Pokemon - Emerald Version (USA, Europe).gba
- ROM 大小：33554432 字节（精确 32 MiB）
- ROM 标题/代码：POKEMON EMER / BPEE
- ROM MD5：872a1a05e39006f3bea69efaa641b3a4
- ROM SHA256：d1e41d5f8004de024391be96535bbc8952fb30a7b04b6e9c5a666d6927492dec
- 烧写：65536 个完整 512 字节块，无短尾块
- 末次写入：地址 0x01FFFE00，长度 512，覆盖至 0x01FFFFFF
- 最终验证：读取全部 33554432 字节并核对 MD5
- 末次读取：地址 0x01FFFFE0，长度 32，覆盖至 0x01FFFFFF
- 末端验证：最后 32 字节除整片 MD5 外再逐字节比较

测试流程：
1. 将压缩包 SD_ROOT 内容覆盖到 SD 卡根目录。
2. Pocket 完全关机，插稳目标卡带后再开机；全程不要拔卡。
3. 进入 openFPGA -> ChisBurner，启动时必须选择 Play Cartridge。
4. 程序显示 WAITING FOR PROGRAM ROM 后，打开 Pocket 菜单 -> Core Settings。
5. 选择“Program ROM (load after launch)”，加载上述 32 MiB 绿宝石 ROM。
6. 返回核心。程序会自动检测精确文件大小、GBA Header 和源文件 MD5。
7. 到确认页后，核对屏幕必须显示：
   SOURCE POKEMON EMER BPEE
   IMAGE 33554432 B
   FULL ERASE 256 SECTORS
   128 KiB SECTOR / 512 B PROGRAM
8. 确认操作：
   - 先松开全部按键 0.5 秒；
   - 按住 L+R 2 秒；
   - 松开全部按键 0.5 秒；
   - 按 A 选择 ERASE+PROGRAM；按 B 会取消并重新锁定；
   - 再松开全部按键 0.5 秒；
   - 按住 START（Pocket 的 + 键）1 秒，才会开始实际全擦。
9. 开始后保持可靠供电；不要退出核心、休眠或拔卡，直到结果页出现。
10. 结果页成功标准：
    PROGRAM PASS RC 0
    ERASE 256/256
    PROGRAM 65536/65536
    VERIFY 33554432/33554432
    SOURCE MD5 与 CART MD5 完全一致
11. Timing 页成功标准：
    LAST WR 01FFFE00 +512
    LAST RD 01FFFFE0 +32
    END 32 BYTE MATCH PASS
12. 冷启动并运行卡带，确认游戏能够正常进入。
13. 若失败，不要立即重试；拍摄结果页和 Timing 页。

安全门：目标生命周期、协议/PING、S29GL256S ID/CFI/32 MiB 几何和 B512
能力、ROM 精确大小、GBA Header、源 MD5 扫描及双阶段实体按键确认中任一项失败，
都不会进入擦除。程序逐扇区擦除全部 32 MiB，不使用芯片整片擦除命令。
