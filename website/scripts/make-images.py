#!/usr/bin/env python3
"""make-images.py [--reprompt-crops] [--crop-only] [--lang <code>[,<code>...]] [capture...]

Takes every picture the documentation, the README and the website show, and
writes each one to the path make-images/pictures.yaml gives it. With no name,
it takes every capture of the list. Name captures, and it redoes those only:

    make-images.py input-states tab-graph
    make-images.py --reprompt-crops tab-graph
    make-images.py --lang fr tab-csv

The captures are taken in a compositor of their own, on a screen no monitor
shows. They come out the same on every machine, and the desktop is left alone.
Keep working while the command runs.

--reprompt-crops opens each picture in pick.html before the cut, to draw its
rectangle with the mouse. The same rectangle is used in every language, so you
draw it on the captures of the first language of the run.

--crop-only cuts the captures that are already there. It takes no new capture,
and it builds nothing.

--lang takes those languages only. Without it, the run takes every language the
app is translated into. A code the app has no translation for is an error.

--settle is how long a window is given to draw, in milliseconds, before its
capture is taken. Raise it if a capture holds a window that is still opening.

CAPTURES says where the captures land, PICTURES which list to read, BUILD_DIR
which build directory to compile in and take the app from, SETTLE what --settle
defaults to.

The run needs kwin_wayland, spectacle, kscreen-doctor and dbus-run-session, all
of KDE, a C++ compiler and pkg-config for physical-size.cpp, plus PyYAML and
Pillow.

One document serves every language, and make-images/documents/overrides/<code>/
holds what changes in a language.

README.md, next to this script, documents the list, the documents, the patches
and the rectangles.
"""

import argparse
import contextlib
import functools
import http.server
import itertools
import json
import os
import shlex
import subprocess
import sys
import tempfile
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
PHYSICAL_SIZE = DATA / "physical-size.cpp"

# an override is merged into a document with one of these suffixes. It replaces
# any other document whole
MERGED = {".zg", ".yaml"}

BUILD = Path(os.environ.get("BUILD_DIR") or ROOT / "build-zg-debug-clang")
LIST = Path(os.environ.get("PICTURES") or DATA / "pictures.yaml")
CAPTURES = Path(os.environ.get("CAPTURES") or ROOT / "captures")

# the radius of the rounded corners KWin draws, measured on the alpha of a
# capture. It is in the pixels of the captures, so it follows SCALE
RADIUS = 10

# the screen the captures are taken on, a copy of the monitor the pictures are
# measured on. Every rectangle of the list is in the pixels SCALE gives, so a new
# SCALE needs every rectangle drawn again, and the pictureScale of
# src/QML/DocPane.qml, which draws the pictures at this scale. SCREEN is in those
# same pixels, and it must fit the biggest window of the list, its frame and its
# drop shadow. SCREEN_MM is the physical size of the monitor, which sets how big
# the app draws the graph: see physical-size.cpp
SCALE = 1.6
SCREEN = (3840, 2160)
SCREEN_MM = (699, 395)
# the socket KWin listens on, and the name it gives its only screen. One name
# for every run: a second run then stops on the socket, instead of patching the
# sources a run already patched
SOCKET = "zg-shot"
SCREEN_NAME = "Virtual-0"
# set on the run KWin starts, so that run takes the captures and nothing else
SEALED = "MAKE_IMAGES_SEALED"

# how long a window is given to draw before its capture is taken, in
# milliseconds
SETTLE = int(os.environ.get("SETTLE") or 2000)


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
    print("== compiling the app")
    run(["meson", "compile", "-C", str(BUILD)])


def near(path):
    """The absolute form of a path. The list gives every path from the repository root."""
    return Path(os.path.normpath(ROOT / path))


# ---------------------------------------------------------- the compositor


def sealed(argv):
    """Runs this script again, in a compositor of its own, and returns its status.

    kwin_wayland draws to a screen no monitor shows, and it starts this script
    on the socket of that screen. dbus-run-session gives that KWin a bus of its
    own, so it owns org.kde.KWin there. The spectacle started inside then talks
    to that KWin, and not to the compositor of the desktop.

    The screen has a known size and scale, and it holds one window, so every
    capture comes out the same on any machine and nothing can take the focus
    from the window. The desktop of the machine is untouched.
    """
    # --exit-with-session takes one command line, and KWin stops when that
    # command ends. An application named after -- is started with no arguments,
    # and KWin keeps running once it ends
    session = shlex.join([sys.executable, str(Path(__file__).resolve()), *argv])
    # KWin, the bus and the portals it starts all log to stderr. That log goes
    # to a file, and it is written out only when the run fails. A pipe would
    # hold the run here until every portal it started closes its end, long
    # after KWin is gone. stdout is left alone, so the run names each capture
    # as it takes it
    with tempfile.TemporaryFile("w+") as log:
        status = subprocess.call(
            ["dbus-run-session", "--",
             "kwin_wayland", "--virtual",
             "--width", str(SCREEN[0]), "--height", str(SCREEN[1]),
             "--socket", SOCKET, "--no-lockscreen",
             # without it, this KWin unregisters the shortcuts of the desktop
             # session and writes what is left to kglobalshortcutsrc
             "--no-global-shortcuts",
             "--exit-with-session", session],
            env={**os.environ, SEALED: "1"}, stderr=log)
        if status:
            log.seek(0)
            sys.stderr.write(log.read())
    return status


def rescale():
    """Sets the scale of the screen the captures are taken on.

    kwin_wayland takes a --scale, but it multiplies the size of the screen by
    it and leaves the scale at 1. The scale is set here instead, on the KWin
    that is already up.
    """
    run(["kscreen-doctor", f"output.{SCREEN_NAME}.scale.{SCALE}"])


def physical_size_library():
    """Compiles physical-size.cpp into the build directory, and returns the library.

    The app is started with this library preloaded, so it reads SCREEN_MM as
    the physical size of the screen.
    """
    library = BUILD / "make-images-physical-size.so"
    flags = run(["pkg-config", "--cflags", "--libs", "Qt6Gui"]).split()
    run(["c++", "-std=c++17", "-shared", "-fPIC",
         f"-DWIDTH_MM={SCREEN_MM[0]}", f"-DHEIGHT_MM={SCREEN_MM[1]}",
         str(PHYSICAL_SIZE), *flags, "-o", str(library)])
    return library


# ---------------------------------------------------------------- the list


def languages():
    """Every language the app has a translation for, English first.

    Read from the .ts files of translations/, so a language the app already
    supports needs no edit in the list. English has no .ts file, so it is added
    here.
    """
    return ["en"] + [ts.stem.removeprefix("ZeGrapher_")
                     for ts in sorted((ROOT / "translations").glob("ZeGrapher_*.ts"))]


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

    A parsed YAML tree holds no position, so it cannot say where to write.
    yaml.compose keeps the line and the columns of every value, and the rewrite
    below uses those to replace the characters of the rectangle alone.
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
    """'over' laid over 'base', mapping into mapping and item into item.

    Two lists are merged item by item. An item that only one list holds is kept
    as it is. Every other value replaces the value under it.
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

    It is a context manager: the base documents are written back when the block
    ends, whatever ends it.

    The documents of that folder are the base ones. overrides/<code>/ holds what
    changes in a language: all.yaml applies to every document, and a file named
    after a document applies to that one. all.yaml is applied last, so it sets
    the language of the run.

    A document that holds no YAML, such as the CSV that states.patch imports, is
    replaced by the override of the same name.

    The documents are written in the folder itself, and not in a copy under
    /tmp, because states.patch opens the CSV from the directory of the document.
    A copy under /tmp would show a /tmp path in the CSV import panel, and would
    load nothing.
    """

    def __init__(self):
        self.base = {path: path.read_bytes()
                     for path in sorted(DOCUMENTS.iterdir()) if path.is_file()}

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        for path, base in self.base.items():
            path.write_bytes(base)

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

        A language with no all.yaml of its own stops the run here, instead of
        taking a whole set of captures in the language of the base documents.
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
        for path, held in self.content(lang).items():
            path.write_bytes(held)


# ------------------------------------------------------------- the captures


@contextlib.contextmanager
def patched(patch):
    """The sources with that patch applied, for the time of the block.

    The patch comes off when the block ends, whatever ends it. None applies
    nothing.
    """
    if patch is None:
        yield
        return
    git("apply", str(patch))
    try:
        yield
    finally:
        git("apply", "-R", str(patch))


def write_settings(document, config, version):
    """Writes the settings file that the app reads for one document.

    The app reads its settings from settings.yaml in its config folder, and
    never from a document. A document of the list holds the settings of its
    capture under 'app:', and this writes them to settings.yaml under 'config',
    with the version of the build first, as the app writes the file when it
    closes.
    """
    held = yaml.safe_load(document.read_text(encoding="utf-8")) or {}
    settings = config / "ZeGrapher" / "settings.yaml"
    settings.parent.mkdir(parents=True, exist_ok=True)
    settings.write_text(yaml.safe_dump({"zegrapher": version, "app": held.get("app") or {}},
                                       allow_unicode=True, sort_keys=False),
                        encoding="utf-8")


def shoot(document, output, settle, preload, config):
    """Opens a document in ZeGrapher and captures its window, drop shadow included.

    The app reads its settings from 'config', which is XDG_CONFIG_HOME for the
    app, and they give the window size. Spectacle waits 'settle' milliseconds,
    then captures the window that is active. That wait is what the app has to
    draw its window in, and it must also outlast the animation KWin opens a
    window with, or the capture holds a window that is still fading in.
    """
    output.parent.mkdir(parents=True, exist_ok=True)
    # spectacle writes no file when no window is active, so a capture left by an
    # earlier run would pass for this one
    output.unlink(missing_ok=True)
    app = subprocess.Popen([str(BUILD / "src" / "ZeGrapher"), str(document)],
                           env={**os.environ, "LD_PRELOAD": str(preload),
                                "XDG_CONFIG_HOME": str(config)},
                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        run(["spectacle", "--activewindow", "--background", "--nonotify",
             "--delay", str(settle), "--output", str(output)])
    finally:
        app.kill()
        app.wait()
    if not output.is_file():
        fail(f"{document.name} drew no window in {settle} ms. Raise --settle")
    with Image.open(output) as shot:
        print(f"{output}: {shot.width}x{shot.height}")


def capture_all(captures, langs, settle):
    """Takes one capture per document and per language, patch group by patch group.

    The group without a patch comes last, even when it holds no capture. Its
    build is then the last one, and it leaves the build directory on the
    sources as they are.
    """
    if git("status", "--porcelain", "--", "src/QML").strip():
        fail("src/QML has uncommitted changes. Commit or stash them, then run this again")

    rescale()
    preload = physical_size_library()
    version = json.loads(run(["meson", "introspect", "--projectinfo", str(BUILD)]))["version"]
    patches = sorted({c["patch"] for c in captures if c.get("patch")})
    # the config folder of the app during the run, so the captures neither read
    # nor write the settings in ~/.config
    with Documents() as documents, tempfile.TemporaryDirectory() as config:
        documents.check(langs)
        for patch in patches + [None]:
            print(f"== states that need {patch}" if patch else "== states from a document alone")
            with patched(near(patch) if patch else None):
                build()
                group = [c for c in captures if c.get("patch") == patch]
                # one document serves every language. It is opened once per
                # language, with the overrides of that language laid over it
                for capture, lang in itertools.product(group, langs):
                    documents.use(lang)
                    document = near(capture["document"])
                    write_settings(document, Path(config), version)
                    shoot(document, CAPTURES / lang / f"{capture['name']}.png",
                          settle, preload, Path(config))


# -------------------------------------------------------------- the picking


def pick_rect(capture, name, current, frame):
    """Serves pick.html for one picture and waits for the rectangle drawn in it.

    Opens the page in a browser, with that capture loaded and the frame above
    it. The rectangle the list holds today is drawn on the capture. Returns the
    rectangle the page sends back, or None when the page closes without one.
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

    Each pixel holds how much of its area the disc of that radius covers,
    measured over a grid of sub-pixels. That coverage is the antialiasing of the
    arc.
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

    A 'whole' picture is the capture encoded again, with the corners and the
    shadow the compositor drew. Every other picture is cut by its rectangle, and
    its corners are rounded the same way.
    """
    output.parent.mkdir(parents=True, exist_ok=True)
    with Image.open(shot) as capture:
        if rect == "whole":
            picture = capture.copy()
        else:
            picture = capture.crop(box_of(rect, capture.size, output)).convert("RGBA")
            mask = corner_mask(picture.size, RADIUS)
            picture.putalpha(ImageChops.multiply(picture.getchannel("A"), mask))
    # optimize picks the zlib filter that writes the smallest file.
    # icc_profile=None drops the color profile, so no metadata is written
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
    parser.add_argument("--settle", type=int, default=SETTLE, metavar="MS")
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
        if os.environ.get(SEALED):
            # KWin started this run. It takes the captures and stops there.
            # The run that started it draws the rectangles and cuts the
            # pictures, on the desktop, where a browser can open
            capture_all(captures, langs, args.settle)
            return
        status = sealed(sys.argv[1:])
        if status:
            fail(f"the run that takes the captures ended with {status}")
    if args.reprompt_crops:
        reprompt_all(captures, langs[0])
    crop_all(captures, langs)


if __name__ == "__main__":
    main()
