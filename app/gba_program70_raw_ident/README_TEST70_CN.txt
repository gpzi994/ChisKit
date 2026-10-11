smoke.70 原始 Identify 只读诊断（SPACE 地址修正版）

smoke.69 的诊断程序错误地把逻辑 CART_SPACE 寄存器当作连续窗口中的第 16 项；实际硬件为避开存储审计窗口，将它单独映射在 0x4000015C。因此 smoke.69 成功读取的是普通 ROM 起始 32 字节。

smoke.70 已改用正确的 0x4000015C。其他内容不变：仍使用硬件验证过的 smoke.66 os.bin/RBF，并且只执行 SAFE_IDLE、READ(IDENTIFY)、SAFE_IDLE，不包含 ARM、擦除或烧写命令。

安装和测试：
1. 将 SD_ROOT 中内容完整覆盖到 TF 卡根目录。
2. 完全关机后重新开机。
3. 插入同一张 S29GL256N 卡带并启动核心。
4. 左右键查看 1/4 到 4/4，拍下全部四页。
