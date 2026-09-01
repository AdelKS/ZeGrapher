#!/usr/bin/env python3
"""make-icons.py

Write every raster of the app icon out of ZeGrapher.svg, the one file that
holds the drawing. Run this command from this folder after each change of that
file:

    ./make-icons.py

The command writes, in this folder:

- ZeGrapher_16.png up to ZeGrapher_512.png, the sizes that meson installs
- ZeGrapher.ico, the icon of the Windows build
- ZeGrapher.icns, the icon of the macOS bundle

The command writes two more files out of the same drawing:

- ../website/assets/images/ZeGrapher-logo.svg, the logo of the website
- ../website/assets/images/ZeGrapher-logo.png, its favicon

Requires: rsvg-convert (librsvg) on PATH. Python 3.12+, Pillow.
"""

import io
import struct
import subprocess
import sys
from pathlib import Path

from PIL import Image

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent
SOURCE = HERE / "ZeGrapher.svg"
WEBSITE_IMAGES = ROOT / "website" / "assets" / "images"

# the sizes of icons/hicolor, and the picture that the website shows
PNG_SIZES = (16, 24, 32, 48, 64, 128, 256, 512)
WEBSITE_SIZE = 128

# what Windows reads out of an .ico, and the members that iconutil writes in an
# .icns: the type, then the pixels of the picture it holds
ICO_SIZES = (16, 24, 32, 48, 64, 128, 256)
ICNS_MEMBERS = (
    ("icp4", 16), ("icp5", 32), ("ic11", 32), ("ic12", 64), ("ic07", 128),
    ("ic13", 256), ("ic08", 256), ("ic14", 512), ("ic09", 512), ("ic10", 1024),
)


def render(size: int) -> bytes:
    """The drawing, rasterized at 'size' by 'size' pixels."""
    done = subprocess.run(
        ["rsvg-convert", "--width", str(size), "--height", str(size), str(SOURCE)],
        capture_output=True,
    )
    if done.returncode != 0:
        sys.exit(f"rsvg-convert: {done.stderr.decode().strip()}")
    return done.stdout


def icns(rasters: dict[int, bytes]) -> bytes:
    """The rasters, in the container that macOS reads."""
    members = b"".join(
        kind.encode("ascii") + struct.pack(">I", len(rasters[size]) + 8) + rasters[size]
        for kind, size in ICNS_MEMBERS
    )
    return b"icns" + struct.pack(">I", len(members) + 8) + members


def main() -> None:
    sizes = {*PNG_SIZES, *ICO_SIZES, *(size for _, size in ICNS_MEMBERS), WEBSITE_SIZE}
    rasters = {size: render(size) for size in sorted(sizes)}

    written = []
    for size in PNG_SIZES:
        out = HERE / f"ZeGrapher_{size}.png"
        out.write_bytes(rasters[size])
        written.append(out)

    # Pillow drops a size larger than the picture it saves, so the largest
    # raster is the one that carries the others
    pictures = [Image.open(io.BytesIO(rasters[size])) for size in sorted(ICO_SIZES, reverse=True)]
    ico = HERE / "ZeGrapher.ico"
    pictures[0].save(ico, format="ICO", sizes=[(size, size) for size in ICO_SIZES],
                     append_images=pictures[1:])
    written.append(ico)

    mac = HERE / "ZeGrapher.icns"
    mac.write_bytes(icns(rasters))
    written.append(mac)

    logo = WEBSITE_IMAGES / "ZeGrapher-logo.svg"
    logo.write_text(SOURCE.read_text())
    written.append(logo)
    favicon = WEBSITE_IMAGES / "ZeGrapher-logo.png"
    favicon.write_bytes(rasters[WEBSITE_SIZE])
    written.append(favicon)

    for out in written:
        print(out.relative_to(ROOT))


if __name__ == "__main__":
    main()
