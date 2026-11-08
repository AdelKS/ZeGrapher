#!/usr/bin/env python3
"""Read ZeGrapher releases from appdata/releases.yaml.

    releases.py (--version | --release | --summary | --start-tag | --date
                 | --release-type | --yaml OUTPUT)

--version prints the version in meson.build, such as '4.0.0_rc0-dev'.

--release prints the release this version leads to, digits and dots only, such
as '4.0.0'.

--summary prints the upcoming release notes: the announcement, then the changes
entry. The workflow passes it to 'gh release create' as the body, and GitHub
appends its list of changes.

--start-tag prints the release GitHub must count those changes from, or nothing
when the previous release is right.

--date prints the upcoming release date, such as '2026-09-01', or nothing if
releases.yaml has no date for it.

--release-type prints the meson.build version type: 'dev' between two releases,
'alpha', 'beta' or 'rc' for a pre-release, and 'full' for a release.

--yaml checks releases.yaml and copies it to OUTPUT, which the app embeds.

This file holds the tag grammar and the releases.yaml checks. Every script that
reads a version or a release goes through it. The releases.yaml header
describes the file format.
"""

import argparse
import re
import sys
from dataclasses import dataclass
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parent.parent
RELEASES = ROOT / "appdata" / "releases.yaml"
MESON_BUILD = ROOT / "meson.build"

# 'v4.0.0' is a release, 'v4.0.0_beta2' one of its pre-releases, and 'v3.1' a
# release without a patch number
TAG = re.compile(r"^v(?P<major>[0-9]+)\.(?P<minor>[0-9]+)(?:\.(?P<patch>[0-9]+))?"
                 r"(?:_(?P<stage>alpha|beta|rc)(?P<number>[0-9]+))?$")

# the stages in order. A full release comes after all three
STAGES = {"alpha": 0, "beta": 1, "rc": 2}
FINAL_STAGE = 3

DATE = re.compile(r"^[0-9]{4}-[0-9]{2}-[0-9]{2}$")

# the project version, from the project() call in meson.build
MESON_VERSION = re.compile(r"^\s*version\s*:\s*'([^']+)'", re.M)

# '4.0.0_rc0-dev' is a tree between the previous release and '4.0.0_rc0'
DEV = re.compile(r"-dev$")


@dataclass
class Entry:
    """One announcement or changes entry in appdata/releases.yaml."""

    after: tuple
    """the 'from' key: the entry counts its changes from that release. Without
    'from', it is the 'to' key"""

    after_tag: str
    """'from' as written in the file, empty without 'from'"""

    newest: tuple
    """the 'to' key: the newest release in the entry"""

    newest_tag: str
    """'to' as written in the file"""

    text: str
    """the entry's markdown"""

    def covers(self, key: tuple) -> bool:
        """Whether the entry covers the release with that key.

        The span excludes 'from'. Without 'from', the entry covers 'to' alone.
        """
        return key == self.newest or self.after < key < self.newest

    def span(self) -> str:
        """'v3.1.1 - v4.0.0', or 'v4.0.0' for an entry without 'from'."""
        return f"{self.after_tag} - {self.newest_tag}" if self.after_tag else self.newest_tag


@dataclass
class Releases:
    """appdata/releases.yaml, checked."""

    dates: dict[str, str]
    """each release date in 'releases', as 2026-09-01, newest first"""

    announcements: list[Entry]
    changes: list[Entry]


@dataclass
class Release:
    """One <release> in the metainfo file."""

    tag: str
    date: str
    text: str


def version_key(tag: str) -> tuple[int, ...] | None:
    """A tag's parts, ordered so that comparing two keys compares the tags.

    src/Utils/versionparser.h orders versions the same way. It also reads two
    suffixes that tags never have: '-r<n>' for a continuous build, and '-dev'
    for a tree between two releases.
    """
    match = TAG.match(tag)
    if match is None:
        return None

    return (int(match["major"]), int(match["minor"]), int(match["patch"] or 0),
            STAGES.get(match["stage"], FINAL_STAGE), int(match["number"] or 0))


def check_tag(tag, where: str) -> str:
    if not isinstance(tag, str) or TAG.match(tag) is None:
        sys.exit(f"{RELEASES}: '{tag}' in '{where}' is not a release tag, such as "
                 f"'v4.0.0' or 'v4.0.0_rc1'")

    return tag


def dates_of(items: list) -> dict[str, str]:
    """The 'releases' list, as {tag: date}."""
    found = {}

    for item in items:
        if not isinstance(item, dict) or set(item) != {"tag", "date"}:
            sys.exit(f"{RELEASES}: each 'releases' item must have a 'tag' and "
                     f"a 'date', and {item!r} doesn't")

        tag = check_tag(item["tag"], "releases")

        # PyYAML reads an unquoted 2026-09-01 as a datetime.date
        date = str(item["date"])
        if DATE.match(date) is None:
            sys.exit(f"{RELEASES}: the {tag} date is '{date}', and it must be "
                     f"written as 2026-09-01")

        previous = next(reversed(found), None)
        if previous and version_key(tag) >= version_key(previous):
            sys.exit(f"{RELEASES}: in 'releases', {tag} comes after {previous}. The "
                     f"list must run newest first, with each tag once")

        found[tag] = date

    return found


def entries_of(name: str, items: list) -> list[Entry]:
    """The '<name>' list, checked to run newest first with no overlapping
    spans."""
    found = []

    for item in items:
        if (not isinstance(item, dict) or "to" not in item
                or set(item) - {"from", "to", "text"}
                or not isinstance(item.get("text", ""), str)):
            sys.exit(f"{RELEASES}: each '{name}' item must have a 'to' tag and "
                     f"a 'text', and can have a 'from' tag. {item!r} doesn't fit")

        newest_tag = check_tag(item["to"], name)
        after_tag = item.get("from", "")
        if after_tag:
            check_tag(after_tag, name)

        entry = Entry(after=version_key(after_tag or newest_tag), after_tag=after_tag,
                      newest=version_key(newest_tag), newest_tag=newest_tag,
                      text=item.get("text", "").strip())

        if after_tag and entry.after >= entry.newest:
            sys.exit(f"{RELEASES}: in '{name}', '{entry.span()}' must start on a "
                     f"release older than {newest_tag}")

        if found and entry.newest >= found[-1].newest:
            sys.exit(f"{RELEASES}: in '{name}', '{entry.span()}' comes after "
                     f"'{found[-1].span()}'. The list must run newest first")

        if found and found[-1].covers(entry.newest):
            sys.exit(f"{RELEASES}: in '{name}', '{entry.span()}' overlaps "
                     f"'{found[-1].span()}'. Spans in one list can't overlap")

        found.append(entry)

    return found


def load() -> Releases:
    """appdata/releases.yaml, checked."""
    try:
        data = yaml.safe_load(RELEASES.read_text(encoding="utf-8"))
    except yaml.YAMLError as error:
        sys.exit(f"{RELEASES}: {error}")

    keys = ["releases", "announcements", "changes"]

    # src/Utils/releasenotes.h reads the file with glaze, which rejects unknown
    # keys and null lists
    if not isinstance(data, dict) or set(data) - set(keys):
        sys.exit(f"{RELEASES} must hold only the lists 'releases', "
                 f"'announcements' and 'changes'")

    for key in keys:
        if not isinstance(data.setdefault(key, []), list):
            sys.exit(f"{RELEASES}: '{key}' must be a list, '[]' when it is empty")

    return Releases(dates=dates_of(data["releases"]),
                    announcements=entries_of("announcements", data["announcements"]),
                    changes=entries_of("changes", data["changes"]))


def covering(entries: list[Entry], tag: str) -> Entry | None:
    """The entry that covers the tag, or None."""
    key = version_key(tag)

    return next((entry for entry in entries if entry.covers(key)), None)


def summary_of(announcement: Entry | None, changes: Entry | None) -> str:
    """The announcement, then the changes entry under a heading with its
    span."""
    if announcement is None or not announcement.text:
        return changes.text if changes else ""

    if changes is None or not changes.text:
        return announcement.text

    return f"{announcement.text}\n\n### {changes.span()}\n\n{changes.text}"


def start_tag_of(tag: str, announcement: Entry | None, changes: Entry | None) -> str:
    """The release GitHub must count this release's changes from.

    GitHub counts from the previous release, which for a full release is its
    last release candidate. So a full release counts from its announcement's
    'from', and a pre-release from its changes entry's 'from'. Each falls back
    to the other entry. If the entry has no 'from', this returns nothing and
    GitHub counts from the previous release.
    """
    if TAG.match(tag)["stage"] is None:
        entry = announcement or changes
    else:
        entry = changes or announcement

    return entry.after_tag if entry else ""


def announced() -> list[Release]:
    """The releases that software centers show, newest first.

    Pre-releases are left out, and so are releases newer than the meson.build
    version: a 4.0.0_rc0 build is not 4.0.0. Each release takes the text of the
    announcement that ends on it, or else of the changes entry that ends on it.
    """
    built = version_key(pending_tag())
    found = load()

    def text_of(tag):
        ending = [entry for entry in found.announcements + found.changes
                  if entry.newest_tag == tag]
        return ending[0].text if ending else ""

    return [Release(tag=tag, date=date, text=text_of(tag))
            for tag, date in found.dates.items()
            if version_key(tag)[3] == FINAL_STAGE and version_key(tag) <= built]


def project_version() -> str:
    """The meson.build version, such as '4.0.0_rc0-dev'."""
    match = MESON_VERSION.search(MESON_BUILD.read_text(encoding="utf-8"))
    if match is None:
        sys.exit(f"{MESON_BUILD} has no project version")

    return match.group(1)


def pending_tag() -> str:
    """The upcoming release tag, from the meson.build version."""
    version = project_version()
    tag = f"v{DEV.sub('', version)}"

    if not TAG.match(tag):
        sys.exit(f"the meson.build version '{version}' gives '{tag}', which is "
                 f"not a release tag")

    return tag


def release_numbers() -> str:
    """The release the meson.build version leads to: '4.0.0' for
    '4.0.0_rc0-dev'."""
    match = TAG.match(pending_tag())
    return f"{match['major']}.{match['minor']}.{match['patch'] or 0}"


def release_type(version: str) -> str:
    """The version type: 'dev', 'alpha', 'beta', 'rc' or 'full'.

    'dev' is a tree between two releases, and 'full' is a release that is not a
    pre-release. The version has no leading 'v', as in meson.build and in bundle
    names.
    """
    if DEV.search(version):
        return "dev"

    match = TAG.match(f"v{version}")
    if match is None:
        sys.exit(f"'{version}' is not a release version, such as '4.0.0' "
                 f"or '4.0.0_rc1'")

    return match["stage"] or "full"


def write_app_file(path: Path) -> None:
    """Check releases.yaml, and copy it to the path the app embeds."""
    load()
    text = RELEASES.read_text(encoding="utf-8")

    # an identical write still updates the file's mtime and reruns every step
    # that reads the file, so skip it
    if not path.is_file() or path.read_text(encoding="utf-8") != text:
        path.write_text(text, encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument("--version", action="store_true")
    action.add_argument("--release", action="store_true")
    action.add_argument("--summary", action="store_true")
    action.add_argument("--start-tag", action="store_true")
    action.add_argument("--date", action="store_true")
    action.add_argument("--release-type", action="store_true")
    action.add_argument("--yaml", metavar="OUTPUT")
    args = parser.parse_args()

    if args.yaml is not None:
        write_app_file(Path(args.yaml))
    elif args.version:
        print(project_version())
    elif args.release:
        print(release_numbers())
    elif args.release_type:
        print(release_type(project_version()))
    else:
        tag = pending_tag()
        found = load()
        announcement = covering(found.announcements, tag)
        changes = covering(found.changes, tag)

        if args.summary:
            print(summary_of(announcement, changes))
        elif args.start_tag:
            print(start_tag_of(tag, announcement, changes))
        else:
            print(found.dates.get(tag, ""))

    return 0


if __name__ == "__main__":
    sys.exit(main())
