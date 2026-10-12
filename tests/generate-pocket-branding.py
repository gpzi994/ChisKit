"""Generate Pocket artwork; requires Pillow. Sources are retained for rebuilding.

BIN: rotated 90 degrees CCW, two bytes per pixel (inverted gray, zero).
This matches openFPGA Library's little-endian brightness decode and inversion.
"""
from pathlib import Path
from PIL import Image, ImageOps
import argparse

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--sd-root', type=Path)
args = parser.parse_args()
src = root / 'assets/branding'
out = root / 'platform/openfpga'
for name, size, relative in [
    ('banner', (521, 165), 'Platforms/_images/chiskit.bin'),
    ('icon', (36, 36), 'Cores/Alius.chiskit/icon.bin'),
]:
    im = Image.open(src / (name + '-source.png')).convert('L')
    im = im.resize(size, Image.Resampling.LANCZOS)
    if name == 'icon':
        im = im.point(lambda v: 255 if v >= 128 else 0)
    im.save(src / (name + '-preview.png'))
    rotated = ImageOps.invert(im).transpose(Image.Transpose.ROTATE_90)
    data = bytes(b for v in rotated.tobytes() for b in (v, 0))
    dest = out / relative
    dest.parent.mkdir(parents=True, exist_ok=True)
    dest.write_bytes(data)
    assert len(data) == size[0] * size[1] * 2
    decoded = ImageOps.invert(Image.frombytes('L', (size[1], size[0]), data[::2])).transpose(Image.Transpose.ROTATE_270)
    assert decoded.tobytes() == im.tobytes()
    if args.sd_root:
        target = args.sd_root / relative
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(data)
    print(relative, len(data), 'round-trip PASS')
