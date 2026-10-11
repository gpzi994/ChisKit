"""Compile the actual Pocket file reuse predicate, not a duplicate model."""
from pathlib import Path
import subprocess
import tempfile
root = Path(__file__).resolve().parents[1]
source = (root / '.integration/openfpgaos/src/firmware/os/targets/pocket/file.c').read_text()
start = source.index('static int gb180_backup_reusable(')
end = source.index('\n}\n', start) + 3
test = r'''
#include <assert.h>
#define CHECK(p,n,expected) assert(gb180_backup_reusable(p,strlen(p),n)==expected)
int main(void){
 CHECK("/Saves/chiskit/common/GB-GOLD-rtc-backup.bin",48,1);
 CHECK("/Saves/chiskit/common/GB-GOLD-backup.sav",32768,1);
 CHECK("/Saves/chiskit/common/GB-GOLD-rtc-backup.bin",32768,0);
 CHECK("/Saves/chiskit/common/GB-GOLD-backup.sav",48,0);
 CHECK("/Saves/chiskit/common/GBA-GOLD-rtc-backup.bin",48,0);
 CHECK("/Saves/chiskit/common/GB-../GOLD-rtc-backup.bin",48,0);
 CHECK("/Saves/chiskit/common/GB-GOLD.bin",48,0);
 return 0;
}
'''
with tempfile.TemporaryDirectory() as d:
    p = Path(d) / 'policy.c'
    p.write_text('#include <stdint.h>\n#include <string.h>\n#define CART_GB_RTC189 1\n#define CART_GB_RESTORE185 1\n' + source[start:end] + test)
    exe = str(Path(d) / 'policy')
    subprocess.run(['gcc','-Wall','-Wextra','-Werror',str(p),'-o',exe],check=True)
    subprocess.run([exe],check=True)
print('GB190 policy PASS: exact RTC backup only, size/prefix/traversal guards, RAM policy retained')
