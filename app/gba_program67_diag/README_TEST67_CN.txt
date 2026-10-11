ChisBurner - smoke.67 目标识别只读诊断包

用途

smoke.67 只用于定位 smoke.66 的 TARGET IDENTIFY FAIL RC -10。
本应用没有解锁流程，也不会调用卡带擦除或编程服务。它只执行：

1. 读取 OS/FPGA 能力及状态寄存器快照；
2. 查询只读协议并执行无副作用 PING；
3. 执行固定的 JEDEC/CFI 识别序列；
4. 请求安全空闲，并再次执行固定识别序列；
5. 显示每一步返回值和前后状态。

固定识别序列会向 Flash 发送进入/退出 ID 和 CFI 模式所需的命令字，但绝不
发送扇区擦除、芯片擦除或编程命令。应用中不存在 ARM、ERASE 或 PROGRAM
调用。

安装

1. 将压缩包中 SD_ROOT 下的全部内容合并覆盖到 TF 卡根目录。
2. 完全关闭 Pocket 电源，再重新开机。
3. 进入 openFPGA -> ChisBurner -> Play Cartridge。
4. 本包不需要选择 .gba 文件。

取证

进入后诊断会自动运行一次，一般数秒内完成。使用左/右键查看共 5 页，拍摄
全部页面并发回。尤其要保证以下字段清晰可见：

  INITIAL CART_GET_CAPS RC
  ID / CAPS
  STATUS / ERROR / PROFILE
  GEOM
  FLASH CAPS FIRST RC
  SAFE IDLE RC
  IDENTIFY SPACE READ RC
  FLASH CAPS RETRY RC
  FINAL STATUS / ERROR

不要在诊断时拔卡、休眠或断电。页面拍摄完成后可正常退出并关机。

