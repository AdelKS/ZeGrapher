"""The releases of ZeGrapher, out of appdata/release-notes.md.

The grammar of a tag and the check of the notes file live here, and every
script that reads a version or a release goes through this file.

A heading of the notes file names one release, as '## v4.0.0 (2026-09-01)', or a
span of releases, as '## v3.1.1 - v4.0.0 (2026-09-01)'. The span holds every
release after '<from>', up to '<to>'. '<from>' itself stays out of the span: it
is the release the changes are counted from. Every release of a span carries the
text under that heading, and the date in the heading is the day '<to>' went out.

The headings run newest first, and no two of them hold the same release. A
heading can name a release that has no tag yet. Between two releases the tree
carries the version written in meson.build, and pending_tag() reads it.
"""

import re
import sys
from dataclasses import dataclass
from pathlib import Path

NOTES_NAME = "release-notes.md"

# 'v4.0.0' names a release, 'v4.0.0_beta2' a pre-release of it, and 'v3.1' a
# release written without its patch number
TAG = re.compile(r"^v(?P<major>[0-9]+)\.(?P<minor>[0-9]+)(?:\.(?P<patch>[0-9]+))?"
                 r"(?:_(?P<stage>alpha|beta|rc)(?P<number>[0-9]+))?$")

# the stages in order. A final release comes after all three
STAGES = {"alpha": 0, "beta": 1, "rc": 2}
FINAL_STAGE = 3

# '## v4.0.0 (2026-09-01)', and the markdown under it
HEADING = re.compile(r"^## +(.+?)[ \t]*$(.*?)(?=^## |\Z)", re.M | re.S)

# the day that closes a heading. The tags come before it, and the day holds
# dashes of its own, so it comes off before the tags are split
DATED = re.compile(r"^(?P<tags>.*\S)[ \t]+\((?P<date>[0-9]{4}-[0-9]{2}-[0-9]{2})\)$")

# the version of the project, out of the project() call of meson.build
MESON_VERSION = re.compile(r"^\s*version\s*:\s*'([^']+)'", re.M)

# '4.0.0_rc0-dev' is a tree between the release before it and '4.0.0_rc0'
DEV = re.compile(r"-dev$")


@dataclass
class Release:
    """One release that a heading of the notes file names."""

    tag: str
    """'v4.0.0', the tag the way git writes it"""

    date: str
    """the day the release went out, as 2026-09-01"""

    summary: str
    """what the notes file writes about the release"""

    @property
    def version(self):
        """'4.0.0', the tag without the letter that opens it"""
        return self.tag[1:]


@dataclass
class Span:
    """One heading of appdata/release-notes.md."""

    after: tuple
    """the key of '<from>', the release that the span counts its changes from"""

    newest: tuple
    """the key of '<to>', the newest release that the span holds"""

    newest_tag: str
    """'<to>' the way the heading writes it"""

    date: str
    """the day that '<to>' went out, as 2026-09-01"""

    summary: str
    """the markdown under the heading"""

    def covers(self, key: tuple) -> bool:
        """Whether the span holds the release of that key.

        '<from>' stays out of the span. A heading of one tag names that tag
        twice, and it holds that release alone.
        """
        return key == self.newest or self.after < key < self.newest


def version_key(tag: str) -> tuple[int, ...] | None:
    """The parts of a tag, in the order that compares two of them.

    src/Utils/versionparser.h orders a version the same way, and it also reads
    two forms that no tag carries: the '-r<n>' of a continuous build, and the
    '-dev' of a tree between two releases.
    """
    match = TAG.match(tag)
    if match is None:
        return None

    return (int(match["major"]), int(match["minor"]), int(match["patch"] or 0),
            STAGES.get(match["stage"], FINAL_STAGE), int(match["number"] or 0))


def notes(root: Path) -> list[Span]:
    """The spans of appdata/release-notes.md, the newest one first."""
    path = root / "appdata" / NOTES_NAME
    found = []

    for heading, body in HEADING.findall(path.read_text(encoding="utf-8")):
        dated = DATED.match(heading)
        if dated is None:
            sys.exit(f"{path}: '## {heading}' names no day. A heading closes on "
                     f"the day the release went out, such as "
                     f"'## v4.0.0 (2026-09-01)'")

        tags = [part.strip() for part in dated["tags"].split("-")]

        if len(tags) > 2 or any(TAG.match(tag) is None for tag in tags):
            sys.exit(f"{path}: '## {heading}' names no release. A heading names "
                     f"one tag, such as '## v4.0.0 (2026-09-01)', or the two "
                     f"tags of a span, such as "
                     f"'## v3.1.1 - v4.0.0 (2026-09-01)'")

        span = Span(after=version_key(tags[0]), newest=version_key(tags[-1]),
                    newest_tag=tags[-1], date=dated["date"],
                    summary=body.strip())

        if span.after > span.newest:
            sys.exit(f"{path}: '## {heading}' opens on the newer tag. A span "
                     f"names the release it counts from first")

        if found and span.newest >= found[-1].newest:
            sys.exit(f"{path}: '## {heading}' comes under a heading that names "
                     f"an older release. The headings come newest first")

        if found and found[-1].covers(span.newest):
            sys.exit(f"{path}: '## {heading}' ends inside the span above it, "
                     f"and no two headings hold the same release")

        found.append(span)

    return found


def announced(root: Path) -> list[Release]:
    """The releases that a software centre shows, the newest one first.

    A pre-release tells a software centre nothing, so a heading that ends on an
    '_alpha', a '_beta' or an '_rc' stays out. A release that is newer than the
    version in meson.build stays out too: a build of 4.0.0_rc0 is not 4.0.0. The
    releases before 4.0 predate this file, and appdata/ZeGrapher.metainfo.xml.in
    holds them.
    """
    built = version_key(pending_tag(root))

    return [Release(tag=span.newest_tag, date=span.date, summary=span.summary)
            for span in notes(root)
            if span.newest[3] == FINAL_STAGE and span.newest <= built]


def project_version(root: Path) -> str:
    """The version that meson.build names, such as '4.0.0_rc0-dev'."""
    path = root / "meson.build"
    match = MESON_VERSION.search(path.read_text(encoding="utf-8"))
    if match is None:
        sys.exit(f"{path} names no version, and the release under work follows "
                 f"it until it has a tag")

    return match.group(1)


def pending_tag(root: Path) -> str:
    """The tag that the release under work takes, out of meson.build."""
    version = project_version(root)
    tag = f"v{DEV.sub('', version)}"

    if not TAG.match(tag):
        sys.exit(f"meson.build names the version '{version}', and '{tag}' is no "
                 f"tag of a release")

    return tag


def release_type(version: str) -> str:
    """What a version is: 'dev', 'alpha', 'beta', 'rc' or 'full'.

    'dev' is a tree between two releases, and 'full' a release that is not a
    pre-release. The version is written without the 'v' of a tag, as meson.build
    and the name of a bundle write it.
    """
    if DEV.search(version):
        return "dev"

    match = TAG.match(f"v{version}")
    if match is None:
        sys.exit(f"'{version}' is not the version of a release, such as '4.0.0' "
                 f"or '4.0.0_rc1'")

    return match["stage"] or "full"
