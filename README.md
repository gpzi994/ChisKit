# ChisKit

*在 Analogue Pocket 上，管理你的 GB / GBC / GBA 实体卡带。*

**简体中文** · [English](README.en.md)

`0.01b · Beta`　`Analogue Pocket / openFPGA`　`Developed by Alius`

---

ChisKit 是运行在 Analogue Pocket 上的卡带工具，提供 ROM 与存档备份、存档恢复、实时时钟管理，以及受支持烧录卡的 ROM 擦除、写入和校验。

> **使用前请备份。** ROM 烧写和存档恢复会覆盖卡带数据。项目处于 Beta 阶段，已验证的卡带不代表所有卡板、芯片和 Mapper 组合均兼容。

## 📋 功能一览

| 功能 | GB / GBC | GBA |
| --- | --- | --- |
| 卡带分析、ROM 备份 | 支持已实现的 Mapper | 区分烧录卡 NOR 与正版卡容量路径 |
| 烧录卡 ROM 擦除、写入、校验 | 受支持的 NOR 配置 | 受支持的 NOR 配置 |
| 存档备份、恢复 | 普通 SRAM；MBC2 512 字节；MBC7 256 字节 EEPROM | SRAM/FRAM；512 字节及 8 KiB EEPROM；64/128 KiB Flash |
| RTC 读取、备份、恢复、自由修改 | 受支持的 MBC3 / MBC30 时钟卡 | 检测到受支持 RTC 的卡带 |
| 界面 | 中文 / English；浅色 / 深色主题 | 中文 / English；浅色 / 深色主题 |

GB/GBC 与 GBA 使用各自的平台处理逻辑。Pocket 系统菜单和输出文件名保持英文；备份文件名包含可识别的游戏信息。

RTC 选项仅在检测到支持时开放。恢复 RTC 还需要存档包含对应时钟数据，可选择补偿备份后经过的时间。GB RTC 不提供同步 Pocket 日期时间功能。

## 📦 安装

准备支持 openFPGA 的 Analogue Pocket、microSD 卡和需要操作的卡带。

1. 获取发布时提供的 `ChisKit-0.01b-firmware.zip`，先备份卡内的重要文件。
2. 解压，将 ZIP 根目录内的 `Assets`、`Cores`、`Platforms`、`Saves` 合并到卡根目录，不要删除原有目录。
3. 若使用 USB 传输，完成后安全弹出并退出 USB 模式。
4. 在 Pocket 的 openFPGA 中启动 ChisKit。

请配套更新应用、OS 和 FPGA 文件，不要混用版本。安装无需自行编译；源码包供审查、修改和重建。本项目不包含商业游戏 ROM 或用户存档。

## 💾 使用方法

1. 更换卡带后，使用 **重置 / Reset** 清除上一张卡的检测状态。
2. 选择 **分析卡带**，检查游戏信息、Mapper、ROM 容量和存档类型。
3. 优先备份 ROM 或存档，确认成功后再进行写入操作。
4. 写入 ROM 或恢复存档前，通过 Pocket 菜单加载对应文件，核对目标卡带及容量。
5. 等待完成提示；操作过程中不要拔卡、断电或进入 USB 模式。

需要编辑时钟时，先分析卡带；确认卡带支持 RTC 后，在主页面按 **X** 打开对应平台的 RTC 编辑页面，读取并自由修改时钟信息，按页面提示确认写入。

ROM 和 SAV 文件选择入口可访问 `Saves/chiskit/common/`，输出文件也使用此目录，方便将备份重新用于恢复。不要把其他游戏的 SAV 当成当前卡带的存档。

入口灰色通常表示尚未完成分析、类型未确定或操作不受支持。手动存档类型设置不代表任意硬件配置都可以安全写入。

## ⚠️ 兼容范围与限制

- 未实现 MBC6、2 KiB RAM 和 MBC7 512 字节存档操作。
- 电池失效或 RTC 通信异常可能导致时钟功能不可用。

验证样例包括 GB/GBC 宝可梦系列、塞尔达烧录卡、吞食天地、滚滚卡比，以及 GBA 瓦力欧寻宝记、龙珠大冒险、缩小帽、黄金太阳和绿宝石等。这不是所有地区、卡板版本或芯片批次的兼容保证。

## 🔧 源码与构建

项目包含应用、RTL、修改后的 openfpgaOS、SDK、构建脚本及资源输入。首次重建请使用完整源码包，避免遗漏开发仓库忽略的依赖目录。

Linux / WSL 应用与 OS 构建，运行：

```bash
export OPENFPGA_SDK="$PWD/dependencies/openfpgaSDK"
bash tests/build-gb-rtc189.sh release001b
```

验证环境使用 RISC-V GCC 13.2.0；其他工具包括 make、Python 3、host GCC，以及 RTL 仿真所需的 Icarus Verilog。FPGA 工程为 `quartus/ap_core.qpf`，使用 Quartus Prime Lite 25.1std。

## 💬 反馈

请提供版本号、平台、正版卡或烧录卡、游戏名、Mapper / NOR / 存档类型、操作步骤、错误码，以及截图或日志。不要公开上传商业 ROM 或不愿公开的个人存档。

## 📚 致谢与授权

Developed by **Alius**。

感谢 [MoriBunrnner-for-ESP32-S3](https://github.com/moribaka/MoriBunrnner-for-ESP32-S3) 的实现参考及移植代码、[FlashGBX](https://github.com/lesserkuma/FlashGBX) 的卡带与存档处理参考，以及 openfpgaOS、openfpgaSDK 和相关项目的贡献。ChisKit 不是这些项目或 Analogue 的官方产品。

自有及相关派生代码按 [LICENSE.md](LICENSE.md) 说明发布；第三方组件保留原许可证。来源及修改说明见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。

界面字体使用 **凤凰点阵体（Vonwaon Bitmap Font）**，作者 **Haoyu Qiu**。感谢作者提供字体，详见 [作者发布页面](https://timothyqiu.itch.io/vonwaon-bitmap)。

## ☕ 请杯咖啡

如果 ChisKit 对你有帮助，欢迎请我喝杯咖啡，支持后续开发。感谢你的支持！

使用支付宝扫描下方二维码即可自愿赞助；不赞助也可以正常使用全部功能。

<p align="center">
  <img src="docs/images/alipay-support.png" width="300" alt="支付宝赞助二维码" />
</p>
