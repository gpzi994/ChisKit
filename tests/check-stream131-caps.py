"""Execute the production capability-query function with mocked MMIO."""
from pathlib import Path
import subprocess
import tempfile
import re

root = Path(__file__).resolve().parents[1]
src = (root / '.integration/openfpgaos/src/firmware/os/kernel/services_table.c').read_text()
fn = src[src.index('static int svc_cart_get_caps('):src.index('static int svc_cart_enter_safe_idle(')]
regs = (root / '.integration/openfpgaos/src/firmware/os/hal/regs.h').read_text()
defines = '\n'.join(line for line in regs.splitlines() if line.startswith('#define CART_HW_'))
script = (root / 'tests/build-stream131-os.sh').read_text()
assert '-DCART_S29GL256_PROGRAMMER=1' in script
assert '-DS29GL256_PROGRAMMER=1' not in script
prefix = r'''
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "of_cart.h"
static uint32_t mock_caps;
#define CART_CAPS mock_caps
#define CART_LIFECYCLE 0
#define CART_HOST_B1_RAW 0
#define CART_HOST_DIAG 0
static int svc_cart_caps_init(void *p, uint32_t n) { memset(p, 0, n); return 0; }
static int svc_cart_present(void) { return 1; }
static void svc_cart_caps_set_diag(of_cart_caps_t *p, uint32_t n) { (void)n; p->cart_caps_raw=mock_caps; }
static void svc_cart_caps_set_space_masks(of_cart_caps_t *p, uint32_t n, uint32_t g, uint32_t a) {
 (void)n; p->gb_gbc_space_mask=g; p->gba_space_mask=a;
}
'''
main = r'''
int main(void) {
 of_cart_caps_t c;
 mock_caps=0x02313f03;
 int rc=svc_cart_get_caps(&c,sizeof(c));
 fprintf(stderr,"enabled=%d hw=%08x rc=%d\n",CART_S29GL256_PROGRAMMER,mock_caps,rc);
#if CART_S29GL256_PROGRAMMER
 assert(rc==0); assert(c.bus_mask==3); assert(c.operation_mask==63);
 assert(c.max_read_bytes==32); assert(c.gb_gbc_space_mask==17); assert(c.gba_space_mask==17);
#else
 assert(rc==-10);
#endif
 mock_caps|=0x80000000u;
 assert(svc_cart_get_caps(&c,sizeof(c))==OF_ERR_INVALID_STATE);
 return 0;
}
'''
with tempfile.TemporaryDirectory() as td:
    path = Path(td) / 'caps.c'
    path.write_text(prefix + defines + '\n' + fn + main)
    for enabled in (0, 1):
        exe = Path(td) / f'caps{enabled}'
        subprocess.run(['gcc','-std=c11','-Wall','-Wextra','-Werror',
            f'-DCART_S29GL256_PROGRAMMER={enabled}',
            '-I'+str(root / '.integration/openfpgaos/src/firmware/api'),str(path),'-o',str(exe)],check=True)
        subprocess.run([str(exe)],check=True)
print('PASS: old flags reproduce rc -10; corrected flags accept Stream131 CAPS; unknown bits rejected')
