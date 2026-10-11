"""Generate only UI glyphs from the user supplied font (build-time only)."""
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont
import hashlib
import argparse
root=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser()
parser.add_argument('--check', action='store_true', help='Verify without modifying the generated header')
args=parser.parse_args()
source=root/'assets/fonts/vonwaon-12px.ttf'
assert hashlib.sha256(source.read_bytes()).hexdigest()=='9e548fcc849320f61c91335d9a75fc5b22e62c6368c9e2ae1befb30d6d866cde', 'Font source hash mismatch'
font=ImageFont.truetype(str(source),12)
# Include literal UI text too (e.g. the language selector's Chinese label).
texts=''.join((root/p).read_text(encoding='utf-8') for p in ('app/ui98/lang174.h','app/ui98/ui.c'))
chars=sorted(set(c for c in texts if ord(c)>127))
rows=['/* Generated subset; font SHA256 '+hashlib.sha256(source.read_bytes()).hexdigest()+' */',
      'struct cb_glyph174 {unsigned code; unsigned short rows[14];};',
      'static const struct cb_glyph174 cb_font174[]={']
for c in chars:
    assert bytes(font.getmask(c,mode='1'))!=bytes(font.getmask(chr(0x10ffff),mode='1')),('Missing glyph',c)
    assert font.getlength(c)==12,('Unexpected width',c)
    im=Image.new('1',(12,14));ImageDraw.Draw(im).text((0,0),c,font=font,fill=1)
    assert im.getbbox(),repr(c)
    bits=[sum((1<<(11-x)) for x in range(12) if im.getpixel((x,y))) for y in range(14)]
    rows.append('{0x%x,{%s}},'%(ord(c),','.join('0x%03x'%b for b in bits)))
rows.append('};')
output=root/'app/ui98/font174.h'
generated='\n'.join(rows)+'\n'
if args.check:
    assert output.read_text(encoding='ascii')==generated, 'Generated font differs from checked-in header'
else:
    output.write_text(generated,encoding='ascii')
print('Generated',len(chars),'glyphs; bitmap bytes',len(chars)*28)
