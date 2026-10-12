# ChisKit 0.01b

Initial public beta for Analogue Pocket / openFPGA.

## 简体中文

- GB/GBC/GBA 卡带分析、ROM 备份，以及受支持烧录卡的擦除、写入和校验。
- 已实现类型的存档备份、恢复，以及受支持卡带的 RTC 管理。
- 中文 / English 界面，浅色 / 深色主题。
- 安装：解压固件附件，将 ZIP 根目录的 Assets、Cores、Platforms、Saves 合并至 TF 卡根目录。
- 固件 ZIP 已改为 openFPGA 根目录布局；应用、OS 和 FPGA 内容保持不变。
- 写入前备份重要数据，不要混用不同版本的应用、OS 和 FPGA 文件。

## English

- GB/GBC/GBA cartridge analysis, ROM backup, and erase/program/verify for supported flash cartridges.
- Save backup and restore for implemented types, with RTC management on supported cartridges.
- Chinese / English UI with light / dark themes.
- Install: extract the firmware asset and merge Assets, Cores, Platforms and Saves from the ZIP root into the microSD root.
- The firmware ZIP now uses the openFPGA root layout; application, OS and FPGA contents are unchanged.
- Back up important data before writing. Keep application, OS and FPGA files from the same version together.

Full source is in the repository at this release tag. GitHub also generates
source archives automatically; no separately uploaded source ZIP is needed.
See README.md / README.en.md for usage, and THIRD_PARTY_NOTICES.md for licenses
and outstanding font/platform rights notes. This is beta software, not a
guarantee of compatibility with every cartridge.
