# ChisKit 0.01b - third-party sources and notices

Updated 2026-10-11. This inventory distinguishes component licenses from a
blanket license for the archive. No upstream endorsement is claimed.

| Component / author | Source and revision | Use and changes | License / retained notices |
| --- | --- | --- | --- |
| MoriBunrnner-for-ESP32-S3 / original project authors | User-supplied snapshot, imported 2026-09-09; no Git metadata | NOR database, lookup and derived cartridge routines; renamed identifiers, ported platform access, added profiles and independent FPGA execution | AGPLv3; LICENSES/AGPL-3.0-only.txt, core/nor/mori/PROVENANCE.md and COPYING; original hashes are recorded there |
| FlashGBX / Lesserkuma and contributors | https://github.com/lesserkuma/FlashGBX ; baseline 0272d8ef039b2e131a7278c1a3b6dd4ade5a7ecd (5.1); other reference revisions are preserved in source comments | ROM database indexes, mapper/save/RTC detection and transfer references/adaptations; ported to C/RTL, SDRAM file buffering and Pocket UI; see core/save, app/ui98 and source comments | GPLv3; LICENSES/GPL-3.0.txt; upstream notices preserved in docs/licenses/FlashGBX-Third-Party-Notices.md (inventory, not a claim that all listed dependencies are shipped) |
| openfpgaOS / upstream contributors | Local openfpgaCore baseline 453a28350dab333b3afd520f8f8ac4508641bb3a | Modified OS, file services and FPGA integration | .integration/openfpgaos/LICENSE and NOTICE; nested components keep their own licenses |
| openfpgaSDK / upstream contributors | a408ddc12aed0dfaa4aa22c06af82f829db77126 | Build interfaces and libraries | dependencies/openfpgaSDK/LICENSE, NOTICE and LICENSES |
| Analogue / open-fpga core-template | da3a021b1eaf742604d86d8dc9b33a6666263e6a | Pocket framework, pins and integration | No general repository license identified in the reviewed snapshot; platform terms apply. See docs/source-manifest.md. Do not claim this framework is relicensed by ChisKit |
| Phoenix / Vonwaon Bitmap / Haoyu Qiu | https://timothyqiu.itch.io/vonwaon-bitmap | 12px font converted to a UI-only bitmap subset | Author declares CC0; exact local source and unresolved upstream/redistributor rights are documented in assets/fonts/README.md |

ChisKit changes were developed during 2026; this release snapshot is dated
2026-10-11. Detailed adaptation notes and original copyright notices remain
in the individual files. Merely studying hardware protocols does not imply
that every reference project is incorporated; copied/adapted material must
retain its applicable license regardless of language or identifier changes.

The original FlashGBX program and its Python runtime are not bundled as an
application. Included database inputs are data/chisburner/db_AGB.json and
db_DMG.json; generators are in tools/. Do not substitute database versions
without checking generator hashes. Proprietary toolchains (Quartus) are
prerequisites, not included or licensed by this project.

Remaining release-review items: the font's underlying glyph rights and
platform/vendor framework terms are not certified by this notice. Source
availability and attribution alone do not resolve those questions.
