smoke.74 只读目标识别诊断测试

目的
----
定位 smoke.73 中 TARGET IDENTIFY FAIL RC -4 对应的具体能力字段。

安全范围
--------
本版本仅执行能力查询、协议查询、PING、Flash ID/CFI 查询、只读
IDENTIFY 空间读取和 SAFE IDLE。应用中不存在 ARM、擦除或烧写调用，
不会修改卡带内容，也不需要选择 ROM 文件。

安装与运行
----------
1. 解压 ZIP，把其中 SD_ROOT 目录内的全部内容覆盖到 Pocket TF 卡根目录。
2. 完全关机后重新开机。
3. 启动 Play Cartridge / ChisBurner。无需选择 ROM。
4. 等待出现 GBA TARGET DIAG smoke.74 1/5。
5. 使用方向键 LEFT/RIGHT 依次查看 1/5 至 5/5。
6. 清晰拍下全部五页并发回，尤其不要遗漏第 5 页。

判读
----
第 5 页会把程序正式识别所要求的每个字段单独标成 0 或 1。
1 表示该字段符合，0 表示该字段不符合。收到五页结果后即可确定
RC -4 是卡带能力字段还是 Flash 能力字段造成的。

