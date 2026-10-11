# ChisBurner smoke.101：传输块、NOR 状态与 mapper 重入修复版

> 2026-09-08 状态：smoke.100 实机确认仍有三处闭环问题：GBA 备份在第一次输出写入时 `rc -2`；二次确认写 ROM 后在首个扇区擦除时 `rc -10`；GB/GBC NOR 识别器的一次性门控导致重复分析及换入 MBC3 时 `rc -13`。本版将 64 KiB 应用输出拆成两个 32 KiB Pocket 目标写，保留二次确认后重新识别出的 NOR profile 直到擦除，并把固定命令的只读 NOR 识别改成可重复、幂等执行。主机、RTL、AXI 与既有 GBA/MBC3/MBC5 回归通过后仍需 Pocket 实机验收。

本版以已验收的 smoke.97 三页 UI 和既有 GBA 擦写引擎为基础。槽 4 只用于用户选择待写入 ROM，槽 5 只用于自动创建及回读备份；两者不再相互改绑。GB/GBC 自动检测和分析遇到瞬态 I/O 错误仍会有限重试；每次 SAFE 都从 mapper 序列 0 重新开始，NOR 固定命令识别器本身可重复执行。MBC3 与 MBC5 都继续进行只读 NOR 查询，但 GB/GBC 擦写仍未开放。包内不含游戏 ROM。

## 安装

将 `SD_ROOT` 中的目录合并复制到 TF 卡根目录，覆盖同名 ChisBurner 文件。必须整包更新，不能只换 ELF、OS 或 RBF。通过 Play Cartridge 启动，右上角应显示 `smoke.101`。Pocket 菜单中数据槽 4 名称为 `ROM input (select .gba)`；写 ROM 前在这里选择自己的 `.gba` 文件。数据槽 5 名称为 `Backup output (automatic)`，由程序自动使用，不需要手动选择文件。

## 当前接入范围

| 功能 | 状态与边界 |
| --- | --- |
| 三页 UI、Refresh、GBA / GB/GBC 自动区分 | 接入；刷新清除上张卡的分析及写入许可 |
| GB/GBC 头信息、MBC3 / MBC5 判读 | 接入；头类型是主要依据，银行行为用于一致性验证，不把烧录卡行为直接当成头类型 |
| GBA NOR | 写入白名单目前只有 S29GL256S：32 MiB，256 × 128 KiB，512 字节缓冲编程配置；其他卡仍可只读识别与备份 |
| GB/GBC NOR | 已验证 bank-0 alias 通路上的 MX29GL640EB：8 MiB，8 × 8 KiB + 127 × 64 KiB，32 字节缓冲；只识别、不擦写 |
| PRI | MX29GL640EB 接受已知 1.1 / 1.3，仍校验 PRI 签名、底部启动区、两次几何一致性和完整容量 |
| GBA Backup ROM | 不要求可写 NOR 型号；对有效 GBA 卡执行 32 MiB 地址窗口只读备份，在独立输出槽 5 创建新文件，写后整文件 MD5 + CRC32 回读比较 |
| GBA Write ROM | 复用已验收编程器；仅启用已测的 32 MiB 和 8,705,280 字节长度；完整 32 MiB 扇区擦除，512 字节缓冲写入，写入范围 MD5 和末 32 字节校验 |
| GB/GBC 全 ROM dump、GB NOR 烧写、存档备份/写入 | 保持禁用，本轮不把尚未闭环的测试当作已验证功能 |

未知型号、几何不符、读取异常或不支持的映射器不解锁烧写。识别到型号或配置不代表 GB 烧写已受支持。MBC3 的 bank-0-to-1 行为必须与 MBC3 头一致；MBC5 的 bank-0 alias 行为必须与 MBC5 头一致，两类通过后都执行同一套固定命令只读 NOR 查询。

## 第一轮：只测界面和分析

1. 分别插入 GBA 与 GB/GBC 卡，点击 Refresh，确认分类、ROM 头信息正确。
2. 点击 Analyze Cart，自动进入信息页。上下键切换头信息 / NOR 详情；L/R 切三大页，B 返回功能页。
3. GBA S29GL256S 应显示 32768 KiB、256 × 128 KiB、buffer512 与匹配配置；GBA 页不显示 mapper 类型。
4. MBC5 分析完成后，不退出核心再次 Refresh / Analyze，必须仍能完成，不能再出现 `analysis rc -13`。然后空闲换入 MBC3，Refresh / Analyze；应显示 MBC3 行为与头一致，并继续显示 NOR 识别结果，不能只显示 `Analysis blocked`。
5. GB MX29GL640EB 应显示 8192 KiB、8 × 8 KiB、127 × 64 KiB、Region 1 起点 0x010000、PRI 1.3（该实物）及 Write Disabled。
6. 有效 GBA 头完成 Refresh 后 Backup ROM 即可选；仅 S29GL256S 分析匹配后 Write ROM 可选。GB/GBC ROM 传输以及所有存档按钮保持禁用。
7. 可进入 GBA Write ROM 确认页后按 B 取消；这一步不得擦除卡带。

已在 UI97 实测的“空闲时换卡后 Refresh”流程保留，但不将其扩大为任意时刻热插拔安全保证。传输或 NOR 查询期间不要拔卡。稳妥的换卡方式仍是断电；若继续空闲换卡测试，换卡后必须 Refresh / Analyze，不能沿用旧结果。

## 第二轮：GBA 备份

先分析已支持的 GBA 卡，选择 Backup ROM，第一次按 A 进入确认页；松开 A 后再按一次 A 才开始任务。B 可在第二次确认前取消。长按第一次 A 不会被当作第二次确认。

输出到 `/Saves/chisburner/common/cb101-XXXXXXXX.gba`，固定 32 MiB。文件名使用当前计时值，先检查同名文件：已存在则拒绝，不主动覆盖或删除旧备份。保证 TF 卡空间充足；错误后的不完整文件保留，不能当作成功备份。smoke.101 的验收重点是正版卡和烧录卡都必须越过第一次 `Writing backup output`，不能再得到 `rc -2`，并最终完成全文件回读校验。

传输页显示阶段、KiB 数量、阶段平均速度、累计时间、估计剩余时间和校验结果。PASS 意味着卡带读出数据与写入 TF 卡后全文件回读一致，不代表与某个原版 ROM 数据库一致。请再把文件拷到电脑检查长度及 MD5；需要验证卡带读取内容本身时，和可信参考 ROM / 独立 dump 比对。

## 第三轮：GBA 烧写（会删除原卡全部 ROM）

1. 先保存并检查备份。在 Pocket 菜单打开数据槽 4 `ROM input (select .gba)`，选择目标 `.gba`；备份输出使用槽 5，不会替换该选择。
2. 返回核心后 Refresh / Analyze，再选 Write ROM。第一次按 A 选择任务；程序先扫描完整输入文件、验证 GBA 头，再显示游戏标题。松开 A 后第二次按 A 才授权写入，B 取消。第二次 A 后会真正重新识别 NOR，但不会再用 SAFE 清除刚得到的 profile；必须越过首个 `Erase sectors`，不能再得到 `rc -10`。
3. 只接受 32 MiB 或 8,705,280 字节输入；其他长度拒绝。本轮没有自动填充、截断或“试用其他型号”逻辑。
4. 确认后重新检查 NOR 配置，再擦除 **整个 32 MiB NOR**，即使输入 ROM 较小也一样。SRAM 存档不在操作范围内。
5. 开始擦写后不提供中途停止按钮，不断电、不拔卡。出错立即停止后续操作并执行 disarm / safe；已发生的擦除不可撤销，需要用备份重新烧写恢复。
6. 完成需 PASS / rc 0、MD5 与输入一致，再测试卡带实际运行。每个任务结束都会撤销 UI 写许可，下一次必须重新分析。

## 验证分层

原 GBA 编程引擎来自 smoke.82/83 的已验收功能；组合版做了主机 ASAN/UBSAN、真实 RTL Flash 模型、MBC3/MBC5、CFI 几何、AXI 到组合路由及旧 UI97 回归。新 UI、OS 文件输出接线及多引擎组合的最终实机验收仍需本轮完成。不要用“仿真 PASS”代替实机 PASS。
