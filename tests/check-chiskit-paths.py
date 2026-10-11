from pathlib import Path
import subprocess, tempfile
root=Path(__file__).resolve().parents[1]
source=(root/'.integration/openfpgaos/src/firmware/os/kernel/services_table.c').read_text()
start=source.index('static int svc_output_path_allowed(')
end=source.index('static int svc_output_open(',start)
test=r'''
int main(void){
 assert(svc_output_path_allowed("/Saves/chiskit/common/chiskit-language.cfg"));
 assert(svc_output_path_allowed("/Saves/chiskit/common/GBA-test-backup.sav"));
 assert(svc_output_path_allowed("/Saves/chiskit/common/GB-test-backup.sav"));
 assert(!svc_output_path_allowed("/Saves/chisburner/common/test.sav"));
 assert(!svc_output_path_allowed("/Saves/chiskit/common/../test.sav"));
 assert(!svc_output_path_allowed("/Saves/chiskit/common/"));
 assert(!svc_output_path_allowed("/Saves/chiskit/common//test.sav"));
 assert(!svc_output_path_allowed("/Saves/chiskit/common/C:\\test.sav"));
 return 0;
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'paths.c';p.write_text('#include <stdint.h>\n#include <assert.h>\n'+source[start:end]+test)
 exe=str(Path(d)/'paths')
 subprocess.run(['gcc','-Wall','-Wextra','-Werror',str(p),'-o',exe],check=True)
 subprocess.run([exe],check=True)
print('chiskit path whitelist PASS: new root accepted, legacy/traversal rejected')
