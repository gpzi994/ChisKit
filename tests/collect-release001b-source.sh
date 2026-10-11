#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
dest="$PWD/build/publish/ChisKit-0.01b/Source"
mkdir -p "$dest/.integration" "$dest/dependencies/openfpgaSDK" "$dest/quartus" "$dest/docs"
tar --exclude=.git --exclude=build --exclude=bld --exclude=target --exclude=dist --exclude=output_files --exclude=db --exclude=incremental_db --exclude=__pycache__ --exclude='*.sav' --exclude='*.gba' --exclude='*.gb' --exclude='*.gbc' -cf - app core data hardware platform tests tools .gitignore | tar -xf - -C "$dest"
tar --exclude=.git --exclude=build --exclude=bld --exclude=target --exclude=output_files --exclude=db --exclude=incremental_db --exclude=__pycache__ --exclude='*.sav' --exclude='*.gba' --exclude='*.gb' --exclude='*.gbc' -C .integration -cf - openfpgaos | tar -xf - -C "$dest/.integration"
tar --exclude=.git --exclude=build --exclude=dist --exclude=releases --exclude=__pycache__ -C /home/lin13/work/ONS_FPGA/openfpgaSDK -cf - src scripts tools runtime LICENSE LICENSES NOTICE REUSE.toml Makefile README.md | tar -xf - -C "$dest/dependencies/openfpgaSDK"
cp build/gb-compare208-quartus/ap_core.qpf build/gb-compare208-quartus/firmware.mif "$dest/quartus/"
cp docs/GB_COMPARE208_VALIDATION.md docs/CHISKIT_GB208.md "$dest/docs/"
echo 'Source collection complete (no ROM/save files, build databases or Git metadata).'
