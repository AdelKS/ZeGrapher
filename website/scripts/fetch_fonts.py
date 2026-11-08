#!/usr/bin/env python3
"""Download the website font.

Writes woff2 files to <site-root>/assets/fonts/. <site-root> defaults to the
website folder, the parent of this script's folder.

Usage: fetch_fonts.py [site-root]
"""

import re
import sys
import urllib.request
from pathlib import Path

API = ("https://fonts.googleapis.com/css2"
       "?family=Source+Sans+Pro:ital,wght@0,300;0,400;1,300;1,400")

# Google Fonts serves woff2 only to the browsers it knows, based on the user
# agent. Unknown agents get ttf.
USER_AGENT = ("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36"
              " (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36")

# The family also has cyrillic and vietnamese subsets, which the site doesn't
# need. The greek subset stays for pi and other math letters.
SUBSETS = ("latin", "latin-ext", "greek")

# The API puts the subset name in a comment above each @font-face.
API_FACE = re.compile(r"/\* (\S+) \*/\s*@font-face \{(.*?)\}", re.S)

SITE = Path(__file__).resolve().parents[1]
CSS = SITE / "assets/css/main.css"
CSS_FACE = re.compile(r'url\("\.\./fonts/(\S+?)"\)[^}]*?unicode-range: (.*?);', re.S)


def read(url: str) -> bytes:
    request = urllib.request.Request(url, headers={"User-Agent": USER_AGENT})
    with urllib.request.urlopen(request, timeout=30) as response:
        return response.read()


def field(body: str, name: str) -> str:
    return re.search(rf"{name}: (.*?);", body, re.S).group(1).strip()


def fetch_fonts(dest: Path) -> None:
    """Write one woff2 file per weight, style and subset into dest."""
    dest.mkdir(parents=True, exist_ok=True)
    ranges = {}
    for subset, body in API_FACE.findall(read(API).decode("utf-8")):
        if subset not in SUBSETS:
            continue
        italic = field(body, "font-style") == "italic"
        weight = field(body, "font-weight")
        name = f"source-sans-pro-{weight}{'-italic' if italic else ''}-{subset}.woff2"
        print(f"fetch {name}")
        (dest / name).write_bytes(read(re.search(r"url\((\S+?)\)", body).group(1)))
        ranges[name] = field(body, "unicode-range")

    expected = 4 * len(SUBSETS)
    if len(ranges) != expected:
        raise SystemExit(f"got {len(ranges)} faces from Google Fonts, expected {expected}")

    # Browsers never download a font file without an @font-face rule, and a
    # changed range leaves characters to a fallback font. Fix either one by
    # hand in main.css.
    declared = dict(CSS_FACE.findall(CSS.read_text(encoding="utf-8")))
    if declared != ranges:
        print(f"warning: the @font-face rules in {CSS} no longer match Google Fonts",
              file=sys.stderr)


def main(argv: list[str]) -> None:
    if len(argv) > 2:
        raise SystemExit(f"usage: {Path(argv[0]).name} [site-root]")
    site = Path(argv[1]) if len(argv) == 2 else SITE
    fetch_fonts(site / "assets" / "fonts")


if __name__ == "__main__":
    main(sys.argv)
