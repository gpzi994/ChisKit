"""Run the production progress renderer with a simulated VSync wait."""
from pathlib import Path
import subprocess
import tempfile
root=Path(__file__).resolve().parents[1]
s=(root/'app/ui98/nor108_transfer.h').read_text()
fn=s[s.index('static void progress108('):s.index('static int program_nor108(')]
code='''
#include <stdint.h>
#include <string.h>
#include <assert.h>
struct U {const char *phase; uint32_t done,total,elapsed_ms,phase_ms;} u,*ui=&u;
static uint32_t tick,began,phase_began,paints;
static uint32_t now(void){return tick;}
static void paint(void){paints++;tick+=17;}
'''+fn+'''
int main(void){
 progress108("program",0,512000);assert(paints==1);
 for(unsigned i=1;i<1000;i++){tick++;progress108("program",i*512,512000);}
 assert(paints==10);assert(u.done==999*512);
 progress108("program",512000,512000);assert(paints==11);
 progress108("verify",512,512000);assert(paints==12);
 tick=0xfffffff0u;progress108("wrap",0,512000);
 unsigned p=paints;tick+=100;progress108("wrap",512,512000);assert(paints==p+1);
}
'''
with tempfile.TemporaryDirectory() as td:
    c=Path(td)/'test.c';exe=Path(td)/'test'
    c.write_text(code)
    subprocess.run(['gcc','-std=c11','-Wall','-Wextra','-Werror',str(c),'-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
print('PASS production progress: 1000 updates use 11 paints, transition/completion immediate, timer wrap safe')
