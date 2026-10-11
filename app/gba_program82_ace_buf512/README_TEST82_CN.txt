smoke.82 S29GL256S 512 字节缓冲实际烧写测试

警告：本版本会实际擦除并改写卡带 NOR。确认后无法恢复卡带原有 ROM。

固定目标：
- NOR：S29GL256S，32 MiB
- 擦除：完整 32 MiB，共 256 个 128 KiB 扇区；不会因 ROM 较小而缩短擦除范围
- ROM：C:\Users\lin13\Desktop\game\逆转裁判.gba
- ROM 大小：8705280 字节
- ROM MD5：07e851b1dc49009ebd00982514ebfe92
- 烧写：17002 个完整 512 字节块，加最后 1 个 256 字节块，共 17003 次
- 最终验证：以 32 字节读取重新读取 8705280 字节并核对 MD5

测试流程：
1. 将压缩包 SD_ROOT 内容覆盖到 SD 卡根目录。
2. Pocket 完全关机，插稳目标卡带后再开机；全程不要拔卡。
3. 进入 openFPGA -> ChisBurner，启动时必须选择 Play Cartridge。
4. 程序显示 WAITING FOR PROGRAM ROM 后，打开 Pocket 菜单 -> Core Settings。
5. 选择“Program ROM (load after launch)”，加载上述逆转裁判 ROM。
6. 返回核心。程序会自动检测文件、扫描 GBA Header 和源文件 MD5。
7. 到确认页后，核对屏幕必须显示：
   TARGET AMD ID 227E0001
   IMAGE 8705280 B
   FULL ERASE 256 SECTORS
   128 KiB SECTOR / 512 B PROGRAM
8. 确认操作：
   - 先松开全部按键 0.5 秒；
   - 按住 L+R 2 秒；
   - 松开全部按键 0.5 秒；
   - 按 A 选择 ERASE+PROGRAM；按 B 会取消并重新锁定；
   - 再松开全部按键 0.5 秒；
   - 按住 START（Pocket 的 + 键）1 秒，才会开始实际全擦。
9. 开始后保持供电且不要退出核心、休眠或拔卡，直到结果页出现。
10. 成功标准：PROGRAM PASS、ERASE 256/256、PROGRAM 17003/17003、
    VERIFY 8705280/8705280，SOURCE MD5 与 CART MD5 完全一致。
11. 若失败，不要立刻重试；拍摄结果页和 Timing 页。

安全门：目标生命周期、协议/PING、S29GL256S ID/CFI/几何和 B512 能力、
ROM 大小、GBA Header、源 MD5 扫描及双阶段实体按键确认中任一项失败，
都不会进入擦除。程序不使用芯片整片擦除命令，而是逐个擦除识别容量内全部扇区。
