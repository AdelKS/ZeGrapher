#!/usr/bin/env python3
"""The content of the site, which the app shares part of.

    content.py qrc          prints the <file> entries of the website.qrc that
                            website/meson.build writes
    content.py header VERSION
                            prints buildconfig.h, the C++ header of the
                            version of the app, of the site, of the GitHub
                            repository, of the email address of the author and
                            of the donation page

website/content/ holds one folder per language. English is the source: a
language that leaves out a file or a picture takes the English one. The build
of the app and every script that reads the content find the files through this
module, so each one falls back to English the same way.

website/build-config/build.yaml is read here too, with PyYAML.
"""

import re
import sys
from pathlib import Path

SITE = Path(__file__).resolve().parents[1]
CONTENT = SITE / "content"
CONF_FILE = SITE / "build-config" / "build.yaml"

# The language that every other one falls back to
SOURCE = "en"

# The pictures folder inside a language folder, and the path panels use
PICTURES = "images"

# A language's short strings that no panel holds, such as the footer text. The
# app embeds the same file, and reads the donation appeal from it.
STRINGS = "strings.yaml"

# The site's documentation panel is the app manual. The app reads it as
# 'documentation.md', without the panel's position prefix
MANUAL = "4-documentation.md"
APP_MANUAL = "documentation.md"

# A picture that a panel shows, '![...](images/overview.png)', and its file name
SHOWN_PICTURE = re.compile(rf"\]\({PICTURES}/([^)\s]+)\)")


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


def homepage_url(conf: dict) -> str:
    """The address of the site, the 'homepage' of build.yaml."""
    url = conf.get("homepage")
    if not url:
        sys.exit(f"{CONF_FILE}: 'homepage' names no address")
    return url


def repository_url(conf: dict) -> str:
    """The GitHub repository of ZeGrapher, the 'repository' of build.yaml."""
    url = conf.get("repository")
    if not url:
        sys.exit(f"{CONF_FILE}: 'repository' names no address")
    return url


def contact_address(conf: dict) -> str:
    """The email address of the author, out of the 'contact' entry of 'footer: links'."""
    link = (conf.get("footer") or {}).get("links", {}).get("contact")
    url = link_url(link) if link else ""
    if not url.startswith("mailto:"):
        sys.exit(f"{CONF_FILE}: 'footer: links' names no 'contact' that starts "
                 f"with 'mailto:', the entry that gives the email address")
    return url.removeprefix("mailto:")


def qrc_entries() -> list[str]:
    """The files of the site that the app embeds, under ':/website/<lang>/'.

    Every language gets the manual, the strings file and the pictures that the
    manual shows, the English file standing in for one that the language leaves
    out. A picture that only the site shows, such as the one of the landing
    panel, stays out of the app.
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
    """The C++ string literal that holds the value."""
    return '"' + value.replace("\\", "\\\\").replace('"', '\\"') + '"'


def header(conf: dict, version: str) -> str:
    """buildconfig.h, the version of the app and the addresses of build.yaml as
    C++ macros."""
    macros = {"SOFTWARE_VERSION": version,
              "HOMEPAGE_URL": homepage_url(conf),
              "REPOSITORY_URL": repository_url(conf),
              "CONTACT_ADDRESS": contact_address(conf),
              "DONATE_URL": donate_url(conf)}
    lines = ["// website/scripts/content.py writes this file out of the version "
             "of meson.build and website/build-config/build.yaml",
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
