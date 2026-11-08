#!/usr/bin/env python3
"""Find the site content files, some of which the app also embeds.

    content.py qrc          prints the <file> entries for the website.qrc that
                            website/meson.build writes
    content.py header VERSION
                            prints buildconfig.h, the C++ header with the app
                            version, site address, GitHub repository, author
                            email address and donation page

website/content/ holds one folder per language. English is the source: a
language without a given file or picture uses the English one. The app build
and every script that reads the content use this module, so they all fall back
to English the same way.

This module also reads website/build-config/build.yaml, with PyYAML.
"""

import re
import sys
from pathlib import Path

SITE = Path(__file__).resolve().parents[1]
CONTENT = SITE / "content"
CONF_FILE = SITE / "build-config" / "build.yaml"

# The fallback language
SOURCE = "en"

# The pictures folder in each language folder, and the path that panels use
PICTURES = "images"

# The right-to-left languages. Their pages and app windows are mirrored
RIGHT_TO_LEFT = {"ar", "fa", "he", "ur"}

# A language's short strings outside the panels, such as the footer text. The
# app embeds the same file for the donation appeal.
STRINGS = "strings.yaml"

# The site documentation panel is the app manual. The app embeds it as
# 'documentation.md', without the panel number prefix
MANUAL = "4-documentation.md"
APP_MANUAL = "documentation.md"

# A picture in a panel, '![...](images/overview.png)', capturing its file name
SHOWN_PICTURE = re.compile(rf"\]\({PICTURES}/([^)\s]+)\)")


def languages() -> list[str]:
    """Every site language, English first.

    Any content/ folder with a markdown file counts as a language, so adding a
    language only takes a new folder.
    """
    folders = sorted(folder.name for folder in CONTENT.iterdir()
                     if folder.is_dir() and any(folder.glob("*.md")))
    return [SOURCE] + [name for name in folders if name != SOURCE]


def file_of(lang: str, name: str) -> Path:
    """content/<lang>/<name>, or the English file if the language has none."""
    translated = CONTENT / lang / name
    return translated if translated.is_file() else CONTENT / SOURCE / name


def pictures(lang: str) -> dict[str, Path]:
    """A language's pictures, by file name.

    The language's own pictures replace the English ones with the same name,
    so a language can translate one screenshot and keep English for the rest.
    """
    found = {}
    for folder in dict.fromkeys((SOURCE, lang)):
        path = CONTENT / folder / PICTURES
        if path.is_dir():
            found.update((picture.name, picture) for picture in sorted(path.iterdir())
                         if picture.is_file())
    return found


def captions(lang: str) -> dict[str, str]:
    """A language's gallery captions, by file name.

    They come from the 'gallery' mapping in content/<lang>/strings.yaml. The
    site uses them as alt text, and the metainfo file as screenshot captions.
    """
    import yaml

    path = CONTENT / lang / STRINGS
    strings = yaml.safe_load(path.read_text(encoding="utf-8")) if path.is_file() else None
    found = strings.get("gallery") if isinstance(strings, dict) else None
    return found if isinstance(found, dict) else {}


def read_conf() -> dict:
    """website/build-config/build.yaml, as a dict."""
    import yaml

    if not CONF_FILE.is_file():
        sys.exit(f"{CONF_FILE} is missing: it holds the site configuration")

    conf = yaml.safe_load(CONF_FILE.read_text(encoding="utf-8")) or {}
    if not isinstance(conf, dict):
        sys.exit(f"{CONF_FILE}: the file must be a YAML mapping")
    return conf


def link_url(link) -> str:
    """The URL of a 'footer: links' entry in build.yaml.

    An entry is a URL, or a mapping with a 'url' key.
    """
    return link["url"] if isinstance(link, dict) else link


def donate_url(conf: dict) -> str:
    """The donation page: the 'donate' entry in 'footer: links'."""
    link = (conf.get("footer") or {}).get("links", {}).get("donate")
    if not link:
        sys.exit(f"{CONF_FILE}: 'footer: links' has no 'donate' entry for the "
                 f"donation page")
    return link_url(link)


def homepage_url(conf: dict) -> str:
    """The site address: 'homepage' in build.yaml."""
    url = conf.get("homepage")
    if not url:
        sys.exit(f"{CONF_FILE}: 'homepage' is empty")
    return url


def repository_url(conf: dict) -> str:
    """The ZeGrapher GitHub repository: 'repository' in build.yaml."""
    url = conf.get("repository")
    if not url:
        sys.exit(f"{CONF_FILE}: 'repository' is empty")
    return url


def contact_address(conf: dict) -> str:
    """The author's email address, from the 'contact' entry in 'footer: links'."""
    link = (conf.get("footer") or {}).get("links", {}).get("contact")
    url = link_url(link) if link else ""
    if not url.startswith("mailto:"):
        sys.exit(f"{CONF_FILE}: 'footer: links' needs a 'contact' entry that "
                 f"starts with 'mailto:'")
    return url.removeprefix("mailto:")


def qrc_entries() -> list[str]:
    """The site files that the app embeds, under ':/website/<lang>/'.

    Each language gets the manual, the strings file and the manual pictures,
    with English files for the missing ones. Site-only pictures, such as the
    landing panel picture, stay out of the app.
    """
    def entry(alias: str, path: Path) -> str:
        return f'<file alias="{alias}">{path}</file>'

    entries = []
    for lang in languages():
        manual = file_of(lang, MANUAL)
        shown = set(SHOWN_PICTURE.findall(manual.read_text(encoding="utf-8")))
        entries.append(entry(f"{lang}/{APP_MANUAL}", manual))
        entries.append(entry(f"{lang}/{STRINGS}", file_of(lang, STRINGS)))
        entries += [entry(f"{lang}/{PICTURES}/{name}", path)
                    for name, path in pictures(lang).items() if name in shown]
    return entries


def cpp_string(value: str) -> str:
    """The value as a C++ string literal."""
    return '"' + value.replace("\\", "\\\\").replace('"', '\\"') + '"'


def header(conf: dict, version: str) -> str:
    """buildconfig.h: the app version and the build.yaml addresses as C++
    macros."""
    macros = {"SOFTWARE_VERSION": version,
              "HOMEPAGE_URL": homepage_url(conf),
              "REPOSITORY_URL": repository_url(conf),
              "CONTACT_ADDRESS": contact_address(conf),
              "DONATE_URL": donate_url(conf)}
    lines = ["// website/scripts/content.py writes this file from the meson.build "
             "version and website/build-config/build.yaml",
             "#pragma once"]
    lines += [f"#define {name} {cpp_string(value)}" for name, value in macros.items()]
    return "\n".join(lines)


def main() -> int:
    match sys.argv[1:]:
        case ["qrc"]:
            print("\n".join(qrc_entries()))
        case ["header", version]:
            print(header(read_conf(), version))
        case _:
            sys.exit(__doc__)
    return 0


if __name__ == "__main__":
    sys.exit(main())
