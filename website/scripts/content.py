#!/usr/bin/env python3
"""The content of the site, which the app shares part of.

    content.py qrc          prints the <file> entries of website/website.qrc.in
    content.py donate-url   prints the address of the donation page

website/content/ holds one folder per language. English is the source: a
language that leaves out a file or a picture takes the English one. The build
of the app and every script that reads the content find the files through this
module, so each one falls back to English the same way.

website/build-config/build.yaml is read here too, with PyYAML.
"""

import sys
from pathlib import Path

SITE = Path(__file__).resolve().parents[1]
CONTENT = SITE / "content"
CONF_FILE = SITE / "build-config" / "build.yaml"

# The language that every other one falls back to
SOURCE = "en"

# The folder of pictures inside a language folder, and the path a panel writes
PICTURES = "images"

# The short strings of a language that no panel holds: the words of the footer,
# and the words that ask for a donation. The app embeds the same file, and reads
# the donation out of it.
STRINGS = "strings.yaml"

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


def read_conf() -> dict:
    """website/build-config/build.yaml, as a mapping."""
    import yaml

    if not CONF_FILE.is_file():
        sys.exit(f"{CONF_FILE} does not exist, and it holds the configuration of the site")

    conf = yaml.safe_load(CONF_FILE.read_text(encoding="utf-8")) or {}
    if not isinstance(conf, dict):
        sys.exit(f"{CONF_FILE}: the file holds no mapping of names to settings")
    return conf


def link_url(link) -> str:
    """The address of an entry of 'footer: links' in build.yaml.

    An entry is an address, or a mapping that holds it under 'url'.
    """
    return link["url"] if isinstance(link, dict) else link


def donate_url(conf: dict) -> str:
    """The donation page, which is the 'donate' entry of 'footer: links'."""
    link = (conf.get("footer") or {}).get("links", {}).get("donate")
    if not link:
        sys.exit(f"{CONF_FILE}: 'footer: links' names no 'donate', the entry "
                 f"that links the donation page")
    return link_url(link)


def qrc_entries() -> list[str]:
    """The files of the site that the app embeds, under ':/website/<lang>/'.

    Every language gets the manual, the strings file and every picture, the
    English file standing in for one that the language leaves out.
    """
    def entry(alias: str, path: Path) -> str:
        return f'<file alias="{alias}">{path}</file>'

    entries = []
    for lang in languages():
        entries.append(entry(f"{lang}/{APP_MANUAL}", file_of(lang, MANUAL)))
        entries.append(entry(f"{lang}/{STRINGS}", file_of(lang, STRINGS)))
        entries += [entry(f"{lang}/{PICTURES}/{name}", path)
                    for name, path in pictures(lang).items()]
    return entries


def main() -> int:
    commands = {
        "qrc": lambda: "\n".join(qrc_entries()),
        "donate-url": lambda: donate_url(read_conf()),
    }
    if len(sys.argv) != 2 or sys.argv[1] not in commands:
        sys.exit(__doc__)

    print(commands[sys.argv[1]]())
    return 0


if __name__ == "__main__":
    sys.exit(main())
