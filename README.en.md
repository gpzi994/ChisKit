# ChisKit

*Manage your physical GB / GBC / GBA cartridges on Analogue Pocket.*

[简体中文](README.md) · **English**

`0.01b · Beta`　`Analogue Pocket / openFPGA`　`Developed by Alius`

---

ChisKit is a cartridge utility for Analogue Pocket. Back up ROMs and saves, restore saves, manage supported real-time clocks, and erase, program and verify flash cartridges with supported hardware profiles.

> **Back up first.** ROM programming and save restoration overwrite cartridge data. This is a beta release: tested cartridges do not establish compatibility with every board, chip or mapper combination.

## 📋 Features

| Feature | GB / GBC | GBA |
| --- | --- | --- |
| Cartridge analysis and ROM backup | Implemented mappers | Separate flash-cartridge NOR and retail-cartridge capacity paths |
| Flash-cartridge ROM erase, program and verify | Supported NOR profiles | Supported NOR profiles |
| Save backup and restore | SRAM; MBC2 512-byte RAM; MBC7 256-byte EEPROM | SRAM/FRAM; 512-byte and 8 KiB EEPROM; 64/128 KiB Flash |
| RTC read, backup, restore and manual editing | Supported MBC3 / MBC30 clock cartridges | Cartridges with a detected, supported RTC |
| Interface | Chinese / English; light / dark themes | Chinese / English; light / dark themes |

GB/GBC and GBA have separate platform-specific handling. Pocket system menus and output filenames remain in English. Backup filenames include identifiable game information where available.

RTC options appear only when support is detected. RTC restoration also requires clock data in the selected save, with optional elapsed-time compensation. GB RTC editing does not offer synchronization to the Pocket calendar.

## 📦 Installation

You need an openFPGA-capable Analogue Pocket, a microSD card and your cartridge.

1. Obtain the release file `ChisKit-0.01b-firmware.zip`. Back up important microSD files first.
2. Extract it and merge `Assets`, `Cores`, `Platforms` and `Saves` from `Firmware/SD_ROOT/` into the card root. Do not delete existing directories.
3. If transferring over USB, safely eject the device and leave USB mode when finished.
4. Launch ChisKit from openFPGA on your Pocket.

Update the application, OS and FPGA files together; do not mix versions. Installation does not require compilation. The source archive is for inspection, modification and rebuilding. No commercial game ROMs or user saves are included.

## 💾 Usage

1. After changing cartridges, use **Reset** to clear the previous cartridge's detection state.
2. Select **Analyze cartridge** and check the game, mapper, ROM capacity and save type.
3. Back up the ROM or save before attempting any write operation.
4. Before programming a ROM or restoring a save, load the appropriate file through the Pocket menu and verify the target cartridge and capacity.
5. Wait for completion. Do not remove the cartridge, interrupt power or enter USB mode during an operation.

To edit the clock, analyze the cartridge first. Once RTC support is confirmed, press **X** on the main screen to open the platform-specific RTC editor. Read and freely edit the clock values, then follow the on-screen prompts to confirm writing them to the cartridge.

The ROM and SAV file selectors can access `Saves/chiskit/common/`. Output files use the same directory so backups can be selected for restoration. Never restore a different game's save by mistake.

A disabled action usually means analysis is incomplete, the type is unresolved, or the operation is unsupported. Manual save-type selection does not make arbitrary hardware safe to write.

## ⚠️ Compatibility and limitations

- MBC6, 2 KiB RAM and MBC7 512-byte save operations are not implemented.
- A depleted battery or RTC communication failure may make clock features unavailable.

Hardware test examples include Pokémon titles, a Zelda flash cartridge, Tenchi o Kurau and Korokoro Kirby on GB/GBC, plus Wario Land 4, Dragon Ball: Advance Adventure, The Minish Cap, Golden Sun and Pokémon Emerald on GBA. These examples do not guarantee every region, board revision or chip batch.

## 🔧 Source and building

The project includes application code, RTL, modified openfpgaOS, SDK, build scripts and resource inputs. Use the complete source package for your first rebuild so dependencies ignored by the development checkout are not omitted.

To build the application and OS on Linux / WSL, run:

```bash
export OPENFPGA_SDK="$PWD/dependencies/openfpgaSDK"
bash tests/build-gb-rtc189.sh release001b
```

The verified environment uses RISC-V GCC 13.2.0. Other tools include make, Python 3, host GCC and Icarus Verilog for RTL simulation. The FPGA project is `quartus/ap_core.qpf`, built with Quartus Prime Lite 25.1std.

## 💬 Feedback

Include the version, platform, retail or flash-cartridge status, game name, mapper / NOR / save type, reproduction steps, error code and screenshots or logs. Do not publicly upload commercial ROMs or private save data.

## 📚 Credits and licensing

Developed by **Alius**.

Thanks to [MoriBunrnner-for-ESP32-S3](https://github.com/moribaka/MoriBunrnner-for-ESP32-S3) for implementation references and adapted code, [FlashGBX](https://github.com/lesserkuma/FlashGBX) for cartridge and save handling references, and openfpgaOS, openfpgaSDK and their contributors. ChisKit is not an official product of those projects or Analogue.

Original and related derived code is distributed as described in [LICENSE.md](LICENSE.md). Third-party components retain their own licenses. See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for provenance and modification notes.

The interface uses **Vonwaon Bitmap Font (凤凰点阵体)** by **Haoyu Qiu**. Thanks to the author for providing the font. See the [author's release page](https://timothyqiu.itch.io/vonwaon-bitmap).

## ☕ Buy me a coffee

If ChisKit has been helpful, consider buying me a coffee to support continued development. Thank you for your support!

Scan the QR code below with Alipay to make an optional donation. All features remain available without donating.

<p align="center">
  <img src="docs/images/alipay-support.png" width="300" alt="Alipay donation QR code" />
</p>
