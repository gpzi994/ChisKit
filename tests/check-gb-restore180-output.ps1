$ErrorActionPreference='Stop'
# Extract and compile the actual production output-open implementation,
# not a mock that merely promises the +1 result.
& "$PSScriptRoot/check-save160-output.ps1"
& wsl.exe -d Ubuntu-24.04 -- bash -lc 'cd /mnt/c/ONS_FPGA/work/ChisBurner && g++ -DCART_GB_RESTORE180=1 -std=c++17 -Wall -Wextra -Werror -fsanitize=undefined -I.integration/openfpgaos/src/firmware/api tests/test-save160-output.cpp -o build/gb180-output-test && build/gb180-output-test'
if($LASTEXITCODE){throw 'GB180 production output policy regression failed'}
