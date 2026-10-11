ChisBurner - smoke.71 GL-S profile 只读验证说明

目的
----
smoke.70 已证明 Identify/CFI 总线事务、低字节通道和地址顺序正确，并实测：

  JEDEC：0001 / 227E
  CFI：QRY、AMD/Fujitsu 命令集 0002
  容量：2^19 = 32 MiB
  写缓冲上限：2^9 = 512 字节

0x09 与 Infineon S29GL256S 官方 CFI 表一致。smoke.71 将 FPGA 的目标
profile 从误用的 GL-N 0x05 修正为 GL-S 0x09，但实际烧写粒度仍固定为
32 字节。本包仍然只执行 SAFE_IDLE、READ(IDENTIFY)、SAFE_IDLE，不会发出
ARM、扇区擦除或烧写命令，也不需要选择 ROM 文件。

安装
----
1. 删除 TF 卡上旧版的以下目录后，再复制本包 SD_ROOT 内的全部内容：
   Assets/chisburner
   Cores/ChisBurner.ChisBurner
   Platforms/_images/chisburner.bin
2. 安全弹出 TF 卡，Pocket 完全关机后重新开机。
3. 进入 openFPGA -> ChisBurner。

测试
----
1. 保持当前 GBA 卡带插好；卡带内是什么 ROM 不影响本测试。
2. 启动核心。无需选择 ROM 文件。
3. 用 LEFT/RIGHT 查看四页并拍照。
4. 重点检查：
   - 第 1 页 DIRECT RC 0、AFTER ST 00000005、ERR 00000000；
   - 第 1 页 PROFILE 最低位应为 1；
   - 第 3 页 BUF 0009、LANE: BANK3/LOW NATIVE；
   - 第 4 页 PUBLIC FIXED CHECKS: PASS；
   - 第 4 页 HARDWARE PROFILE: PASS。

如果任一项不是 PASS，请不要进入后续烧写测试，发送四页照片。
