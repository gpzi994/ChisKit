smoke.78：B1 / Reset 时序只读诊断

目的：确认带 ROM 数据槽启动时，Pocket 是否向核心发送 B1 卡带生命周期通知，
以及该通知是否随后被 Reset Enter 清除。

安全边界：
- 只读取卡带能力寄存器和 slot:4 文件大小。
- 不调用协议 Ping、卡带 ROM 读取、Flash Identify、擦除、烧写或 Arm。
- 该版本不可能进入烧写确认页。

测试：
1. 使用与 smoke.77 相同的目录结构覆盖 TF 卡。
2. 将“逆转裁判.gba”选为 Play Cartridge 的 ROM。
3. 冷启动 Pocket 并进入 ChisBurner。
4. 等待约 2 秒进入第 1 页。
5. 用左右键查看 1/5 到 5/5，拍摄全部五页。
6. 不要拔卡；无需按 A/B。

关键字段：
- B1：核心最后一次收到的 B1 原始值。
- COUNT B1/ENTER/EXIT：B1、Reset Enter、Reset Exit 的累计次数。
- VALID：当前 B1 是否仍有效。
- RESET_N：APF 当前是否已退出复位。
