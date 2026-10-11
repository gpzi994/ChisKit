# ChisFlash physical-card intake

Status: a physical ChisFlash card is available and is known by the user to
support ROM programming, ROM dumping, and save-data operations. Genuine GBA
cartridges are also available for ROM dumps and save-data operations. These
are test resources, not validation results; exact revisions, markings, known
images, and independent-reader baselines are not yet recorded. Do not insert
or energize any cartridge during the empty-slot smoke stage.

This record separates what is visible on the user's actual card from the
nominal ChisFlash v1.0 design reviewed in `docs/chisflash-1.0.md`.

## Identification

| Field | Actual card |
| --- | --- |
| Intake ID | TBD |
| PCB title/revision text | TBD |
| Acquisition/source | TBD |
| Known prior use | TBD |
| Known-good ROM image and hash | TBD |
| Trusted independent reader | TBD |
| Trusted-reader baseline date | TBD |

## Required photographs

- [ ] PCB front, entire board, straight-on and in focus
- [ ] PCB rear, entire board, straight-on and in focus
- [ ] Cartridge edge contacts
- [ ] Close-up of U2 marking
- [ ] Close-up of U4 marking
- [ ] Close-up of U5 marking
- [ ] Any bodge wires, rework, damage, corrosion, or missing components

Keep originals unchanged. Record filenames and SHA-256 hashes below rather
than pasting resized chat previews into the evidence directory.

| View | Filename | SHA-256 |
| --- | --- | --- |
| Front | TBD | TBD |
| Rear | TBD | TBD |
| U2 | TBD | TBD |
| U4 | TBD | TBD |
| U5 | TBD | TBD |

## Markings and comparison

| Reference | Nominal v1.0 part | Actual top marking | Match status |
| --- | --- | --- | --- |
| U2 | S29GL256S10TFI010 | TBD | UNVERIFIED |
| U4 | FM28V100-TG | TBD | UNVERIFIED |
| U5 | EPM240T100C5N | TBD | UNVERIFIED |

Do not infer an exact device from package appearance alone. A readable marking,
board connectivity, and the matching manufacturer datasheet are all required.

## Pre-power hold points

- [ ] H0 instruments and interposer are documented
- [ ] Physical GBA mode actuation is confirmed without software voltage control
- [ ] Ground reference and cartridge VCC test points are identified
- [ ] Current limiting/protection and abort procedure are recorded
- [ ] Exact U2/U4/U5 absolute maximum ratings are available
- [ ] A disposable known cartridge has passed H3 before using ChisFlash

Until every item is checked, ChisFlash remains an unpowered evidence sample.
Afterward, H4 is still read-only: normal header/ROM reads and repeatable hashes
only. Software ID/CFI entry, FRAM writes, Flash erase, and Flash program remain
forbidden.
