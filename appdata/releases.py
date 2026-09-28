#!/usr/bin/env python3
"""The releases of ZeGrapher, out of appdata/releases.yaml.

    releases.py (--version | --summary | --start-tag | --date
                 | --release-type | --yaml OUTPUT)

--version prints the version in meson.build, such as '4.0.0_rc0-dev'.

--summary prints what the release file writes about the release under work: the
announcement, then the changes entry. It is the body that the workflow gives to
'gh release create'. GitHub writes the list of changes under it.

--start-tag prints the release that those changes are counted from, and nothing
when GitHub can count from the release before.

--date prints the day of the release under work, such as '2026-09-01'. It
prints nothing when the release file doesn't date that release.

--release-type prints what the version in meson.build is: 'dev' between two
releases, 'alpha', 'beta' or 'rc' for a pre-release, and 'full' for a release.

--yaml checks the release file and copies it to OUTPUT, which the app embeds.

The grammar of a tag and the check of the release file live here, and every
script that reads a version or a release goes through this file. The top of
appdata/releases.yaml says what the file holds. Between two releases the tree
carries the version written in meson.build, and pending_tag() reads it.
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

# 'v4.0.0' names a release, 'v4.0.0_beta2' a pre-release of it, and 'v3.1' a
# release written without its patch number
TAG = re.compile(r"^v(?P<major>[0-9]+)\.(?P<minor>[0-9]+)(?:\.(?P<patch>[0-9]+))?"
                 r"(?:_(?P<stage>alpha|beta|rc)(?P<number>[0-9]+))?$")

# the stages in order. A final release comes after all three
STAGES = {"alpha": 0, "beta": 1, "rc": 2}
FINAL_STAGE = 3

DATE = re.compile(r"^[0-9]{4}-[0-9]{2}-[0-9]{2}$")

# the project version, from the project() call in meson.build
MESON_VERSION = re.compile(r"^\s*version\s*:\s*'([^']+)'", re.M)

# '4.0.0_rc0-dev' is a tree between the release before it and '4.0.0_rc0'
DEV = re.compile(r"-dev$")


@dataclass
class Entry:
    """One announcement or changes entry of appdata/releases.yaml."""

    after: tuple
    """the key of 'from', the release that the entry counts its changes from.
    It is the key of 'to' for an entry without 'from'"""

    after_tag: str
    """'from' the way the file writes it, empty for an entry without 'from'"""

    newest: tuple
    """the key of 'to', the newest release that the entry holds"""

    newest_tag: str
    """'to' the way the file writes it"""

    text: str
    """the markdown of the entry"""

    def covers(self, key: tuple) -> bool:
        """Whether the entry holds the release of that key.

        'from' stays out of the span, and an entry without 'from' holds 'to'
        alone.
        """
        return key == self.newest or self.after < key < self.newest

    def span(self) -> str:
        """'v3.1.1 - v4.0.0', or 'v4.0.0' for an entry without 'from'."""
        return f"{self.after_tag} - {self.newest_tag}" if self.after_tag else self.newest_tag


@dataclass
class Releases:
    """appdata/releases.yaml, after the check."""

    dates: dict[str, str]
    """the day of each release in 'releases', as 2026-09-01, newest first"""

    announcements: list[Entry]
    changes: list[Entry]


@dataclass
class Release:
    """One <release> of the metainfo file."""

    tag: str
    date: str
    text: str


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
            sys.exit(f"{RELEASES}: each item of 'releases' holds a 'tag' and a "
                     f"'date', and {item!r} doesn't")

        tag = check_tag(item["tag"], "releases")

        # PyYAML reads an unquoted 2026-09-01 as a datetime.date
        date = str(item["date"])
        if DATE.match(date) is None:
            sys.exit(f"{RELEASES}: the date of {tag} is '{date}', and it must be "
                     f"written as 2026-09-01")

        previous = next(reversed(found), None)
        if previous and version_key(tag) >= version_key(previous):
            sys.exit(f"{RELEASES}: in 'releases', {tag} comes after {previous}. The "
                     f"list runs newest first, and holds each tag once")

        found[tag] = date

    return found


def entries_of(name: str, items: list) -> list[Entry]:
    """The '<name>' list, after checking that it runs newest first and that no
    two of its spans overlap."""
    found = []

    for item in items:
        if (not isinstance(item, dict) or "to" not in item
                or set(item) - {"from", "to", "text"}
                or not isinstance(item.get("text", ""), str)):
            sys.exit(f"{RELEASES}: each item of '{name}' holds a 'to' tag, a "
                     f"'text', and can hold a 'from' tag, and {item!r} doesn't")

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
                     f"'{found[-1].span()}'. The list runs newest first")

        if found and found[-1].covers(entry.newest):
            sys.exit(f"{RELEASES}: in '{name}', '{entry.span()}' overlaps "
                     f"'{found[-1].span()}'. Two spans of the same list can't overlap")

        found.append(entry)

    return found


def load() -> Releases:
    """appdata/releases.yaml, after the check."""
    try:
        data = yaml.safe_load(RELEASES.read_text(encoding="utf-8"))
    except yaml.YAMLError as error:
        sys.exit(f"{RELEASES}: {error}")

    keys = ["releases", "announcements", "changes"]

    # src/Utils/releasenotes.h reads the file with glaze, which rejects an
    # unknown key and a null list
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
    """The entry that holds the release of that tag, None when no entry does."""
    key = version_key(tag)

    return next((entry for entry in entries if entry.covers(key)), None)


def summary_of(announcement: Entry | None, changes: Entry | None) -> str:
    """The announcement, then the changes entry under a heading that names its
    span."""
    if announcement is None or not announcement.text:
        return changes.text if changes else ""

    if changes is None or not changes.text:
        return announcement.text

    return f"{announcement.text}\n\n### {changes.span()}\n\n{changes.text}"


def start_tag_of(tag: str, announcement: Entry | None, changes: Entry | None) -> str:
    """The release that GitHub counts the changes of that release from.

    GitHub counts from the release before, which for a full release is its last
    release candidate. A full release counts from the 'from' of its
    announcement instead, and a pre-release from the 'from' of its changes
    entry. Each one takes the other entry when its own is missing. An entry
    without 'from' gives nothing, and GitHub counts from the release before.
    """
    if TAG.match(tag)["stage"] is None:
        entry = announcement or changes
    else:
        entry = changes or announcement

    return entry.after_tag if entry else ""


def announced() -> list[Release]:
    """The releases that a software center shows, the newest one first.

    A pre-release tells a software center nothing, so it stays out. A release
    that is newer than the version in meson.build stays out too: a build of
    4.0.0_rc0 is not 4.0.0. A release takes the text of the announcement that
    ends on it, or else the text of the changes entry that ends on it.
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
    """The version that meson.build names, such as '4.0.0_rc0-dev'."""
    match = MESON_VERSION.search(MESON_BUILD.read_text(encoding="utf-8"))
    if match is None:
        sys.exit(f"{MESON_BUILD} names no version, and the release under work "
                 f"follows it until it has a tag")

    return match.group(1)


def pending_tag() -> str:
    """The tag that the release under work takes, out of meson.build."""
    version = project_version()
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


def write_app_file(path: Path) -> None:
    """Check the release file, and copy it to the path that the app embeds."""
    load()
    text = RELEASES.read_text(encoding="utf-8")

    # skip identical writes: they still update the file's mtime, which reruns
    # every step that reads it
    if not path.is_file() or path.read_text(encoding="utf-8") != text:
        path.write_text(text, encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    action = parser.add_mutually_exclusive_group(required=True)
    action.add_argument("--version", action="store_true")
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
