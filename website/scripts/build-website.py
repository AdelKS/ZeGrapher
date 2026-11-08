#!/usr/bin/env python3
"""
Build the ZeGrapher website: markdown files -> one HTML page per language.

Each page is a single file whose sections are CSS :target tabs. Each markdown
file becomes one <article class="panel">, and the panels of a language go into
one index.html. See the README.md in the website folder.

Layout
------
Paths start at the website folder, which holds this script under scripts/.

  build-config/build.yaml  the GitHub repository, panel icons, gallery
                           pictures, landing panel picture and footer links
  build-config/htaccess    the server rules, written to <out>/.htaccess
  templates/               layout.html, intro.html, section.html
  assets/                  css, fonts, icons and images, copied as is
  ../icons/ZeGrapher.svg   the app icon, copied to <out>/assets/images/
    icons/*.svg            one standalone SVG per icon, inlined where used
  content/<lang>/*.md      a language's panels
  content/<lang>/strings.yaml  a language's short strings: its name, the page
                           title and tagline, footer text, button labels, the
                           line above the download tiles, source tile text,
                           gallery captions and the donation appeal
  content/<lang>/images/   the panel pictures
  ../deploy/               the bundles that the deploy/ scripts write, which
                           the download tiles link to
  ../build-website/        (--out) the output

Panels
------
Each panel is one markdown file, named '<number>-<id>.md'. The number sets the
panel order, and the id is the URL fragment that opens it, such as '#gallery'.
The build.yaml 'icon-map' gives each panel's icon, keyed by file name.

The first panel is the landing panel, and uses templates/intro.html. The other
panels use templates/section.html.

A panel's title is the '# ' heading on the first line of its file, translated
like the rest. If a file starts with anything else, the build stops.

Footer
------
The footer comes from two files. The build.yaml 'footer: links' lists the
entries in order, with each entry's URL. The 'footer:' mapping in
content/<lang>/strings.yaml gives each entry's text in that language:

  footer:
    donate: Faire un don

An entry's text is the link text. If the entry also has a 'link-name', that
name is the link, and the text goes before it as plain text. The theme credit
works this way: 'Based on Astral by HTML5 UP'.

The copyright comes first, from COPYRIGHT in this script.

Languages
---------
content/ holds one folder per language, with the same file names in each.
English is the source: a language without a panel file uses the English one,
so a partial translation still has every panel. A key missing from a strings
file also falls back to English.

Pictures work the same way. The build copies content/en/images/ to
'<out>/<lang>/images/', then copies content/<lang>/images/ over it. A
translated screenshot replaces the English one, and the rest stay English.

Three strings in content/<lang>/strings.yaml are for the page, not a panel:

  language     the language name, in that language, shown in the language
               menu at the top of each page. It has no English fallback, so
               every language folder must set it
  page_title   the page <title>
  tagline      the line under the landing panel title, also used as the page
               description that search engines show

Generated blocks
----------------
A markdown body can hold a placeholder on its own line. The build replaces that
line with markup that markdown can't write:

  {{ gallery }}    the build.yaml pictures in a lightbox grid. The 'gallery'
                   mapping in content/<lang>/strings.yaml gives the captions,
                   used as alt text
  {{ downloads }}  one tile per platform and one for the source code, under
                   the 'release' line from content/<lang>/strings.yaml. The
                   'downloads' mapping in that file holds the source tile text
  {{ donation }}   the 'donation' string from content/<lang>/strings.yaml,
                   with a large centered icon below that links to the donation
                   page. The app shows the same string, so it must be plain
                   markdown

Download tiles
--------------
../appdata/releases.py reads the version from meson.build. The deploy/ scripts
put that version in each bundle name. The build copies the six bundles of that
version from deploy/ to <out>/download/, and links each tile to its bundle. The
date under the tiles is the build date.

The build also writes the version to <out>/latest_tag and <out>/latest, even
with --no-bundles. Running ZeGrapher instances read one of them to check for a
new release. 'latest_tag' holds the tag, and 'latest' holds the version as a
decimal number for ZeGrapher 3.

The build stops if a bundle is missing from deploy/. It builds any version,
including development versions. Pass --no-bundles to link the tiles to the
GitHub releases page instead.

Usage:    ./scripts/build-website.py [--out build-website] [--no-bundles]
Requires: pandoc on PATH, PyYAML, Python 3.10+.
"""

import argparse
import hashlib
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from datetime import date
from html import escape
from pathlib import Path

import yaml

SITE = Path(__file__).resolve().parents[1]
ROOT = SITE.parent

# content.py finds each language's files, and releases.py reads the
# meson.build version
sys.path[:0] = [str(SITE / "scripts"), str(ROOT / "appdata")]

import content  # noqa: E402
import releases  # noqa: E402
from content import CONF_FILE, CONTENT, PICTURES, RIGHT_TO_LEFT, SOURCE, STRINGS  # noqa: E402

TEMPLATES = SITE / "templates"
ASSETS = SITE / "assets"
ICONS = ASSETS / "icons"
HTACCESS_FILE = SITE / "build-config" / "htaccess"
DOWNLOADS = ROOT / "deploy"

# The app icon, which the site uses as its logo and favicon. The build copies
# it into the site, so the repository has a single copy.
APP_ICON = ROOT / "icons" / "ZeGrapher.svg"

# The first footer entry. It has nothing to translate, so it is here instead
# of in build.yaml. The range ends at the build year. The two '&nbsp;' keep the
# gap after the year: HTML collapses plain spaces into one.
COPYRIGHT = "© 2011-{year} &nbsp;&nbsp; Adel Kara Slimane"

# Each page goes to '<out>/<lang>/index.html', so the site root is one folder
# up, and every path in a page starts there.
SITE_ROOT = ".."

PLACEHOLDER = re.compile(r"{{\s*(\w+)\s*}}")
SRC_ATTR = re.compile(r'(\bsrc=")([^"]+)(")')
SCHEME = re.compile(r"^[a-zA-Z][\w+.-]*:|^//|^#")
FENCED_CODE = re.compile(r"^(```|~~~).*?^\1", re.M | re.S)
HTML_BLOCK = re.compile(r"^<[a-zA-Z!/]", re.M)
HREF_ATTR = re.compile(r'(\bhref=")([^"]+)(")')
DOC_LINK = re.compile(r"^(?:.*/)?([\w.-]+)\.md(?:#(.+))?$")
BLOCK = re.compile(r"<p>\s*{{\s*(\w+)\s*}}\s*</p>")
BLOCKS = ("donation", "downloads", "gallery")

# Each tile's bundle, as (platform, architecture) -> extension. The deploy/
# scripts name bundles 'ZeGrapher-<platform>-<architecture>-<version>'
BUNDLES = {
    ("Linux", "x86_64"): ".AppImage",
    ("Linux", "arm64"): ".AppImage",
    ("Windows", "x86_64"): ".7z",
    ("Windows", "arm64"): ".7z",
    ("macOS", "arm64"): ".dmg",
    ("macOS", "x86_64"): ".dmg",
}

# '4-documentation.md' -> the panel number and its URL fragment
PANEL_NAME = re.compile(r"(\d+)-([a-z0-9-]+)\.md")



def read_conf() -> dict:
    """The build configuration, checked field by field.

    All the checks run here, so a mistake in the file stops the build before
    pandoc runs.
    """
    conf = content.read_conf()
    for field in ("icon-map", "gallery", "hero-image", "footer"):
        if not conf.get(field):
            sys.exit(f"{CONF_FILE}: '{field}' is empty")

    if not conf["footer"].get("links"):
        sys.exit(f"{CONF_FILE}: 'footer: links' is empty, and it lists every "
                 f"footer entry")
    return conf


def read_panels(icon_map: dict) -> list:
    """Every panel of the page, ordered by file name number."""
    folder = CONTENT / SOURCE
    if not folder.is_dir():
        sys.exit(f"{folder} is missing: it holds the source of every panel")

    panels = []
    for md in sorted(folder.glob("*.md")):
        name = PANEL_NAME.fullmatch(md.name)
        if name is None:
            sys.exit(f"{md}: a panel file must be named '<number>-<id>.md', "
                     f"such as '1-home.md'. The number sets the panel order, and "
                     f"the id is the URL fragment that opens it")
        if md.stem not in icon_map:
            sys.exit(f"{CONF_FILE}: 'icon-map' has no icon for '{md.stem}'")
        panels.append({"index": int(name.group(1)), "id": name.group(2),
                       "file": md.name, "icon": icon_map[md.stem]})

    if not panels:
        sys.exit(f"{folder} has no markdown file, so the page has no panel")

    unknown = sorted(set(icon_map) - {Path(panel["file"]).stem for panel in panels})
    if unknown:
        sys.exit(f"{CONF_FILE}: 'icon-map' lists {', '.join(unknown)}, which "
                 f"{folder} doesn't have")

    numbers = [panel["index"] for panel in panels]
    if len(set(numbers)) != len(numbers):
        sys.exit(f"{folder}: two panels have the same number. The number sets "
                 f"the panel order, so each number must be unique")

    return sorted(panels, key=lambda panel: panel["index"])


def read_languages() -> dict:
    """The site languages and their names, English first.

    content.languages() finds the languages. The 'language' key in each strings
    file gives the language name, in that language, for the language menu.
    """
    names = {}
    for lang in content.languages():
        name = read_strings(lang).get("language")
        if not name:
            sys.exit(f"{CONTENT / lang / STRINGS}: 'language' is missing. Set it "
                     f"to the language name, in that language, such as "
                     f"'language: Français'")
        names[lang] = name
    return names


def picture_path(name: str, lang: str) -> str:
    """A picture path as a panel writes it, checked against content/.

    copy_pictures() copies the English and translated pictures into the same
    output folder, so one path works for both.
    """
    if name in content.pictures(lang):
        return f"{PICTURES}/{name}"
    holders = " nor ".join(f"content/{folder}/{PICTURES}/"
                           for folder in dict.fromkeys((lang, SOURCE)))
    sys.exit(f"'{name}' is not in {holders}")


def copy_pictures(lang: str, out_root: Path) -> None:
    """Copy a language's pictures to '<out>/<lang>/images/'."""
    dest = out_root / lang / PICTURES
    dest.mkdir(parents=True, exist_ok=True)
    for name, path in content.pictures(lang).items():
        shutil.copy2(path, dest / name)


def localize_pictures(html: str, lang: str, md: Path) -> str:
    """Point each 'images/<name>' src at the language's picture folder.

    Panels write 'images/<name>', a path that the app manual also resolves.
    copy_pictures() puts the picture in '<out>/<lang>/images/', and each page
    sits one folder below the site root. Remote and anchor references are left
    as is.
    """
    def replace(match: "re.Match") -> str:
        prefix, url, suffix = match.groups()
        if SCHEME.match(url):
            return match.group(0)
        if not content.file_of(lang, url).is_file():
            sys.exit(f"{md}: '{url}' is neither in content/{lang}/ nor in "
                     f"content/{SOURCE}/")
        return f"{prefix}{SITE_ROOT}/{lang}/{url}{suffix}"

    return SRC_ATTR.sub(replace, html)


def fill(template: str, values: dict) -> str:
    """Replace every {{ name }} with values[name] (missing -> empty string)."""
    return PLACEHOLDER.sub(lambda m: str(values.get(m.group(1), "")), template)


def markdown_to_html(body: str, id_prefix: str) -> str:
    """Render a markdown fragment with pandoc (raw HTML blocks pass through).

    Heading ids get a '<panel id>-' prefix, and pandoc updates the same-page
    '#slug' links to match. Panel ids and heading anchors share the fragment
    namespace of the single-page site, and the prefix keeps them apart.

    gfm_auto_identifiers makes pandoc write heading anchors the way GitHub
    does, which is also what editors use for a table of contents. Without it,
    pandoc drops a heading's leading number, and links such as
    '#3-6-plot-style' break.

    Each heading also moves down one level: a file's '# ' title becomes the
    panel <h2>, and a '## ' section an <h3>. The page <h1> is the site name, in
    the landing panel.
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
    """Turn 'other.md' and 'other.md#slug' links into '#id' fragments.

    Docs link to each other by file name. On the site every doc is a panel of
    the same page, so the link becomes an anchor. The slug gets the target
    panel prefix, the same one pandoc put on the heading id.
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
    """Reject raw HTML in the files that the app shares with the site.

    The app renders the manual with Qt's markdown importer, which supports only
    a small part of the HTML that pandoc supports. A raw HTML block that looks
    right on the site would break silently in the app. If markdown can't write
    the markup, use a generated block.
    """
    # scan the whole file, and blank out fenced code instead of removing it, so
    # the reported line numbers match the editor's
    without_code = FENCED_CODE.sub(lambda m: "\n" * m.group(0).count("\n"), text)
    match = HTML_BLOCK.search(without_code)
    if match:
        line = without_code.count("\n", 0, match.start()) + 1
        sys.exit(f"{md}:{line}: raw HTML is not allowed in shared docs, "
                 f"only markdown the app can render too")


def read_icon(name: str) -> str:
    """An icon's <svg>, ready to inline.

    Each icon is a standalone Font Awesome glyph file that fetch_icon.py wrote.
    The page inlines the file unchanged, with the Font Awesome credit comment
    that the CC BY 4.0 license requires. To add an icon, put a file in
    'assets/icons/' and pass its name here. The build.yaml 'icon-map' sets the
    panel icons. The README next to this script says where the glyphs come
    from.
    """
    icon = ICONS / f"{name}.svg"
    if not icon.is_file():
        sys.exit(f"icon '{name}' is used, but {icon} is missing")
    return icon.read_text(encoding="utf-8").strip()


def gallery_html(names: list, lang: str, panel_id: str) -> str:
    """The build.yaml gallery pictures, in a lightbox grid.

    Markdown can't write this: each picture needs a checkbox and two labels,
    one for the thumbnail and one for the overlay. A click on the thumbnail
    checks the box, '.lightbox-toggle:checked ~ .lightbox' shows the overlay,
    and a click on the overlay unchecks the box. The gallery doesn't
    need JavaScript.
    """
    figures = []
    for name in names:
        src = picture_path(name, lang)
        alt = escape(translated(lang, "gallery", name)[0])
        shot = f"{panel_id}-{Path(name).stem}"
        figures.append(
            f'<figure>\n'
            f'<input type="checkbox" id="{shot}" class="lightbox-toggle" />\n'
            f'<label for="{shot}" class="thumb"><img src="{src}" alt="{alt}" /></label>\n'
            f'<label for="{shot}" class="lightbox"><img src="{src}" alt="{alt}" /></label>\n'
            f'</figure>')
    return '<div class="gallery">\n' + "\n".join(figures) + "\n</div>"


def read_bundles(folder: Path, version: str) -> dict:
    """Each tile's bundle for that version.

    A missing bundle stops the build.
    """
    bundles = {(os_name, arch): folder / f"ZeGrapher-{os_name}-{arch}-{version}{ext}"
               for (os_name, arch), ext in BUNDLES.items()}
    missing = [path.name for path in bundles.values() if not path.is_file()]
    if missing:
        sys.exit(f"{folder} is missing {', '.join(missing)}. The site needs all "
                 f"six {version} bundles, one per tile")

    return bundles


def copy_bundles(bundles: dict, out_root: Path) -> dict:
    """Copy each bundle to 'download/', and return their site paths.

    The deploy replaces the whole site, so 'download/' only holds this
    release. GitHub keeps the older ones.
    """
    links = {}
    for key, source in bundles.items():
        dest = out_root / "download" / source.name
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, dest)
        links[key] = f"download/{source.name}"
    return links


def download_tile(icon: str, name: str, href: str, subs: list) -> str:
    """One tile: a large icon linking to a file, its name, and smaller links."""
    parts = ['<div class="download">',
             f'<a class="icon" href="{href}">{read_icon(icon)}</a>',
             f'<span class="name" dir="auto">{name}</span>']
    parts += [f'<a class="sub" href="{href}">{text}</a>' for text, href in subs]
    parts.append("</div>")
    return "\n".join(parts)


def downloads_html(repo: str, release: dict | None, lang: str) -> str:
    """The download tiles, under the version line.

    Without a release, every tile links to the GitHub releases page and there
    is no version line. Use this to build the site without bundles.
    """
    if release is None:
        line, links = "", dict.fromkeys(BUNDLES, f"{repo}/releases/latest")
    else:
        words, _ = translated(lang, "release")
        words = words.replace("{version}", release["version"]).replace("{date}", release["date"])
        line = f'<p class="release">{words}</p>\n'
        links = {key: f"{SITE_ROOT}/{path}" for key, path in release["links"].items()}
    source = {key: translated(lang, "downloads", key)[0]
              for key in ("source", "releases", "issues")}
    tiles = [
        download_tile("linux", "Linux", links["Linux", "x86_64"],
                      [("x64", links["Linux", "x86_64"]),
                       ("arm64", links["Linux", "arm64"])]),
        download_tile("windows", "Windows", links["Windows", "x86_64"],
                      [("x64", links["Windows", "x86_64"]),
                       ("arm64", links["Windows", "arm64"])]),
        download_tile("apple", "macOS 26+", links["macOS", "arm64"],
                      [("Apple Silicon", links["macOS", "arm64"]),
                       ("Intel", links["macOS", "x86_64"])]),
        download_tile("github", source["source"], repo,
                      [(source["releases"], f"{repo}/releases"),
                       (source["issues"], f"{repo}/issues")]),
    ]
    return line + '<div class="downloads">\n' + "\n".join(tiles) + "\n</div>"


def read_strings(lang: str) -> dict:
    """A language's strings file, empty if the language has none."""
    path = CONTENT / lang / STRINGS
    if not path.is_file():
        return {}
    strings = yaml.safe_load(path.read_text(encoding="utf-8")) or {}
    if not isinstance(strings, dict):
        sys.exit(f"{path}: the file must be a YAML mapping")
    return strings


def translated(lang: str, *keys: str) -> tuple[str, Path]:
    """A string from a language's strings file, and the file it came from.

    Several keys point inside a mapping: ('downloads', 'source') is 'source' in
    the 'downloads' mapping. If the language doesn't have the string, it comes
    from the English file.
    """
    for folder in dict.fromkeys((lang, SOURCE)):
        words = read_strings(folder)
        for key in keys:
            words = words.get(key) if isinstance(words, dict) else None
        if words:
            return words, CONTENT / folder / STRINGS
    sys.exit(f"{CONTENT / SOURCE / STRINGS}: '{': '.join(keys)}' is missing, "
             f"and every language falls back to it")


def donation_html(conf: dict, lang: str, panel_id: str) -> str:
    """The donation appeal, with a large icon below it.

    The centered icon links to the donation page, the 'donate' entry in the
    build.yaml 'footer: links'. So the footer and the icon always link to the
    same page.
    """
    url = content.donate_url(conf)

    text, path = translated(lang, "donation")
    check_portable(path, text)

    return (markdown_to_html(text, panel_id)
            + f'\n<div class="donate">'
            + f'<a class="icon" href="{url}">{read_icon("hand-holding-heart")}</a>'
            + f'</div>')


def decimal_version(version: str) -> str:
    """The version as a decimal number, which ZeGrapher 3 reads as a double.

    ZeGrapher 3 compares this number with its own version, and 'v3.1.1' was
    built as 3.11. The first dot is the decimal point, and the later dots are
    dropped: '4.0.1' becomes 4.01, which is greater than any 3.x number. A
    pre-release suffix, such as '_beta1', is dropped too, so the pre-release
    reads as the release it leads to.
    """
    major, _, rest = re.match(r"\d+(?:\.\d+)*", version).group(0).partition(".")
    return f"{major}.{rest.replace('.', '')}" if rest else f"{major}.0"


def write_version_files(version: str, out_root: Path) -> None:
    """Write the two files that ZeGrapher reads to check for a new release.

    Both files are at the site root, where the app looks for them.
    'latest_tag' holds the version tag, which src/Utils/updatecheck.cpp
    compares with the app version. 'latest' holds the same version as a
    decimal number, for ZeGrapher 3.
    """
    (out_root / "latest_tag").write_text(f"v{version}\n", encoding="utf-8")
    (out_root / "latest").write_text(f"{decimal_version(version)}\n", encoding="utf-8")


def release_info(version: str, out_root: Path, no_bundles: bool) -> dict | None:
    """What the download section shows, or None with --no-bundles.

    A release goes online the day it is built, so the date under the tiles is
    the build date. The 2026-09-01 format reads the same in every language.
    """
    if no_bundles:
        return None
    return {
        "version": version,
        "date": date.today().isoformat(),
        "links": copy_bundles(read_bundles(DOWNLOADS, version), out_root),
    }


def insert_blocks(html: str, blocks: dict, md: Path) -> str:
    """Replace each '{{ name }}' paragraph with its generated markup.

    Markdown can't write a gallery, so the build generates it. The placeholder
    sets where the markup goes in the panel.
    """
    def replace(match: "re.Match") -> str:
        name = match.group(1)
        if name not in blocks:
            sys.exit(f"{md}: '{name}' is not a known block. "
                     f"The blocks are: {', '.join(sorted(BLOCKS))}")
        return blocks[name]

    return BLOCK.sub(replace, html)


def lazy_pictures(html: str) -> str:
    """Add loading="lazy" to each panel picture.

    All the panels of a language are in one page, and a browser fetches the
    pictures of hidden panels too, unless they load lazily. The landing panel
    picture comes from templates/intro.html, not from this HTML, so it loads
    with the page.
    """
    return html.replace("<img ", '<img loading="lazy" ')


def language_menu(names: dict, current: str) -> str:
    """The header menu that opens the page in another language.

    Each language has its own folder, so a link to another language is a link
    to its folder. <details> opens the list without JavaScript, and keeps it
    open until the next click.

    With one language, there is no menu.
    """
    if len(names) < 2:
        return ""
    items = [f'<li><a href="{SITE_ROOT}/{lang}/" lang="{lang}" '
             f'hreflang="{lang}">{name}</a></li>'
             for lang, name in names.items() if lang != current]
    # 'lang' tells screen readers which language each name is in, so they
    # pronounce it right.
    return ('<details>\n\t\t\t\t\t\t'
            f'<summary lang="{current}">{names[current]}</summary>\n\t\t\t\t\t\t'
            '<ul>\n\t\t\t\t\t\t\t'
            + "\n\t\t\t\t\t\t\t".join(items)
            + '\n\t\t\t\t\t\t</ul>\n\t\t\t\t\t</details>')


def footer_words(lang: str, links: dict) -> dict:
    """A language's footer text, from its strings file.

    Each key under 'footer:' is an entry in the build.yaml 'footer: links', and
    its value is the entry text.
    """
    words = read_strings(lang).get("footer") or {}
    unknown = sorted(set(words) - set(links))
    if unknown:
        sys.exit(f"{CONTENT / lang / STRINGS}: 'footer' has "
                 f"{', '.join(unknown)}, which 'footer: links' in {CONF_FILE} "
                 f"doesn't list")
    return words


def footer_html(footer: dict, lang: str) -> str:
    """The page footer, from the build.yaml 'footer'.

    The copyright comes first, then each 'links' entry in file order. Entry
    text comes from the language's strings file, or from the English one if a
    key is missing.

    An entry's text is the link text. If the entry also has a 'link-name', that
    name is the link, and the text goes before it as plain text. Names such as
    'Astral by HTML5 UP' are not translated.
    """
    links = footer["links"]
    words = footer_words(SOURCE, links)
    missing = sorted(set(links) - set(words))
    if missing:
        sys.exit(f"{CONTENT / SOURCE / STRINGS}: 'footer' is missing "
                 f"{', '.join(missing)}, and every language falls back to it")
    words.update(footer_words(lang, links))

    entries = [COPYRIGHT.format(year=date.today().year)]
    for key, link in links.items():
        url = content.link_url(link)
        name = link.get("link-name") if isinstance(link, dict) else None
        entries.append(f"{words[key]} [{name}]({url})" if name
                       else f"[{words[key]}]({url})")
    html = markdown_to_html("\n".join(f"- {entry}" for entry in entries), "footer")
    return f'<div id="footer">\n\t\t\t{html}\n\t\t</div>'


def htaccess(template: str, languages: list) -> str:
    """The server rules, with this build's languages.

    The rules match an alternation of the site languages. A browser that asks
    for another language gets the English page instead of a missing folder.
    """
    return fill(template, {"languages": "|".join(languages)})


def active_tab_style(panels: list) -> str:
    """A <style> that highlights the open panel's nav icon and shows its caret.

    No CSS selector matches "the link whose href is the current #fragment", so
    this writes one selector per panel id. With no target, the landing panel
    icon is active.

    A panel is open when the fragment is its id or one of its heading anchors,
    hence the ':has(#id:target, #id :target)' pair in every selector.
    """
    home = panels[0]["id"]
    nav_sel = [f'#wrapper:not(:has(#main :target)) #nav a[href="#{home}"]']
    nav_sel += [f'#wrapper:has(#{m["id"]}:target, #{m["id"]} :target) '
                f'#nav a[href="#{m["id"]}"]' for m in panels]
    # Show a panel's prev/next arrows only while the panel is open.
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
    """A panel's title and body.

    A panel file starts with a single '# ' heading: the panel title,
    translated like the rest of the file. markdown_to_html() turns it into the
    panel <h2>.
    """
    lines = body.lstrip("\n").split("\n")
    heading = re.match(r"^#\s+(.+?)\s*$", lines[0]) if lines else None
    if heading is None:
        sys.exit(f"{md}: a panel must open with a '# Title' heading")

    return heading.group(1), "\n".join(lines)


@dataclass
class Site:
    """What all the pages share."""
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
        md = content.file_of(lang, panel["file"])
        text = md.read_text(encoding="utf-8")
        check_portable(md, text)

        meta = dict(panel)
        # The landing panel comes first, and its template shows one picture.
        meta["template"] = "intro" if i == 0 else "section"
        if i == 0:
            meta["hero_image"] = picture_path(site.conf["hero-image"], lang)

        meta["title"], body = title_of(md, text)
        html = markdown_to_html(body, meta["id"])
        names, blocks = BLOCK.findall(html), {}
        if "gallery" in names:
            blocks["gallery"] = gallery_html(site.conf["gallery"], lang, meta["id"])
        if "downloads" in names:
            blocks["downloads"] = downloads_html(
                content.repository_url(site.conf), site.release, lang)
        if "donation" in names:
            blocks["donation"] = donation_html(site.conf, lang, meta["id"])
        meta["html"] = lazy_pictures(insert_blocks(html, blocks, md))
        meta["source"] = md
        panels.append(meta)

    # Second pass: a doc-to-doc link needs the target panel id, and the loop
    # above can render the link before it reads that panel.
    panel_ids = {m["file"]: m["id"] for m in panels}
    for m in panels:
        m["html"] = resolve_doc_links(m["html"], m["source"], panel_ids)

    # the labels go in HTML attributes, so escape() turns quotes into &quot;
    labels = {key: escape(translated(lang, "buttons", key)[0])
              for key in ("theme", "previous", "next")}

    page_title = translated(lang, "page_title")[0]
    tagline = translated(lang, "tagline")[0]

    nav, articles, arrow_links = [], [], []
    for i, m in enumerate(panels):
        # The landing panel picture links to the next panel.
        following = panels[i + 1]["id"] if i + 1 < len(panels) else ""
        # Localize the whole panel, so pictures from the template, such as the
        # intro hero, get rewritten like the markdown pictures.
        article = fill(site.templates[m["template"]],
                       {**m, "tagline": tagline, "next_id": following,
                        "content": m["html"]})
        articles.append(localize_pictures(article, lang, m["source"]))
        nav.append(f'<a href="#{m["id"]}" class="icon">'
                   f'{read_icon(m["icon"])}'
                   f'<span>{m["title"]}</span></a>')
        # This panel's prev/next page arrows, shown while the panel is open. The
        # first panel has no '<', and the last has no '>'.
        if i > 0:
            arrow_links.append(f'<a class="tab-arrow prev" data-panel="{m["id"]}" '
                               f'href="#{panels[i-1]["id"]}" aria-label="{labels["previous"]}">'
                               f'{read_icon("chevron-left")}</a>')
        if i < len(panels) - 1:
            arrow_links.append(f'<a class="tab-arrow next" data-panel="{m["id"]}" '
                               f'href="#{panels[i+1]["id"]}" aria-label="{labels["next"]}">'
                               f'{read_icon("chevron-right")}</a>')

    page = fill(site.templates["layout"], {
        "lang": lang,
        "dir": "rtl" if lang in RIGHT_TO_LEFT else "ltr",
        "title": page_title,
        "description": escape(tagline),
        "css_version": site.css_version,
        "nav": "\n\t\t\t\t\t".join(nav),
        "languages": language_menu(site.languages, lang),
        "theme_label": labels["theme"],
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
                    help="output folder (default: build-website/ at the "
                         "repository root, which is not committed)")
    ap.add_argument("--no-bundles", action="store_true",
                    help="link the download tiles to the GitHub releases page, "
                         "instead of the deploy/ bundles")
    args = ap.parse_args()

    if not shutil.which("pandoc"):
        sys.exit("pandoc is not on PATH. Install pandoc, then run this again.")

    conf = read_conf()
    panels = read_panels(conf["icon-map"])
    languages = read_languages()

    templates = {
        name: (TEMPLATES / f"{name}.html").read_text(encoding="utf-8")
        for name in ("layout", "section", "intro")
    }
    # Added to the main.css <link>. It changes with the CSS, so browsers can't
    # serve a stale stylesheet after a rebuild.
    css_version = hashlib.md5(
        (ASSETS / "css/main.css").read_bytes()).hexdigest()[:8]

    if not any((ASSETS / "fonts").glob("*.woff2")):
        print(f"warning: {ASSETS / 'fonts'} has no font. Run "
              f"'fetch_fonts.py', next to this script, to download the web font", file=sys.stderr)

    args.out.mkdir(parents=True, exist_ok=True)
    shutil.copytree(ASSETS, args.out / "assets", dirs_exist_ok=True)
    if not APP_ICON.is_file():
        sys.exit(f"{APP_ICON} is missing: it is the site logo")
    logo = args.out / "assets" / "images" / APP_ICON.name
    logo.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(APP_ICON, logo)
    (args.out / ".htaccess").write_text(
        htaccess(HTACCESS_FILE.read_text(encoding="utf-8"), list(languages)),
        encoding="utf-8")

    version = releases.project_version()
    release = release_info(version, args.out, args.no_bundles)
    write_version_files(version, args.out)

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
