smoke.69 原始 Identify 只读诊断

目的：读取 FPGA 固定 Identify 流程返回的 32 个原始字节，判断失败来自 JEDEC ID、CFI 字节通道/D0-D1 位序，还是未公开的 CFI geometry 字段。

安全边界：
- 使用已经通过硬件启动验证的 smoke.66 os.bin 与 RBF。
- 只发出 SAFE_IDLE 和 READ(IDENTIFY) 命令。
- 不包含 ROM 文件。
- 不调用 ARM、扇区擦除或编程命令。

安装：把 SD_ROOT 内文件完整覆盖到 TF 卡根目录，然后彻底关机再开机。

测试：
1. 插入同一张 S29GL256N 复现卡带。
2. 启动 ChisBurner。
3. 左右键查看 1/4 到 4/4。
4. 拍下全部四页并发送。

判读：
- DIRECT RC 0、RX LEVEL 32 表示原始 Identify 事务完成。
- 第 3 页显示 ID、QRY、容量和写缓冲字段以及检测到的字节通道/位序。
- 若 PUBLIC FIXED CHECKS PASS 但 HARDWARE PROFILE FAIL，则失败只可能在隐藏的 CFI geometry 检查。
