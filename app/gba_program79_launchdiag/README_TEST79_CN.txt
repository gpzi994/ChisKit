smoke.79b 测试流程（完全只读，不会擦除或烧写卡带）

目的：把“Play Cartridge 启动”和“待烧 ROM 选择”拆开，验证卡带生命周期、ROM 槽和 S29GL256S 识别可同时成立。

1. 将压缩包 SD_ROOT 的内容覆盖到 SD 卡根目录。
2. Pocket 完全关机；插好 GBA 卡带后再开机。
3. 进入 openFPGA -> ChisBurner。
4. 启动时必须选择 Play Cartridge。不要在启动阶段选择逆转裁判 ROM。
5. 进入 smoke.79b 后看第 1 页：PLAY、POWER、READY 应全部为 1。
6. 按 Pocket 菜单键，进入 Core Settings，选择“Program ROM (load after launch)”，加载：
   C:\Users\lin13\Desktop\game\逆转裁判.gba
7. 回到核心后按 A 重新扫描；第 2 页应显示：
   SIZE 8705280 EXPECT 8705280
   ROM READY 1
8. 翻到第 5 页。目标结果：
   PLAY CARTRIDGE PASS
   RUNTIME ROM SLOT PASS
   S29GL256S TARGET PASS
9. 如果失败：
   - Play Cartridge 失败：拍第 1 页和第 5 页。
   - ROM Slot 失败：拍第 2 页和第 5 页。
   - Target 失败：拍第 3、4、5 页。

安全保证：此版本应用中不存在 ARM、ERASE、PROGRAM 或输出写入调用。
