# ChisBurner smoke.97：UI 第一阶段

本版交付三页 320×288 像素 UI（10:9 显示模式），以及启动/刷新时的 GB/GBC 或 GBA 只读头识别。
这是独立的 UI 试验包，不是已接通全部烧录能力的正式版。保留此前已验证版本。

## 安装与测试

1. 关机后插卡。把本包 `SD_ROOT` 中的 Assets、Cores、Platforms 合并到 SD 根目录，完整覆盖同名文件；不要只换 ELF 或只换 FPGA。
2. 启动 `openFPGA → ChisBurner → Play Cartridge`。顶栏应为 `ChisBurner / smoke.97`。卡带准备好后自动等待稳定并读取头。
3. 第一页应显示 `GB/GBC` 或 `GBA`，有效且重复一致的头显示 `Header verified`。未知、损坏或不稳定的头不会强行报告识别成功。
4. 方向键选功能，A 确认，L/R 肩键切三页，B 回功能页。`Refresh` 重读；`Analyze Cart` 重读后转到信息页；`Cart Info` 只显示上次结果；`Options` 显示当前只读配置。
5. GB/GBC 信息页显示 Mapper（来自头字段，不是物理 mapper 行为认证）、ROM/RAM 声明容量、CGB 标志、RTC 声明及校验信息。GBA 信息页显示游戏代码/厂商/修订号，不出现 Mapper。
6. 四个备份/写入按钮为灰色，按 A 只提示未启用。第三页应显示 `No active task`，不显示假进度、速度或成功状态。
7. 分别用准备好的 MBC3、MBC5、GBA 卡测试，每张卡做两次刷新；换卡必须关机，不进行热插拔。检查整页没有文字重叠或裁切，并拍功能页和对应信息页反馈。

## 本阶段不做什么

- 没有 ROM dump、ROM 烧写、存档备份/写入、NOR JEDEC/CFI 命令，也不访问 SRAM/RTC。
- NOR 型号和容量字段已经放入分析页，当前明确显示 `Not probed (stage 1)` / `Unknown`，不根据 ROM 头或此前另一张卡的数据猜测。下一阶段接实际 NOR 探测和精确型号 profile，再考虑写入放行。
- GBA 头无法直接给出准确的实体 NOR 容量、ROM 长度、存档器件型号；这些显示 `Needs full analysis`。GB/GBC 显示的 ROM/RAM 容量也只是头声明，不代表实际烧录卡 NOR 容量。
- GB 全局校验值只显示头中的存储值，不等于全 ROM 重算验证通过；汉化 ROM 可以与重算值不同。
- 顶栏只显示版本；没有时钟、电池电量、Genre，也不伪造数据库游戏名称。

## 安全与兼容

新 FPGA 宏 `CHISBURNER_UI97` 只包装现有 GB 和 GBA 只读引擎，未改动旧 GBA 烧录引擎。GB 首探只读 0x0100–0x015F；GBA 只读 0x0000–0x00BF。每次按 32 字节读取，丢弃每遍第一次读取，重复比较，再校验 logo 和头校验。

默认先探 GB；可辨认但损坏的 GB 头、读错误或不稳定读会停在未知，不盲目切换。探不到有效头时不能仅凭本版判定卡带损坏。无有效 GB 头的稳定结果才继续 GBA 探测。该保守策略需要三类实体卡实测确认。

两种总线都不发出 /WR，SAFE/ABORT 到达两个引擎，事务中禁止切换总线。所有阶段结束回安全空闲。已有 GBA 程序路径保留并另做 RTL 回归，本包不开放其写功能。

预览由与 Pocket 应用同一套绘制代码生成，示例数据仅演示布局；不是本次实体卡测试截图。桌面测试/仿真通过并不等于 Pocket 实测通过。

## 开发复现

- OS：PowerShell 运行 `tests/build-ui97-os.ps1`；保留 contract-check，不绕过检查。
- 应用：WSL 在 `app/ui97` 运行 `make all`，使用现有 openfpgaSDK。
- 自动测试：WSL 运行 `bash tests/check-ui97.sh <本地有效GBA-ROM路径>`，仅取该文件头做测试，不复制 ROM 到安装包。
- 预览：PowerShell 运行 `tests/render-ui97.ps1`。
- FPGA：运行 `tests/build-ui97-fpga.ps1 -Seed <新种子>`，每个种子使用独立目录，保留旧结果。
- 打包：运行 `tests/build-ui97-package.ps1 -Seed <时序通过的种子>`；负 slack、过期测试/构建凭据、启动 MIF 不一致和过长标识符均拒绝打包。
