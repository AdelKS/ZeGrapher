#!/usr/bin/env python3
"""The releases of ZeGrapher.

Every script that reads the version of ZeGrapher goes through this file.
project_version() reads the version that meson.build names.
"""

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MESON_BUILD = ROOT / "meson.build"

# the project version, from the project() call in meson.build
MESON_VERSION = re.compile(r"^\s*version\s*:\s*'([^']+)'", re.M)


def project_version() -> str:
    """The version that meson.build names, such as '4.0.0_rc0-dev'."""
    match = MESON_VERSION.search(MESON_BUILD.read_text(encoding="utf-8"))
    if match is None:
        sys.exit(f"{MESON_BUILD} names no version, and the release under work "
                 f"follows it until it has a tag")

    return match.group(1)
