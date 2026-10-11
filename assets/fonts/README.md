# Font source and reproducible UI subset

The exact previously used user-supplied font is bundled as vonwaon-12px.ttf.
Original filename: 凤凰点阵体 12px_爱给网_aigei_com.ttf
SHA256: 9e548fcc849320f61c91335d9a75fc5b22e62c6368c9e2ae1befb30d6d866cde
It is a third-party download, not a verified byte-identical official archive.

Official author: Haoyu Qiu. Author page:
https://timothyqiu.itch.io/vonwaon-bitmap
Declared license: CC0 1.0, https://creativecommons.org/publicdomain/zero/1.0/
CC0 text: ../../LICENSES/CC0-1.0.txt.

IMPORTANT: the author acknowledges uncertain HZK/ASC upstream glyph rights
in the page comments. CC0 does not clear rights the declarant does not own.
Bundling this exact input restores reproducibility, not proof of permission
from every underlying rightsholder or third-party modifier. Resolve these
questions or replace the font before a rights-cleared public release.

Install Python 3 and Pillow, then run from the source root:

    python tests/generate-font174.py --check
    python tests/generate-font174.py

The first command checks without writing; the second regenerates
app/ui98/font174.h from the bundled font and UI text. Font input is hash
checked. Neither command uses a personal Downloads path. Existing firmware
builds can consume the checked-in header without installing Pillow.
