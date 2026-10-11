ChisBurner - smoke.73 GBA NOR 首次实机烧写测试说明

一、修正内容

smoke.72 错误复用了 smoke.71 的只读 OS/RBF，因此在任何擦除或写入前以
TARGET IDENTIFY FAIL RC -4 安全停止。smoke.73 同时换入启用 programmer 的
OS 与 FPGA 位流；不要混用 smoke.71/smoke.72 的 ELF、OS 或 RBF。

二、测试对象与源文件

  NOR：S29GL256S，32 MiB，AMD 命令集
  扇区：128 KiB，共 256 个
  当前写入单元：32 字节
  ROM：逆转裁判.gba
  精确大小：8705280 字节（0x84D500）
  标题/代码：GYAKUTEN_SAI / ASBJ
  MD5：07e851b1dc49009ebd00982514ebfe92
  SHA-256：0d0945fff2edd4d4ae627ed003ebb0050937ee689d606ccff89c4ff2c2638571

程序必须识别出完全匹配的 S29GL256S/CFI 参数，否则在擦除前停止。

三、烧写范围

无论 ROM 只有多大，都按识别出的 NOR 总容量完整擦除：

  256 个扇区 x 128 KiB = 32 MiB

随后烧写并最终回读校验 ROM 的准确 8705280 字节，共 272040 个 32 字节块。
镜像结尾之后保持全擦后的 FF。本版使用逐扇区全擦，不使用芯片擦除命令，
不访问存档或 RTC。

四、准备与安装

1. 备份卡带 ROM、存档和其他重要内容。
2. Pocket 充足电并连接稳定电源，关闭休眠。
3. 将本压缩包内 SD_ROOT 的全部内容合并复制到 TF 卡根目录。必须整体更新，
   不要只替换 ELF；否则仍可能得到 RC -4。
4. 测试期间不要拔卡、拔 TF 卡、关机或切换核心。

五、启动与确认

进入 openFPGA -> ChisBurner -> Play Cartridge，选择指定 ROM。
人工确认页面出现前只执行识别和源文件检查，不会擦除或写入。核对：

  SOURCE GYAKUTEN_SAI ASBJ
  SIZE 8705280 B HEADER PASS
  SOURCE MD5 07e851b1dc49009ebd00982514ebfe92
  TARGET AMD ID 227E0001
  FULL ERASE 256 SECTORS

任意内容不符，按 B 取消并拍照。

六、人工解锁

1. 松开所有按键 0.5 秒；
2. 按住 L+R 2 秒；
3. 松开所有按键 0.5 秒；
4. 按一下 A；
5. 松开所有按键 0.5 秒；
6. 按住 START（+）1 秒。

确认后才开始破坏性操作。擦除开始后不得断电。

七、成功判据

  PROGRAM PASS RC 0
  STAGE COMPLETE
  ERASE 256/256
  PROGRAM 272040/272040
  VERIFY 8705280/8705280
  STOP OFFSET: NONE
  SOURCE MD5 与 CART MD5 完全相同
  DISARM 0 SAFE 0

请拍摄结果第1页和计时第2页。随后完全关机，再直接运行卡带确认游戏启动。

八、失败处理

出现任何非零 RC、STOP OFFSET、MD5 不一致、红屏或冻结：拍摄完整画面，
不要带电拔卡，不要连续盲目重试，并告知失败阶段、RC 和 STOP OFFSET。
