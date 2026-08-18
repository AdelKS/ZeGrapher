#!/usr/bin/env python3
"""
Build the ZeGrapher website: many markdown files -> one HTML page per language.

A page is a single file whose sections are CSS :target tabs. Every markdown file
of a language becomes one <article class="panel">, and the panels are
concatenated into that one index.html. See README.md in the website folder.

Layout
------
The paths below start at the website folder, which holds this script under
scripts/.

  build-config/build.yaml  the icon of each panel, the pictures of the gallery,
                           the picture of the landing panel, and the links of
                           the footer
  build-config/htaccess    the server rules, written to <out>/.htaccess
  templates/               layout.html, intro.html, section.html
  assets/                  css, fonts, icons and images, copied as they are
    icons/*.svg            one standalone SVG per icon, written in where used
  content/<lang>/*.md      the panels of one language
  content/<lang>/footer.md the words of the footer in that language
  content/<lang>/images/   the pictures those panels show
  download/                the bundles of one release, which the tiles link
  ../build-website/        (--out) the site this writes

The panels of a page
--------------------
A panel is one markdown file, named '<number>-<id>.md'. The number is the place
of the panel in the page, and the id is the fragment that opens it, such as
'#gallery'. build.yaml gives the icon of each panel under the name of the file.

The first panel is the landing panel, and it takes templates/intro.html. Every
other panel takes templates/section.html.

The title of a panel is the '# ' heading that its markdown file opens with, in
the words of that language. A file that opens with anything else stops the
build.

The footer
----------
The strip under the page comes from two files. The 'footer: links' of build.yaml
names one entry per key, in the order of the strip, and it holds the address
that each entry opens. content/<lang>/footer.md holds the words of each key, in
the words of that language:

  - donate: Faire un don

The words of an entry are the link. An entry that carries a 'link-name' links
that name in place of the words, which then stand before it, as the credit of
the theme does.

The copyright is the one entry that neither file holds: it stands at the head of
every strip, out of the COPYRIGHT of this script.

Languages
---------
content/ holds one folder per language, and every folder holds the same file
names. Only the text of a file differs. English is the source of the site: a
language that has no file for one panel takes the English file, so a
part-translated language still holds every panel. A key that footer.md leaves
out works this way too.

The pictures work the same way. Each build copies content/en/images/ into
'<out>/<lang>/images/', then copies content/<lang>/images/ over it, so a
translated screenshot wins and every other picture stays English.

A front matter holds the strings that a language translates and no heading can
carry. Only the landing panel has one:

  language     the name of this language, in the words of that language. The
               menu of the header shows it, and a language folder needs it
  page_title   the <title> of the whole page
  tagline      the line under the title of the landing panel

Generated blocks
----------------
A markdown body can hold a placeholder on a line of its own. The build replaces
that line with markup that markdown cannot write:

  {{ gallery }}    the pictures that build.yaml names, in a lightbox grid
  {{ downloads }}  one tile per platform, under the line that names the version
  {{ donation }}   content/<lang>/donation.md, and one big icon under it,
                   centered, that links the donation page. The app embeds that
                   file and shows the same words, so it holds markdown alone

The download tiles
------------------
The tiles come from the files of download/, next to this script. Each script of
deploy/ writes the platform, the architecture and the version into the name of
the bundle it builds. The build reads all three out of those names, copies each
bundle into <out>/download/ as ZeGrapher-<os>-<arch>-<version>, and links the
tile to it. The date under the tiles is the day of the build.

A release also writes the version to <out>/latest_tag and <out>/latest. A
running ZeGrapher reads one of the two to see whether a new release is out: the
tag in the first file, and the same version as a decimal number in the second,
which ZeGrapher 3 reads.

An empty download/, or no download/ at all, stops the build. Pass --no-bundles
to build a page whose tiles link the releases page on GitHub. That is the build
of someone who has no bundle at hand, and it writes no 'latest' file, so it is
not a build to deploy. A folder that holds a part of a release stops the build,
and so does a bundle of an alpha, a beta or a release candidate: the site
carries releases alone.

Usage:    ./scripts/build-website.py [--out build-website] [--no-bundles]
Requires: pandoc on PATH, PyYAML, Python 3.12+.
"""

import argparse
import hashlib
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from datetime import date
from pathlib import Path

import yaml

SITE = Path(__file__).resolve().parents[1]
ROOT = SITE.parent

TEMPLATES = SITE / "templates"
ASSETS = SITE / "assets"
ICONS = ASSETS / "icons"
CONTENT = SITE / "content"
CONF_FILE = SITE / "build-config" / "build.yaml"
HTACCESS_FILE = SITE / "build-config" / "htaccess"
DOWNLOADS = SITE / "download"

# The language that every other one falls back to
SOURCE = "en"

# The line that opens the footer. It holds no word to translate, so it sits here
# and not in build.yaml, and the year of the build closes its range. The two
# '&nbsp;' hold the gap after the year: a plain space collapses into one.
COPYRIGHT = "© 2011-{year} &nbsp;&nbsp; Adel Kara Slimane"

# The folder of pictures inside a language folder, and the path a panel writes
PICTURES = "images"

# The words of the footer inside a language folder. It is not a panel: the
# footer sits under the page, and every panel shares it.
FOOTER = "footer.md"

# The words that ask for a donation, inside a language folder. It is not a
# panel either: the '{{ donation }}' block of a panel puts it in the page, and
# the app embeds the same file.
DONATION = "donation.md"

# Every page is written to '<out>/<lang>/index.html', so the root of the site is
# the folder above the page, and every path a page holds starts there.
SITE_ROOT = ".."

PLACEHOLDER = re.compile(r"{{\s*(\w+)\s*}}")
SRC_ATTR = re.compile(r'(\bsrc=")([^"]+)(")')
SCHEME = re.compile(r"^[a-zA-Z][\w+.-]*:|^//|^#")
FENCED_CODE = re.compile(r"^(```|~~~).*?^\1", re.M | re.S)
HTML_BLOCK = re.compile(r"^<[a-zA-Z!/]", re.M)
HREF_ATTR = re.compile(r'(\bhref=")([^"]+)(")')
SVG_OPEN = re.compile(r"<svg\b[^>]*>", re.S)
VIEWBOX = re.compile(r'viewBox="([^"]+)"')
XML_COMMENT = re.compile(r"<!--.*?-->", re.S)
DOC_LINK = re.compile(r"^(?:.*/)?([\w.-]+)\.md(?:#(.+))?$")
BLOCK = re.compile(r"<p>\s*{{\s*(\w+)\s*}}\s*</p>")
BLOCKS = ("donation", "downloads", "gallery")

REPO = "https://github.com/AdelKS/ZeGrapher"

# The bundles that the tiles link, as (platform, architecture). Every tile shows
# one platform, so a build carries the four of them or none at all.
BUNDLES = (("Linux", "x86_64"), ("Windows", "x86_64"),
           ("macOS", "arm64"), ("macOS", "x86_64"))

# msys2 names an environment after its toolchain, and windows-bundle-7z.sh
# writes that name where the other scripts write the architecture.
TOOLCHAINS = {"MINGW64": "x86_64", "UCRT64": "x86_64", "CLANG64": "x86_64",
              "CLANGARM64": "arm64"}

# 'ZeGrapher-macOS-arm64-v4.0.1.dmg' -> macOS, arm64, 4.0.1. The version of a
# bundle is the name of a tag, and the site drops the 'v' that opens it. The
# match runs on the stem, which drops the last suffix alone: the version of a
# '.tar.gz' bundle thus ends in '.tar', and no name of that shape matches.
BUNDLE_NAME = re.compile(r"ZeGrapher-(\w+)-(\w+)-v?(\d+(?:\.\d+)*(?:_\w+)?)")

# '4.0.0_beta1' and '4.0.0_rc2' are pre-releases, which the site never carries
PRE_RELEASE = re.compile(r"_(alpha|beta|rc)", re.I)

# '4-documentation.md' -> the place of the panel in the page, and its fragment
PANEL_NAME = re.compile(r"(\d+)-([a-z0-9-]+)\.md")

# '- donate: Faire un don' -> the entry of the footer, and the words it shows
FOOTER_WORDS = re.compile(r"^\s*-\s*([\w-]+)\s*:\s*(\S.*?)\s*$", re.M)


def read_conf(path: Path) -> dict:
    """The configuration of the build, checked field by field.

    Every check on the file happens here, so a mistake in it stops the build
    before pandoc runs.
    """
    if not path.is_file():
        sys.exit(f"{path} does not exist, and it holds the configuration of the build")

    conf = yaml.safe_load(path.read_text(encoding="utf-8")) or {}
    for field in ("icon-map", "gallery", "hero-image", "footer"):
        if not conf.get(field):
            sys.exit(f"{path}: '{field}' holds nothing")

    if not conf["footer"].get("links"):
        sys.exit(f"{path}: 'footer: links' holds nothing, and it names every "
                 f"entry of the footer")
    return conf


def read_panels(icon_map: dict) -> list:
    """Every panel of the page, in the order of the numbers of the file names."""
    folder = CONTENT / SOURCE
    if not folder.is_dir():
        sys.exit(f"{folder} does not exist, and it holds the source of every panel")

    panels = []
    for md in sorted(folder.glob("*.md")):
        if md.name in (FOOTER, DONATION):
            continue
        name = PANEL_NAME.fullmatch(md.name)
        if name is None:
            sys.exit(f"{md}: a panel is named '<number>-<id>.md', such as "
                     f"'1-home.md'. The number is its place in the page, and "
                     f"the id is the fragment that opens it")
        if md.stem not in icon_map:
            sys.exit(f"{CONF_FILE}: the 'icon-map' names no icon for '{md.stem}'")
        panels.append({"index": int(name.group(1)), "id": name.group(2),
                       "file": md.name, "icon": icon_map[md.stem]})

    if not panels:
        sys.exit(f"{folder} holds no markdown file, so the page has no panel")

    unknown = sorted(set(icon_map) - {Path(panel["file"]).stem for panel in panels})
    if unknown:
        sys.exit(f"{CONF_FILE}: the 'icon-map' names {', '.join(unknown)}, and "
                 f"{folder} holds no such file")

    numbers = [panel["index"] for panel in panels]
    if len(set(numbers)) != len(numbers):
        sys.exit(f"{folder}: two panels carry the same number. The number of a "
                 f"panel is its place in the page, so each one is used one time")

    return sorted(panels, key=lambda panel: panel["index"])


def read_languages(landing: dict) -> dict:
    """The languages of the site and the name of each, English first.

    A folder of content/ counts as a language when it holds a markdown file, so
    a new language is a folder and the files in it. The 'language' of the front
    matter of the landing panel gives the name that the menu of the header
    shows, in the words of that language.
    """
    folders = sorted(folder.name for folder in CONTENT.iterdir()
                     if folder.is_dir() and any(folder.glob("*.md")))
    names = {}
    for lang in [SOURCE] + [name for name in folders if name != SOURCE]:
        md = CONTENT / lang / landing["file"]
        front = split_front_matter(md.read_text(encoding="utf-8"))[0] if md.is_file() else {}
        if not front.get("language"):
            sys.exit(f"{md}: the front matter holds no 'language'. It names this "
                     f"language in the words of that language, such as "
                     f"'language: Français'")
        names[lang] = front["language"]
    return names


def panel_source(panel: dict, lang: str) -> Path:
    """The markdown file of one panel, for one language, English when it is missing."""
    translated = CONTENT / lang / panel["file"]
    return translated if translated.is_file() else CONTENT / SOURCE / panel["file"]


def picture_path(name: str, lang: str) -> str:
    """The path of one picture, as a panel writes it, checked against content/.

    copy_pictures() writes both languages into the same folder of the output, so
    one path covers the picture of the language and the English one behind it.
    """
    folders = dict.fromkeys((lang, SOURCE))
    for folder in folders:
        if (CONTENT / folder / PICTURES / name).is_file():
            return f"{PICTURES}/{name}"
    holders = " nor ".join(f"content/{folder}/{PICTURES}/" for folder in folders)
    sys.exit(f"'{name}' is not in {holders}")


def copy_pictures(lang: str, out_root: Path) -> None:
    """Copy the pictures of one language into '<out>/<lang>/images/'.

    The English pictures go in first, and the pictures of the language write
    over them. A language that translates one screenshot thus keeps the English
    picture of every other panel.
    """
    dest = out_root / lang / PICTURES
    for folder in dict.fromkeys((CONTENT / SOURCE, CONTENT / lang)):
        if (folder / PICTURES).is_dir():
            shutil.copytree(folder / PICTURES, dest, dirs_exist_ok=True)


def localize_pictures(html: str, lang: str, md: Path) -> str:
    """Rewrite each 'images/<name>' src to the pictures of one language.

    A panel writes 'images/<name>', the path that the app resolves in the manual
    too. copy_pictures() puts that picture under '<out>/<lang>/images/', and a
    page sits one folder under the root of the site. A remote or anchor
    reference stays as it is.
    """
    def replace(match: "re.Match") -> str:
        prefix, url, suffix = match.groups()
        if SCHEME.match(url):
            return match.group(0)
        if not (CONTENT / lang / url).is_file() and not (CONTENT / SOURCE / url).is_file():
            sys.exit(f"{md}: '{url}' is neither in content/{lang}/ nor in "
                     f"content/{SOURCE}/")
        return f"{prefix}{SITE_ROOT}/{lang}/{url}{suffix}"

    return SRC_ATTR.sub(replace, html)


def fill(template: str, values: dict) -> str:
    """Replace every {{ name }} with values[name] (missing -> empty string)."""
    return PLACEHOLDER.sub(lambda m: str(values.get(m.group(1), "")), template)


def split_front_matter(text: str):
    """Return (metadata dict, body) for a '--- ... ---' YAML-ish front matter."""
    lines = text.splitlines()
    if not lines or lines[0].strip() != "---":
        return {}, text
    meta, i = {}, 1
    while i < len(lines) and lines[i].strip() != "---":
        if ":" in lines[i]:
            key, value = lines[i].split(":", 1)
            meta[key.strip()] = value.strip().strip('"').strip("'")
        i += 1
    body = "\n".join(lines[i + 1:]) if i < len(lines) else ""
    return meta, body


def markdown_to_html(body: str, id_prefix: str) -> str:
    """Render a markdown fragment with pandoc (raw HTML blocks pass through).

    Headings get '<panel id>-' prefixed identifiers, and pandoc rewrites the
    same-page '#slug' links that point at them to match. A panel id and a heading
    anchor share the one fragment namespace of the single-page site. The prefix
    is what keeps the two apart.

    gfm_auto_identifiers makes pandoc write the anchor of a heading the way
    GitHub writes it, which is also what the table of contents of an editor
    generates. Without it pandoc drops the number a heading starts with, and
    every '#3-6-plot-style' of a generated table of contents misses.

    Every heading also drops one level, so the '# ' title of a file becomes the
    <h2> of its panel and a '## ' section becomes an <h3>. The <h1> of the page
    is the name of the site, which the landing panel holds.
    """
    if not body.strip():
        return ""
    result = subprocess.run(
        # -implicit_figures: a lone image stays an <img>, not a <figure>+<figcaption>.
        ["pandoc", "-f", "markdown-implicit_figures+gfm_auto_identifiers",
         "-t", "html5", "--wrap=none",
         "--shift-heading-level-by=1", f"--id-prefix={id_prefix}-"],
        input=body, capture_output=True, text=True,
    )
    if result.returncode != 0:
        sys.exit(f"pandoc failed:\n{result.stderr}")
    return result.stdout.rstrip("\n")


def resolve_doc_links(html: str, md: Path, panel_ids: dict) -> str:
    """Turn 'other.md' / 'other.md#slug' links into this page's '#id' fragments.

    A doc links to another doc by file name, which is the form the app resolves
    too. Here every doc is a panel of the same page, so the link becomes an
    anchor. The heading slug gets the prefix of the target panel, the same one
    pandoc put on the id of that heading.
    """
    def replace(match: "re.Match") -> str:
        prefix, url, suffix = match.groups()
        link = DOC_LINK.match(url)
        if not link:
            return match.group(0)
        name, slug = link.groups()
        if f"{name}.md" not in panel_ids:
            sys.exit(f"{md}: link to '{url}', but no panel comes from {name}.md")
        target = panel_ids[f"{name}.md"]
        return f"{prefix}#{target}-{slug}{suffix}" if slug else f"{prefix}#{target}{suffix}"

    return HREF_ATTR.sub(replace, html)


def check_portable(md: Path, text: str) -> None:
    """Reject what only pandoc can render, for files shared with the app.

    The app renders the manual with the importer of Qt, which accepts a small
    part of the HTML that pandoc accepts. A raw HTML block that looks right on
    the site thus degrades in the doc viewer, and nothing reports it. The blocks
    that this build generates are the way to markup that markdown cannot write.
    """
    # This scans the whole file, and blanks the code out in place of dropping it.
    # The line number reported is then the one an editor shows.
    without_code = FENCED_CODE.sub(lambda m: "\n" * m.group(0).count("\n"), text)
    match = HTML_BLOCK.search(without_code)
    if match:
        line = without_code.count("\n", 0, match.start()) + 1
        sys.exit(f"{md}:{line}: raw HTML is not allowed in shared docs, "
                 f"only markdown the app can render too")


def read_icon(name: str) -> str:
    """The <svg> for one icon, ready to inline at its use site.

    Each icon is one standalone file, a Font Awesome glyph. To add an icon, put
    a file in 'assets/icons/' and name it in the 'icon-map' of build.yaml. The
    README next to this script tells where the glyphs come from.
    """
    icon = ICONS / f"{name}.svg"
    if not icon.is_file():
        sys.exit(f"icon '{name}' is used but {icon} does not exist")
    svg = XML_COMMENT.sub("", icon.read_text(encoding="utf-8"))
    open_tag = SVG_OPEN.search(svg)
    box = VIEWBOX.search(open_tag.group(0)) if open_tag else None
    if box is None:
        sys.exit(f"{icon}: not an <svg> with a viewBox")
    inner = svg[open_tag.end():svg.rindex("</svg>")].strip()
    return f'<svg viewBox="{box.group(1)}">{inner}</svg>'


def gallery_html(names: list, lang: str, panel_id: str) -> str:
    """The pictures that build.yaml names, in a lightbox grid.

    Markdown cannot write this: each picture needs a checkbox and two labels, one
    for the thumbnail and one for the overlay. A click on the thumbnail ticks the
    box, '.lightbox-toggle:checked ~ .lightbox' opens the overlay, and a click on
    the overlay unticks the box. That is the whole gallery, without JavaScript.
    """
    figures = []
    for name in names:
        src = picture_path(name, lang)
        shot = f"{panel_id}-{Path(name).stem}"
        figures.append(
            f'<figure>\n'
            f'<input type="checkbox" id="{shot}" class="lightbox-toggle" />\n'
            f'<label for="{shot}" class="thumb"><img src="{src}" alt="" /></label>\n'
            f'<label for="{shot}" class="lightbox"><img src="{src}" alt="" /></label>\n'
            f'</figure>')
    return '<div class="gallery">\n' + "\n".join(figures) + "\n</div>"


def read_bundles(folder: Path, no_bundles: bool) -> tuple[str, dict] | None:
    """The version of a release and the file of each bundle, or None.

    The name of a bundle carries its platform, its architecture and its
    version, which is what every script of deploy/ writes. The build reads the
    three out of the names, so the version on the page is the version of the
    files that the page links.

    A folder that is not there, or that holds no file, stops the build.
    'no_bundles' turns those two into None, and the tiles then link GitHub.
    Anything else is a release, and every fault in it stops the build.
    """
    empty = "Fill it with the four bundles of one release, or pass --no-bundles"
    if not folder.is_dir():
        if no_bundles:
            return None
        sys.exit(f"{folder} does not exist, and the tiles link the bundles in "
                 f"it. {empty}")
    files = sorted(path for path in folder.iterdir() if path.is_file())
    if not files:
        if no_bundles:
            return None
        sys.exit(f"{folder} holds no bundle, and the tiles link the bundles in "
                 f"it. {empty}")

    bundles, versions = {}, {}
    for path in files:
        parts = BUNDLE_NAME.fullmatch(path.stem)
        if parts is None:
            sys.exit(f"{path}: a bundle is named "
                     f"'ZeGrapher-<platform>-<architecture>-<version>' and an "
                     f"extension, such as ZeGrapher-Linux-x86_64-v4.0.1.AppImage")
        platform, arch, version = parts.groups()
        arch = TOOLCHAINS.get(arch, arch)
        if (platform, arch) not in BUNDLES:
            shown = ", ".join(f"{name} {machine}" for name, machine in BUNDLES)
            sys.exit(f"{path}: no tile shows {platform} {arch}. "
                     f"The tiles show {shown}")
        if (platform, arch) in bundles:
            sys.exit(f"{path} and {bundles[platform, arch]} are both the "
                     f"{platform} {arch} bundle. Keep one of the two")
        bundles[platform, arch] = path
        versions.setdefault(version, path)

    if len(versions) > 1:
        held = "\n".join(f"  {name}: {path.name}" for name, path in versions.items())
        sys.exit(f"{folder} holds more than one version, and the site shows "
                 f"one release:\n{held}")
    missing = [f"{name} {machine}" for name, machine in BUNDLES
               if (name, machine) not in bundles]
    if missing:
        sys.exit(f"{folder} holds no bundle for {', '.join(missing)}. Every tile "
                 f"shows one platform, so the site needs the four bundles")

    version = next(iter(versions))
    if PRE_RELEASE.search(version):
        sys.exit(f"{folder} holds {version}, which is a pre-release. The site "
                 f"names one version to every user of the app, in 'latest' and "
                 f"'latest_tag', so the build stops here. Pass --no-bundles to "
                 f"build a page whose tiles link GitHub")

    return version, bundles


def copy_bundles(bundles: dict, version: str, out_root: Path) -> dict:
    """Copy each bundle into 'download/', and return the paths on the site.

    A bundle takes the one name that the site uses:
    'ZeGrapher-<os>-<arch>-<version>' and the extension of the file that came
    in. The deploy replaces the site with this build, so 'download/' holds the
    bundles of this release alone. GitHub keeps every older release.
    """
    links = {}
    for (os_name, arch), source in bundles.items():
        # Only the last suffix: 'ZeGrapher-Linux-x86_64-v4.0.1.AppImage' splits
        # into '.2', '.1' and '.AppImage'.
        name = f"ZeGrapher-{os_name}-{arch}-{version}{source.suffix}"
        dest = out_root / "download" / name
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, dest)
        links[os_name, arch] = f"download/{name}"
    return links


def download_tile(icon: str, name: str, href: str, subs: list) -> str:
    """One tile: a big icon that links a file, its name, and the smaller links."""
    parts = ['<div class="download">',
             f'<a class="icon" href="{href}">{read_icon(icon)}</a>',
             f'<span class="name">{name}</span>']
    parts += [f'<a class="sub" href="{href}">{text}</a>' for text, href in subs]
    parts.append("</div>")
    return "\n".join(parts)


def downloads_html(release: dict | None) -> str:
    """The download tiles, under the line that names the version.

    Without a release, every tile links to the releases page on GitHub and no
    version line is written. That is the build of someone who has no bundle at
    hand.
    """
    if release is None:
        line, links = "", dict.fromkeys(BUNDLES, f"{REPO}/releases/latest")
    else:
        line = (f'<p class="release">Version {release["version"]}, released on '
                f'{release["date"]}</p>\n')
        links = {key: f"{SITE_ROOT}/{path}" for key, path in release["links"].items()}
    tiles = [
        download_tile("linux", "Linux", links["Linux", "x86_64"], []),
        download_tile("windows", "Windows", links["Windows", "x86_64"], []),
        download_tile("apple", "macOS", links["macOS", "arm64"],
                      [("Intel", links["macOS", "x86_64"])]),
        download_tile("github", "Source", REPO,
                      [("Releases", f"{REPO}/releases"),
                       ("Issues", f"{REPO}/issues")]),
    ]
    return line + '<div class="downloads">\n' + "\n".join(tiles) + "\n</div>"


def donation_html(footer: dict, lang: str, panel_id: str) -> str:
    """The words of content/<lang>/donation.md, and one big icon under them.

    The icon is centered and links the donation page. That page is the 'donate'
    entry of 'footer: links' in build.yaml, so the footer and this icon never
    point at two different pages.
    """
    link = footer["links"].get("donate")
    if link is None:
        sys.exit(f"{CONF_FILE}: 'footer: links' names no 'donate', and the "
                 f"'{{{{ donation }}}}' block links that page")
    url = link["url"] if isinstance(link, dict) else link

    md = CONTENT / lang / DONATION
    if not md.is_file():
        md = CONTENT / SOURCE / DONATION
    if not md.is_file():
        sys.exit(f"{md} does not exist, and it holds the words that ask for a "
                 f"donation")

    text = md.read_text(encoding="utf-8")
    check_portable(md, text)

    return (markdown_to_html(text, panel_id)
            + f'\n<div class="donate">'
            + f'<a class="icon" href="{url}">{read_icon("hand-holding-heart")}</a>'
            + f'</div>')


def decimal_version(version: str) -> str:
    """The version as one decimal number, which ZeGrapher 3 reads as a double.

    That release compares the file with the number it was built with, and
    'v3.1.1' was built as 3.11. The first dot is thus the decimal point, and the
    dots after it go: '4.0.1' is 4.01, which is more than every 3.x number. A
    version that carries a word, such as '4.0.0_beta1', drops the word: that
    build reads as the release it comes before.
    """
    major, _, rest = re.match(r"\d+(?:\.\d+)*", version).group(0).partition(".")
    return f"{major}.{rest.replace('.', '')}" if rest else f"{major}.0"


def write_version_files(release: dict | None, out_root: Path) -> None:
    """Write the two files that a running ZeGrapher reads to find a new release.

    Both sit at the root of the site, where each version of the app asks for
    them. 'latest_tag' holds the tag of the release, which the app compares with
    the tag it was built from. 'latest' holds the same version as a decimal
    number, which ZeGrapher 3 reads.

    A build that holds no release writes neither file, so a build without a
    bundle cannot publish a version over the one that the site holds.
    """
    if release is None:
        return
    (out_root / "latest_tag").write_text(f"v{release['version']}\n", encoding="utf-8")
    (out_root / "latest").write_text(f"{decimal_version(release['version'])}\n",
                                     encoding="utf-8")


def release_info(out_root: Path, no_bundles: bool) -> dict | None:
    """What the download section shows, or None under --no-bundles.

    A release goes up on the day it is built, so the date under the tiles is the
    day of the build.
    """
    found = read_bundles(DOWNLOADS, no_bundles)
    if found is None:
        return None
    version, bundles = found
    day = date.today()
    return {
        "version": version,
        "date": f"{day.day} {day:%B} {day.year}",
        "links": copy_bundles(bundles, version, out_root),
    }


def insert_blocks(html: str, blocks: dict, md: Path) -> str:
    """Replace each '{{ name }}' paragraph with the markup that the build wrote.

    Markdown cannot write a gallery, so the build writes it. The placeholder
    marks where that markup lands, so a panel keeps the order its author gave it.
    """
    def replace(match: "re.Match") -> str:
        name = match.group(1)
        if name not in blocks:
            sys.exit(f"{md}: '{name}' is not a block of this build. "
                     f"The blocks are: {', '.join(sorted(BLOCKS))}")
        return blocks[name]

    return BLOCK.sub(replace, html)


def language_menu(names: dict, current: str) -> str:
    """The header menu that opens the same page in another language.

    The language of a page is the folder it sits in, so a link to another
    language is a link to the folder of that language. <details> opens the list
    without JavaScript, and holds it open until a click closes it again.

    One language gives no menu: there is nothing to switch to.
    """
    if len(names) < 2:
        return ""
    items = [f'<li><a href="{SITE_ROOT}/{lang}/" lang="{lang}" '
             f'hreflang="{lang}">{name}</a></li>'
             for lang, name in names.items() if lang != current]
    # 'lang' marks the text of each name as the language it names, so a screen
    # reader says it in that language.
    return ('<details>\n\t\t\t\t\t\t'
            f'<summary lang="{current}">{names[current]}</summary>\n\t\t\t\t\t\t'
            '<ul>\n\t\t\t\t\t\t\t'
            + "\n\t\t\t\t\t\t\t".join(items)
            + '\n\t\t\t\t\t\t</ul>\n\t\t\t\t\t</details>')


def footer_words(lang: str, links: dict) -> dict:
    """The words of the footer of one language, out of content/<lang>/footer.md.

    Each item of that file names an entry of 'footer: links' of build.yaml, and
    the words that entry shows: '- donate: Faire un don'. Every other line of
    the file is a note to the translator, and no page holds it. That note stays
    English in every language.
    """
    md = CONTENT / lang / FOOTER
    if not md.is_file():
        return {}
    words = dict(FOOTER_WORDS.findall(md.read_text(encoding="utf-8")))
    unknown = sorted(set(words) - set(links))
    if unknown:
        sys.exit(f"{md}: it holds the words of {', '.join(unknown)}, and the "
                 f"'footer: links' of {CONF_FILE} names no such entry")
    return words


def footer_html(footer: dict, lang: str) -> str:
    """The strip under the page, out of the 'footer' of build.yaml.

    The copyright opens the strip, and 'links' names every entry after it, in
    the order of the file. The words of an entry come from the footer.md of this
    language, and from the English one for a key that language leaves out.

    The words of an entry are the link. An entry that carries a 'link-name'
    links that name in place of the words, which then stand before it: a name
    such as 'Astral by HTML5 UP' is one that no language translates.
    """
    links = footer["links"]
    words = footer_words(SOURCE, links)
    missing = sorted(set(links) - set(words))
    if missing:
        sys.exit(f"{CONTENT / SOURCE / FOOTER}: it holds the words of no "
                 f"{', '.join(missing)}, and every language falls back to it")
    words.update(footer_words(lang, links))

    entries = [COPYRIGHT.format(year=date.today().year)]
    for key, link in links.items():
        url = link["url"] if isinstance(link, dict) else link
        name = link.get("link-name") if isinstance(link, dict) else None
        entries.append(f"{words[key]} [{name}]({url})" if name
                       else f"[{words[key]}]({url})")
    html = markdown_to_html("\n".join(f"- {entry}" for entry in entries), "footer")
    return f'<div id="footer">\n\t\t\t{html}\n\t\t</div>'


def htaccess(template: str, languages: list) -> str:
    """The server rules, with the languages that this build holds.

    Apache reads one alternation of language names, so a language that the site
    does not hold gets no rule at all. A browser that asks for such a language
    lands on the English page, in place of a folder that is not there.
    """
    return fill(template, {"languages": "|".join(languages)})


def active_tab_style(panels: list) -> str:
    """A <style> that brightens the nav icon of the open panel and shows its caret.

    No CSS selector matches "the link whose href is the current #fragment", so
    this writes one selector per panel id. The icon of the landing panel is also
    active by default, when no panel is targeted.

    A panel counts as open when the fragment is its own id or one of its heading
    anchors, hence the ':has(#id:target, #id :target)' pair in every selector.
    """
    home = panels[0]["id"]
    nav_sel = [f'#wrapper:not(:has(#main :target)) #nav a[href="#{home}"]']
    nav_sel += [f'#wrapper:has(#{m["id"]}:target, #{m["id"]} :target) '
                f'#nav a[href="#{m["id"]}"]' for m in panels]
    # Show a panel's prev/next arrows only while that panel is the active one.
    arrow_sel = [f'body:not(:has(#main :target)) .tab-arrow[data-panel="{home}"]']
    arrow_sel += [f'body:has(#{m["id"]}:target, #{m["id"]} :target) '
                  f'.tab-arrow[data-panel="{m["id"]}"]' for m in panels]
    icons = ",\n".join(nav_sel)
    carets = ",\n".join(s + ":after" for s in nav_sel)
    arrows = ",\n".join(arrow_sel)
    return ("<style>\n"
            f"{icons} {{ opacity: 1; }}\n"
            f"{carets} {{ transform: scaleY(1); }}\n"
            f"{arrows} {{ display: flex; }}\n"
            "</style>")


def title_of(md: Path, body: str):
    """The title of a panel, and the body that the panel shows.

    A file opens with the one '# ' heading that names it. That heading is the
    title, in the words of its own language, and the panel shows it as well.
    markdown_to_html() is what drops it to the <h2> of the panel.
    """
    lines = body.lstrip("\n").split("\n")
    heading = re.match(r"^#\s+(.+?)\s*$", lines[0]) if lines else None
    if heading is None:
        sys.exit(f"{md}: a panel must open with a '# Title' heading")

    return heading.group(1), "\n".join(lines)


@dataclass
class Site:
    """What every page of the build shares."""
    conf: dict
    templates: dict
    panels: list
    languages: dict
    release: dict | None
    out_root: Path
    css_version: str


def build_language(lang: str, site: Site) -> Path:
    panels = []
    for i, panel in enumerate(site.panels):
        md = panel_source(panel, lang)
        text = md.read_text(encoding="utf-8")
        check_portable(md, text)

        meta = dict(panel)
        # only the strings that a language translates sit in a front matter
        front, body = split_front_matter(text)
        meta.update(front)
        # The landing panel opens the page, and its template shows one picture.
        meta["template"] = "intro" if i == 0 else "section"
        if i == 0:
            meta["hero_image"] = picture_path(site.conf["hero-image"], lang)

        meta["title"], body = title_of(md, body)
        html = markdown_to_html(body, meta["id"])
        names, blocks = BLOCK.findall(html), {}
        if "gallery" in names:
            blocks["gallery"] = gallery_html(site.conf["gallery"], lang, meta["id"])
        if "downloads" in names:
            blocks["downloads"] = downloads_html(site.release)
        if "donation" in names:
            blocks["donation"] = donation_html(site.conf["footer"], lang, meta["id"])
        meta["html"] = insert_blocks(html, blocks, md)
        meta["source"] = md
        panels.append(meta)

    # Second pass: a doc-to-doc link needs the id of a panel, and the loop above
    # can render that link before it reads the panel.
    panel_ids = {m["file"]: m["id"] for m in panels}
    for m in panels:
        m["html"] = resolve_doc_links(m["html"], m["source"], panel_ids)

    nav, articles, arrow_links, page_title = [], [], [], "ZeGrapher"
    for i, m in enumerate(panels):
        # The landing panel links its picture to the panel that comes after it.
        following = panels[i + 1]["id"] if i + 1 < len(panels) else ""
        # Localize the assembled panel, so that a picture the template adds, such
        # as the intro hero, is rewritten like a picture of the markdown.
        article = fill(site.templates[m["template"]],
                       {**m, "next_id": following, "content": m["html"]})
        articles.append(localize_pictures(article, lang, m["source"]))
        nav.append(f'<a href="#{m["id"]}" class="icon">'
                   f'{read_icon(m["icon"])}'
                   f'<span>{m["title"]}</span></a>')
        page_title = m.get("page_title", page_title)
        # The page-level prev/next arrows of this panel. The generated rules show
        # them while it is the open one. The first panel has no '<', the last no '>'.
        if i > 0:
            arrow_links.append(f'<a class="tab-arrow prev" data-panel="{m["id"]}" '
                               f'href="#{panels[i-1]["id"]}" aria-label="Previous">'
                               f'{read_icon("chevron-left")}</a>')
        if i < len(panels) - 1:
            arrow_links.append(f'<a class="tab-arrow next" data-panel="{m["id"]}" '
                               f'href="#{panels[i+1]["id"]}" aria-label="Next">'
                               f'{read_icon("chevron-right")}</a>')

    page = fill(site.templates["layout"], {
        "lang": lang,
        "title": page_title,
        "css_version": site.css_version,
        "nav": "\n\t\t\t\t\t".join(nav),
        "languages": language_menu(site.languages, lang),
        "moon_icon": read_icon("moon"),
        "sun_icon": read_icon("sun"),
        "nav_active": active_tab_style(panels),
        "arrows": "\n\t\t".join(arrow_links),
        "footer": footer_html(site.conf["footer"], lang),
        "content": "\n\n".join(articles),
    })

    out = site.out_root / lang / "index.html"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(page, encoding="utf-8")
    return out


def main() -> None:
    ap = argparse.ArgumentParser(description="Generate the ZeGrapher website.")
    ap.add_argument("--out", type=Path, default=ROOT / "build-website",
                    help="where the site is written (default: build-website/ "
                         "at the root of the repository, which is not committed)")
    ap.add_argument("--no-bundles", action="store_true",
                    help="build a page whose tiles link the releases page on "
                         "GitHub. The build then writes no 'latest' file, so it "
                         "is a build to read and not one to deploy")
    args = ap.parse_args()

    if not shutil.which("pandoc"):
        sys.exit("pandoc is not on PATH. Install pandoc, then run this again.")

    conf = read_conf(CONF_FILE)
    panels = read_panels(conf["icon-map"])
    languages = read_languages(panels[0])

    templates = {
        name: (TEMPLATES / f"{name}.html").read_text(encoding="utf-8")
        for name in ("layout", "section", "intro")
    }
    # Added to the <link> of main.css. It changes with the CSS, so a browser
    # cannot serve a stale stylesheet after a rebuild.
    css_version = hashlib.md5(
        (ASSETS / "css/main.css").read_bytes()).hexdigest()[:8]

    if not any((ASSETS / "fonts").glob("*.woff2")):
        print(f"warning: {ASSETS / 'fonts'} holds no font. Run "
              f"'fetch_fonts.py', next to this script, to download the web font", file=sys.stderr)

    args.out.mkdir(parents=True, exist_ok=True)
    shutil.copytree(ASSETS, args.out / "assets", dirs_exist_ok=True)
    (args.out / ".htaccess").write_text(
        htaccess(HTACCESS_FILE.read_text(encoding="utf-8"), list(languages)),
        encoding="utf-8")

    release = release_info(args.out, args.no_bundles)
    write_version_files(release, args.out)

    site = Site(conf=conf,
                release=release,
                templates=templates,
                panels=panels,
                languages=languages,
                out_root=args.out,
                css_version=css_version)
    for lang in languages:
        copy_pictures(lang, args.out)
        print(f"built {build_language(lang, site)}")


if __name__ == "__main__":
    main()
