#!/usr/bin/env python3
"""The content of the site, which the app shares part of.

    content.py qrc          prints the <file> entries of website/website.qrc.in

website/content/ holds one folder per language. English is the source: a
language that leaves out a file or a picture takes the English one. The build
of the app and every script that reads the content find the files through this
module, so each one falls back to English the same way.
"""

import sys
from pathlib import Path

SITE = Path(__file__).resolve().parents[1]
CONTENT = SITE / "content"

# The language that every other one falls back to
SOURCE = "en"

# The folder of pictures inside a language folder, and the path a panel writes
PICTURES = "images"

# The documentation panel of the site is the manual of the app. The app reads it
# under the name 'documentation.md', without the place of the panel in the page
MANUAL = "4-documentation.md"
APP_MANUAL = "documentation.md"


def languages() -> list[str]:
    """Every language of the site, English first.

    A folder of content/ counts as a language when it holds a markdown file, so
    a new language is a folder and the files in it.
    """
    folders = sorted(folder.name for folder in CONTENT.iterdir()
                     if folder.is_dir() and any(folder.glob("*.md")))
    return [SOURCE] + [name for name in folders if name != SOURCE]


def file_of(lang: str, name: str) -> Path:
    """content/<lang>/<name>, or the English file when that language has none."""
    translated = CONTENT / lang / name
    return translated if translated.is_file() else CONTENT / SOURCE / name


def pictures(lang: str) -> dict[str, Path]:
    """Every picture that a page of one language shows, by file name.

    The English pictures come first, and the pictures of the language replace
    them. A language that translates one screenshot keeps the English picture
    of every other panel.
    """
    found = {}
    for folder in dict.fromkeys((SOURCE, lang)):
        path = CONTENT / folder / PICTURES
        if path.is_dir():
            found.update((picture.name, picture) for picture in sorted(path.iterdir())
                         if picture.is_file())
    return found


def qrc_entries() -> list[str]:
    """The files of the site that the app embeds, under ':/website/<lang>/'.

    Every language gets the manual and every picture, the English file standing
    in for one that the language leaves out.
    """
    def entry(alias: str, path: Path) -> str:
        return f'<file alias="{alias}">{path}</file>'

    entries = []
    for lang in languages():
        entries.append(entry(f"{lang}/{APP_MANUAL}", file_of(lang, MANUAL)))
        entries += [entry(f"{lang}/{PICTURES}/{name}", path)
                    for name, path in pictures(lang).items()]
    return entries


def main() -> int:
    commands = {
        "qrc": lambda: "\n".join(qrc_entries()),
    }
    if len(sys.argv) != 2 or sys.argv[1] not in commands:
        sys.exit(__doc__)

    print(commands[sys.argv[1]]())
    return 0


if __name__ == "__main__":
    sys.exit(main())
