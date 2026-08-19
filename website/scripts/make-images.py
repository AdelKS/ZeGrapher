#!/usr/bin/env python3
"""make-images.py [--reprompt-crops] [--crop-only] [--lang <code>[,<code>...]] [capture...]

Takes every picture the documentation, the README and the website show, and
writes each one to the path make-images/pictures.yaml gives it. With no name,
it does every capture of the list. Name captures, and it redoes those only:

    make-images.py input-states tab-graph
    make-images.py --reprompt-crops tab-graph
    make-images.py --lang fr tab-csv

--reprompt-crops opens each picture in pick.html before the cut, to draw its
rectangle. One rectangle serves every language, so you draw it on the captures
of the first language of the run.

--crop-only cuts the captures that are already there. It takes no new capture,
and it builds nothing.

--lang does those languages only. Without it, the run does every language the
app has a translation for. A code the app has no translation for is an error.

CAPTURES says where the captures land, PICTURES which list to read, BUILD_DIR
which build directory to compile in and take the app from.

While it runs, leave the machine alone: it captures the active window.

The run needs a KDE session for spectacle, PyYAML and Pillow.

One document serves every language. make-images/documents/overrides/<code>/
holds what changes in a language, README.md documents that folder.

README.md documents the list, the documents, the patches and the rectangles.
"""

import argparse
import functools
import http.server
import os
import subprocess
import sys
import threading
import urllib.parse
import webbrowser
from pathlib import Path

import yaml
from PIL import Image, ImageChops

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
# the list, the documents it names, the patches and the page that draws a rectangle
DATA = HERE / "make-images"
DOCUMENTS = DATA / "documents"
OVERRIDES = DOCUMENTS / "overrides"
PICKER = DATA / "pick.html"

# the documents an override is merged into. Every other one it replaces whole
MERGED = {".zg", ".yaml"}

BUILD = Path(os.environ.get("BUILD_DIR") or ROOT / "build-zg-debug-clang")
LIST = Path(os.environ.get("PICTURES") or DATA / "pictures.yaml")
CAPTURES = Path(os.environ.get("CAPTURES") or ROOT / "captures")

# the radius the compositor draws the window corners with, measured on the alpha
# of a capture. In the pixels of the captures, so it follows their scale
RADIUS = 10


def fail(message):
    sys.exit(f"make-images.py: {message}")


def run(argv):
    """Runs a command, quietly. A command that fails prints its log and stops the run."""
    done = subprocess.run(argv, capture_output=True, text=True)
    if done.returncode != 0:
        sys.stderr.write(done.stdout + done.stderr)
        fail(f"{argv[0]} failed")
    return done.stdout


def git(*args):
    return run(["git", "-C", str(ROOT), *args])


def build():
    run(["meson", "compile", "-C", str(BUILD)])


def near(path):
    """The absolute form of a path, which the list gives from the repository root."""
    return Path(os.path.normpath(ROOT / path))


# ---------------------------------------------------------------- the list


def languages():
    """Every language the app has a translation for, English first.

    Read from the .ts files of translations/, so a language the app already
    supports needs no edit in the list. English needs no .ts file, so it is
    always in.
    """
    found = ["en"]
    for ts in sorted((ROOT / "translations").glob("ZeGrapher_*.ts")):
        found.append(ts.stem[len("ZeGrapher_"):])
    return found


def chosen(captures, names):
    """The captures named on the command line, all of them when there is no name.

    An unknown name is an error, so a typo stops the run instead of doing
    nothing.
    """
    known = [c["name"] for c in captures]
    for name in names:
        if name not in known:
            fail(f"no capture is named {name}. The file holds: " + ", ".join(known))
    return [c for c in captures if not names or c["name"] in names]


def write_rect(path, capture, index, rect):
    """Writes one rectangle back into the list, and leaves the rest of the file as it is.

    The parsed tree holds no position. yaml.compose keeps the line and the
    columns of every value, so the rewrite below works on those, and it replaces
    the characters of the rectangle and nothing else.
    """
    text = path.read_text()

    def field(node, name):
        return next(value for key, value in node.value if key.value == name)

    entry = next(c for c in field(yaml.compose(text), "captures").value
                 if field(c, "name").value == capture)
    node = field(field(entry, "images").value[index], "rect")

    lines = text.splitlines(keepends=True)
    line, start, end = node.start_mark.line, node.start_mark.column, node.end_mark.column
    lines[line] = lines[line][:start] + rect + lines[line][end:]
    path.write_text("".join(lines))


# ------------------------------------------------------------ the documents


def read_yaml(path):
    """The tree a YAML file holds, an empty mapping when the file is not there."""
    return (yaml.safe_load(path.read_text()) or {}) if path.is_file() else {}


def merge(base, over):
    """$over laid over $base, mapping into mapping and item into item.

    Two lists go together item by item, and an item the shorter one leaves out
    keeps what it holds. Every other value replaces the value under it.
    """
    if isinstance(base, dict) and isinstance(over, dict):
        merged = dict(base)
        for key, value in over.items():
            merged[key] = merge(merged.get(key), value)
        return merged
    if isinstance(base, list) and isinstance(over, list):
        together = [merge(a, b) for a, b in zip(base, over)]
        return together + base[len(over):] + over[len(base):]
    return over


class Documents:
    """The documents of one language, written over the folder they come from.

    The documents of that folder are the base ones. overrides/<code>/ holds what
    changes in a language: all.yaml goes over every document, and a file named
    after a document goes over that one. all.yaml comes last, so the language of
    the run is the one all.yaml names.

    A document that holds no YAML, such as the CSV that states.patch imports, is
    replaced by the override of the same name.

    In the folder itself, and not in a copy under /tmp, because states.patch
    opens the CSV from the directory of the document: a copy under /tmp shows a
    /tmp path in the CSV import panel, and loads nothing.
    """

    def __init__(self):
        self.base = {path: path.read_bytes()
                     for path in sorted(DOCUMENTS.iterdir()) if path.is_file()}
        self.lang = None

    def content(self, lang):
        """What every document of the folder holds in that language."""
        folder = OVERRIDES / lang
        every = read_yaml(folder / "all.yaml")
        held = {}
        for path, base in self.base.items():
            one = folder / path.name
            if path.suffix not in MERGED:
                held[path] = one.read_bytes() if one.is_file() else base
                continue
            over = merge(read_yaml(one), every)
            held[path] = base if not over else yaml.safe_dump(
                merge(yaml.safe_load(base.decode()), over),
                allow_unicode=True, sort_keys=False).encode()
        return held

    def check(self, langs):
        """Stops the run when a language leaves a document in another language.

        A language with no all.yaml of its own thus stops the run here, instead
        of taking a whole set of captures in the language of the base documents.
        """
        for lang in langs:
            for path, held in self.content(lang).items():
                if path.suffix not in MERGED:
                    continue
                spoken = yaml.safe_load(held.decode())["app"]["language"]
                if spoken != lang:
                    fail(f"the {lang} run opens {path.name} in {spoken}. Write "
                         f"\"app: {{language: {lang}}}\" in "
                         f"{(OVERRIDES / lang / 'all.yaml').relative_to(ROOT)}")

    def use(self, lang):
        """Writes every document of that language over the folder it comes from."""
        if lang == self.lang:
            return
        for path, held in self.content(lang).items():
            path.write_bytes(held)
        self.lang = lang

    def restore(self):
        """Writes the base documents back."""
        for path, base in self.base.items():
            path.write_bytes(base)
        self.lang = None


# ------------------------------------------------------------- the captures


class Patches:
    """Applies the patch each group of captures asks for, one patch at a time."""

    def __init__(self):
        self.applied = None

    def use(self, patch):
        """Takes the current patch off, applies this one in its place, then builds.

        The captures come grouped by patch, so this applies each patch one time.
        """
        if patch == self.applied:
            return
        self.revert()
        if patch:
            print(f"== states that need {patch.name}")
            git("apply", str(patch))
            self.applied = patch
        build()

    def revert(self):
        if self.applied:
            git("apply", "-R", str(self.applied))
            self.applied = None


def shoot(document, output):
    """Opens a document in ZeGrapher and captures its window, drop shadow included.

    The document gives the window size. Spectacle captures the window that is
    active when the delay ends, so nothing else must take the focus.
    """
    output.parent.mkdir(parents=True, exist_ok=True)
    app = subprocess.Popen([str(BUILD / "src" / "ZeGrapher"), str(document)],
                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        run(["spectacle", "--activewindow", "--background", "--nonotify",
             "--delay", "2000", "--output", str(output)])
    finally:
        # SIGKILL on purpose: if ZeGrapher closes normally, it overwrites
        # last-workbook.zg with the document we opened
        app.kill()
        app.wait()
    with Image.open(output) as shot:
        print(f"{output}: {shot.width}x{shot.height}")


def capture_all(captures, langs):
    """Takes one capture per document and per language, patch group by patch group."""
    if git("status", "--porcelain", "--", "src/QML").strip():
        fail("src/QML has uncommitted changes. Commit or stash them, then run this again")

    documents = Documents()
    documents.check(langs)
    patches = Patches()
    # whatever stops the run, the patch comes off and the documents go back to
    # the base ones, so the sources stay as they were. The build of the sources
    # without the patch comes back on the next run
    try:
        # the group without a patch captures the binary of the build directory,
        # which Patches.use never builds for it
        build()
        # Patches.use prints the header of every patched group. The group before
        # them has none, and prints one only if a name left it something to capture
        plain = False
        for capture in sorted(captures, key=lambda c: c.get("patch") or ""):
            patch = capture.get("patch")
            if not patch and not plain:
                print("== states from a document alone")
                plain = True
            patches.use(near(patch) if patch else None)
            # one document, shared by every language, opened once per language
            # with the overrides of that language laid over it
            document = near(capture["document"])
            for lang in langs:
                documents.use(lang)
                shoot(document, CAPTURES / lang / f"{capture['name']}.png")
        patches.use(None)
    finally:
        patches.revert()
        documents.restore()


# -------------------------------------------------------------- the picking


def pick_rect(capture, name, current, frame):
    """Serves pick.html for one picture and waits for the rectangle drawn in it.

    Opens the page in a browser, with that capture loaded and the frame above it.
    The rectangle the list holds now is drawn on the capture. Returns the
    rectangle the page sends back, or None when the page closes without an
    answer.
    """
    picked = None
    done = threading.Event()

    class Handler(http.server.SimpleHTTPRequestHandler):
        # served from the repository root: the page and the captures it loads are
        # in different folders, and a request cannot leave the folder that is served
        def __init__(self, *args, **kwargs):
            super().__init__(*args, directory=str(ROOT), **kwargs)

        def do_POST(self):
            nonlocal picked
            length = int(self.headers.get("Content-Length", 0))
            picked = self.rfile.read(length).decode().strip()
            self.send_response(204)
            self.end_headers()
            done.set()

        def log_message(self, *args):
            pass

    server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), Handler)
    threading.Thread(target=server.serve_forever, daemon=True).start()

    try:
        served = capture.resolve().relative_to(ROOT)
    except ValueError:
        fail(f"{capture} is outside {ROOT}, so the page cannot load it. Set "
             "CAPTURES to a directory of the repository")

    query = urllib.parse.urlencode({
        "img": "/" + str(served),
        "name": name,
        "current": current,
        "prompt": frame,
    })
    page = PICKER.relative_to(ROOT)
    print(f"  {name}: {frame or 'draw the rectangle'} — "
          f"http://127.0.0.1:{server.server_port}/{page}?{query}")
    webbrowser.open(f"http://127.0.0.1:{server.server_port}/{page}?{query}")

    try:
        done.wait()
    except KeyboardInterrupt:
        pass
    server.shutdown()
    return picked


def reprompt_all(captures, first):
    """Asks for one rectangle per picture, on the captures of the first language."""
    print("== rectangles")
    for capture in captures:
        for index, image in enumerate(capture["images"]):
            if image["rect"] == "whole":
                continue
            name = Path(image["output"]).stem
            shot = CAPTURES / first / f"{capture['name']}.png"
            picked = pick_rect(shot, name, image["rect"], image.get("frame", ""))
            if picked:
                write_rect(LIST, capture["name"], index, picked)
                print(f"  {name:<12} {image['rect']} -> {picked}")
                image["rect"] = picked
            else:
                print(f"  {name:<12} {image['rect']} (kept)")


# ------------------------------------------------------------- the cropping


@functools.cache
def corner_tile(radius, samples=16):
    """One rounded corner, as an opacity tile of radius by radius pixels.

    Each pixel holds the part of its area the disc of that radius covers, taken
    over a grid of sub-pixels. That coverage is the antialiasing of the arc.
    """
    step = 1 / samples
    data = bytearray()
    for y in range(radius):
        for x in range(radius):
            inside = 0
            for j in range(samples):
                dy = y + (j + 0.5) * step - radius
                for i in range(samples):
                    dx = x + (i + 0.5) * step - radius
                    if dx * dx + dy * dy <= radius * radius:
                        inside += 1
            data.append(round(255 * inside / samples ** 2))
    return Image.frombytes("L", (radius, radius), bytes(data))


def corner_mask(size, radius):
    """An opacity mask of that size, with its four corners cut to that radius."""
    corner = corner_tile(radius)
    width, height = size
    mask = Image.new("L", size, 255)
    mask.paste(corner, (0, 0))
    mask.paste(corner.transpose(Image.Transpose.FLIP_LEFT_RIGHT), (width - radius, 0))
    mask.paste(corner.transpose(Image.Transpose.FLIP_TOP_BOTTOM), (0, height - radius))
    mask.paste(corner.transpose(Image.Transpose.ROTATE_180), (width - radius, height - radius))
    return mask


def box_of(rect, size, output):
    """The corners of a WxH+X+Y rectangle, as Image.crop takes them."""
    try:
        extent, x, y = rect.split("+")
        width, height = (int(n) for n in extent.split("x"))
        x, y = int(x), int(y)
    except ValueError:
        fail(f"{output}: {rect} is not a rectangle of the form WxH+X+Y")
    if x < 0 or y < 0 or x + width > size[0] or y + height > size[1]:
        fail(f"{output}: the rectangle {rect} runs past the capture, which is "
             f"{size[0]}x{size[1]}. Draw it again with --reprompt-crops")
    return x, y, x + width, y + height


def cut(shot, rect, output):
    """Writes one picture out of a capture.

    A 'whole' picture is the capture encoded again, corners and shadow as the
    compositor drew them. Every other picture is cut by its rectangle, with its
    corners rounded the same way.
    """
    output.parent.mkdir(parents=True, exist_ok=True)
    with Image.open(shot) as capture:
        if rect == "whole":
            picture = capture.copy()
        else:
            picture = capture.crop(box_of(rect, capture.size, output)).convert("RGBA")
            mask = corner_mask(picture.size, RADIUS)
            picture.putalpha(ImageChops.multiply(picture.getchannel("A"), mask))
    # optimize writes the smallest of the zlib filters, and no metadata chunk
    # goes out that this call does not name
    picture.save(output, optimize=True, icc_profile=None)


def crop_all(captures, langs):
    print("== cropping")
    count = 0
    for lang in langs:
        for capture in captures:
            shot = CAPTURES / lang / f"{capture['name']}.png"
            if not shot.is_file():
                fail(f"{shot} is not there. Take the captures again without --crop-only")
            for image in capture["images"]:
                cut(shot, image["rect"], near(image["output"].replace("{lang}", lang)))
                count += 1
    print(f"{count} pictures written")


# ------------------------------------------------------------------- the run


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--reprompt-crops", action="store_true")
    parser.add_argument("--crop-only", action="store_true")
    parser.add_argument("--lang", default="")
    parser.add_argument("captures", nargs="*", metavar="capture")
    args = parser.parse_args()

    doc = yaml.safe_load(LIST.read_text())
    captures = chosen(doc["captures"], args.captures)
    for capture in captures:
        if not near(capture["document"]).is_file():
            fail(f"{near(capture['document'])} is not there")

    every = languages()
    wanted = args.lang.replace(",", " ").split()
    for code in wanted:
        if code not in every:
            fail(f"the app has no translation for {code}. It has: " + ", ".join(every))
    langs = [lang for lang in every if not wanted or lang in wanted]

    if not args.crop_only:
        capture_all(captures, langs)
    if args.reprompt_crops:
        reprompt_all(captures, langs[0])
    crop_all(captures, langs)


if __name__ == "__main__":
    main()
