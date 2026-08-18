#!/usr/bin/env python3
"""
Write one icon file per Font Awesome glyph, fetched from Font Awesome.

The project publishes one SVG per icon. This reads those files and writes them
to assets/icons/ in the form that folder holds: a viewBox, a path, and no fill. The
upstream file carries attributes for the JavaScript of Font Awesome, and a long
license comment, and both are dropped.

Usage:    ./fetch_icon.py home list images
Requires: network access. Python 3.12+, standard library only.
"""

import argparse
import re
import sys
import urllib.error
import urllib.request
from pathlib import Path

SOURCE = ("https://raw.githubusercontent.com/FortAwesome/Font-Awesome/"
          "{version}/svgs/{style}/{name}.svg")
SVG_OPEN = re.compile(r"<svg\b[^>]*>", re.S)
VIEWBOX = re.compile(r'\bviewBox="([^"]+)"')
XML_COMMENT = re.compile(r"<!--.*?-->", re.S)
FILL_ATTR = re.compile(r'\s*\bfill="[^"]*"')


def fetch(url: str, name: str) -> str:
    try:
        with urllib.request.urlopen(url, timeout=30) as answer:
            return answer.read().decode("utf-8")
    except urllib.error.HTTPError as error:
        if error.code == 404:
            sys.exit(f"'{name}' is not an icon of this style: {url}")
        sys.exit(f"{url}: HTTP {error.code}")
    except urllib.error.URLError as error:
        sys.exit(f"{url}: {error.reason}")


def house_form(svg: str, name: str, notice: str) -> str:
    """The upstream file, cut down to what an icon of this folder holds."""
    open_tag = SVG_OPEN.search(svg)
    box = VIEWBOX.search(open_tag.group(0)) if open_tag else None
    if box is None:
        sys.exit(f"'{name}': what came back is not an <svg> with a viewBox")
    inner = svg[open_tag.end():svg.rindex("</svg>")]
    # 'fill: currentColor' in main.css paints an icon, so a fill here would win.
    inner = FILL_ATTR.sub("", XML_COMMENT.sub("", inner)).strip()
    return (f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="{box.group(1)}">'
            f"{notice}{inner}</svg>\n")


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--version", default="5.15.4",
                    help="tag of the Font Awesome repository (default: 5.15.4)")
    ap.add_argument("--style", default="solid",
                    choices=("solid", "regular", "brands"))
    ap.add_argument("--out", type=Path, default=Path(__file__).resolve().parents[1] / "assets" / "icons")
    ap.add_argument("name", nargs="+", help="Font Awesome names, such as 'home'")
    args = ap.parse_args()

    notice = (f"<!-- Font Awesome Free {args.version} {args.style}, CC BY 4.0 "
              f"(fontawesome.com/license/free) -->")

    for name in args.name:
        url = SOURCE.format(version=args.version, style=args.style, name=name)
        out = args.out / f"{name}.svg"
        out.write_text(house_form(fetch(url, name), name, notice))
        print(f"{out}  {url}")


if __name__ == "__main__":
    main()
