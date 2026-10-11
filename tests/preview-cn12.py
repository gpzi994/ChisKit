"""Static UI proposal; does not modify app, firmware, or TF files."""
from pathlib import Path
import re
from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'build' / 'cn12-preview'
OUT.mkdir(parents=True, exist_ok=True)
font = ImageFont.truetype('C:/ONS_FPGA/方正像素12.ttf', 12)
ascii_font = bytes(int(x, 16) for x in re.findall(r'0x([0-9A-Fa-f]{2})', (ROOT / 'app/ui97/font.c').read_text()))
BG, INK, MUTED, BLUE, BORDER = '#f3f7fc', '#172f49', '#62778d', '#1875e8', '#c8d6e5'

def text(im, xy, value, color=INK):
    x, y = xy
    for ch in value:
        if ord(ch) < 128:
            draw = ImageDraw.Draw(im)
            for row, bits in enumerate(ascii_font[ord(ch)*8:ord(ch)*8+8]):
                for col in range(8):
                    if bits & (128 >> col): draw.point((x+col, y+3+row), fill=color)
            x += 8
        else:
            # Binary glyph mask, no gray antialiasing or scaling at native size.
            mask = Image.new('1', (14, 15))
            ImageDraw.Draw(mask).text((0, 0), ch, font=font, fill=1)
            im.paste(color, (x, y), mask)
            x += 12
    assert x <= 320, (value, x)

def base():
    im = Image.new('RGB', (320, 288), BG)
    d = ImageDraw.Draw(im)
    d.rectangle((0, 0, 319, 25), fill='#d6e4f3')
    d.rectangle((7, 7, 20, 21), fill='#f5c64a', outline=INK)
    d.rectangle((10, 4, 17, 8), fill='#f5c64a')
    text(im, (27, 6), 'ChisBurner')
    text(im, (226, 6), 'UI preview', MUTED)
    return im

im = base()
text(im, (9, 34), '平台：GBA', MUTED)
text(im, (170, 34), '状态：已识别', '#0a805f')
text(im, (9, 53), 'POKEMON EMER', INK)
labels = ['刷新', '分析卡带', '备份 ROM', '备份存档', '写入 ROM', '恢复存档', '卡带信息', '设置']
for i, label in enumerate(labels):
    x, y = 7 + i % 2 * 156, 77 + i // 2 * 39
    d = ImageDraw.Draw(im)
    d.rectangle((x, y, x+149, y+33), fill='#e3efff' if i==1 else 'white', outline=BLUE if i==1 else BORDER)
    if i==1: d.rectangle((x+2, y+3, x+4, y+30), fill=BLUE)
    # Small cartridge symbol; all strokes align with framebuffer pixels.
    d.rectangle((x+12, y+11, x+24, y+23), outline=INK)
    d.rectangle((x+15, y+8, x+21, y+11), outline=INK)
    text(im, (x+35, y+10), label)
text(im, (9, 236), '备份文件名保持英文', MUTED)
ImageDraw.Draw(im).line((7, 259, 312, 259), fill=BORDER)
text(im, (9, 267), '方向键选择  A确认  X时钟', MUTED)
im.save(OUT / 'functions-320x288.png')

settings = base()
text(settings, (12, 40), '设置', BLUE)
d = ImageDraw.Draw(settings)
d.rectangle((8, 67, 311, 111), fill='white', outline=BLUE)
text(settings, (20, 81), '语言', INK)
text(settings, (108, 81), '中文 / English', BLUE)
text(settings, (14, 133), '首次启动默认中文', MUTED)
text(settings, (14, 155), '系统菜单入口保持英文', MUTED)
text(settings, (14, 177), '目录和备份文件名保持英文', MUTED)
text(settings, (14, 209), '仅切换应用界面语言', INK)
d.line((7, 259, 312, 259), fill=BORDER)
text(settings, (9, 267), '左右切换  A保存  B返回', MUTED)
settings.save(OUT / 'settings-320x288.png')
sheet = Image.new('RGB', (656, 288), '#c8d6e5')
sheet.paste(im, (0, 0)); sheet.paste(settings, (336, 0))
sheet.resize((1312, 576), Image.Resampling.NEAREST).save(OUT / 'preview-2x.png')
print(OUT)
