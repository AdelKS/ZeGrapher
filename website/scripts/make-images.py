#!/usr/bin/env python3
"""make-images.py [--reprompt-crops] [--crop-only] [--lang <code>[,<code>...]] [capture...]

Takes the pictures that the manual, the README and the website show, and
writes each one to its path in make-images/pictures.yaml. Without arguments, it
takes every capture in the list. To redo some captures only, name them:

    make-images.py input-states tab-graph
    make-images.py --reprompt-crops tab-graph
    make-images.py --lang fr tab-csv

The captures run in a separate compositor, on a virtual screen. They come out
the same on every machine, and the desktop is left alone. You can keep working
while the command runs.

--reprompt-crops opens each picture in pick.html before cropping, so you can
draw its rectangle with the mouse. All languages share the rectangle, so you
draw it on the captures of the first language in the run.

--crop-only crops the existing captures. It doesn't take captures or build
anything.

--lang limits the run to those languages. Without it, the run takes every
language the app has a translation for. A code without a translation is an
error.

--settle is how long a window gets to draw before its capture, in
milliseconds. Raise it if a capture shows a window that is still opening.

Environment variables: CAPTURES sets the captures folder, PICTURES the list to
read, BUILD_DIR the build directory to compile and run the app from, and SETTLE
the --settle default.

The run needs:

- kwin_wayland, spectacle and kscreen-doctor, all of KDE
- dbus-run-session and a systemd user session
- a C++ compiler and pkg-config, for physical-size.cpp
- PyYAML and Pillow

All languages share each document, and make-images/documents/overrides/<code>/
holds the per-language changes.

The README.md next to this script documents the list, the documents, the
patches and the rectangles.
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
from PIL import Image, ImageChops, ImageMath, ImageStat

from content import RIGHT_TO_LEFT

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
# the list, the documents it names, the patches, and the rectangle picker page
DATA = HERE / "make-images"
DOCUMENTS = DATA / "documents"
OVERRIDES = DOCUMENTS / "overrides"
PICKER = DATA / "pick.html"
PHYSICAL_SIZE = DATA / "physical-size.cpp"

# overrides with these suffixes are merged into the document, and any other
# override replaces it whole
MERGED = {".zg", ".yaml"}

BUILD = Path(os.environ.get("BUILD_DIR") or ROOT / "build-zg-debug-clang")
LIST = Path(os.environ.get("PICTURES") or DATA / "pictures.yaml")
CAPTURES = Path(os.environ.get("CAPTURES") or ROOT / "captures")

# the KWin window corner radius, measured on a capture's alpha channel. It is
# in capture pixels, so it depends on SCALE
RADIUS = 10

# the palette entries for non-opaque pixels in a 256-color picture: the rounded
# corners and the window shadow
ALPHA_LEVELS = 64

# the ordered dither thresholds: a 4x4 Bayer matrix of 0 to 15, one row per
# bytes object
BAYER = (b"\x00\x08\x02\x0a", b"\x0c\x04\x0e\x06", b"\x03\x0b\x01\x09", b"\x0f\x07\x0d\x05")

# the capture screen, a copy of the monitor that the pictures are measured on.
# The list rectangles are in SCALE pixels, so a new SCALE needs every rectangle
# redrawn, and a new pictureScale in src/QML/DocPane.qml. SCREEN is in the same
# pixels, and must fit the largest window in the list with its frame and drop
# shadow. SCREEN_MM is the monitor's physical size, which sets the graph size
# in the app: see physical-size.cpp
SCALE = 1.6
SCREEN = (3840, 2160)
SCREEN_MM = (699, 395)
# the KWin socket, and the name of its only screen. Every run uses the same
# socket, so a second concurrent run fails on it instead of patching the
# already patched sources again
SOCKET = "zg-shot"
SCREEN_NAME = "Virtual-0"
# set in the run that KWin starts, which only takes the captures
SEALED = "MAKE_IMAGES_SEALED"

# how long a window gets to draw before the capture, in milliseconds
SETTLE = int(os.environ.get("SETTLE") or 2000)


def fail(message):
    sys.exit(f"make-images.py: {message}")


def run(argv):
    """Runs a command quietly. If it fails, prints its log and stops the run."""
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
    """The absolute path. The list paths start at the repository root."""
    return Path(os.path.normpath(ROOT / path))


# ---------------------------------------------------------- the compositor


def sealed(argv):
    """Runs this script again in a separate compositor, and returns its status.

    kwin_wayland draws to a virtual screen, and starts this script on that
    screen's socket. dbus-run-session gives that KWin its own bus, so it owns
    org.kde.KWin there. The spectacle started inside then talks to that KWin,
    not to the desktop compositor.

    The screen has a fixed size and scale and shows one window, so captures
    come out the same on any machine, and nothing can steal the focus. The
    desktop is left alone.
    """
    # --exit-with-session takes one command line, and KWin stops when it ends.
    # Programs given after -- start without arguments, and KWin keeps running
    # after they end
    session = shlex.join([sys.executable, str(Path(__file__).resolve()), *argv])
    # dbus-run-session stops its bus when KWin ends, but not the services the
    # bus started. xdg-desktop-portal-kde and the ksecretd it starts keep
    # running, each holding an inotify instance, and each run's leftovers add up
    # toward fs.inotify.max_user_instances. So start the run in a transient
    # scope, and stop the scope to kill every process left in it.
    # --slice-inherit keeps the scope in this script's slice, for example
    # ccd1.slice
    scope = f"make-images-{os.getpid()}.scope"
    # KWin, the bus and the portals it starts all log to stderr. That log goes
    # to a file, which is only printed when the run fails. A pipe would block
    # the run here until every portal closes its end, long after KWin is gone.
    # stdout stays as is, so the run names each capture as it takes it
    with tempfile.TemporaryFile("w+") as log:
        try:
            status = subprocess.call(
                ["systemd-run", "--user", "--scope", "--slice-inherit",
                 "--collect", "--quiet", f"--unit={scope}",
                 "dbus-run-session", "--",
                 "kwin_wayland", "--virtual",
                 "--width", str(SCREEN[0]), "--height", str(SCREEN[1]),
                 "--socket", SOCKET, "--no-lockscreen",
                 # without it, this KWin unregisters the desktop session's
                 # shortcuts and writes what is left to kglobalshortcutsrc
                 "--no-global-shortcuts",
                 "--exit-with-session", session],
                env={**os.environ, SEALED: "1"}, stderr=log)
        finally:
            # the scope is already gone if no process outlived KWin
            subprocess.run(["systemctl", "--user", "stop", scope], capture_output=True)
        if status:
            log.seek(0)
            sys.stderr.write(log.read())
    return status


def rescale():
    """Sets the capture screen scale.

    kwin_wayland's --scale multiplies the screen size and leaves the scale at
    1. So this sets the scale on the running KWin instead.
    """
    run(["kscreen-doctor", f"output.{SCREEN_NAME}.scale.{SCALE}"])


def physical_size_library():
    """Compiles physical-size.cpp in the build directory, and returns the library.

    The app starts with this library preloaded, so it sees SCREEN_MM as the
    screen's physical size.
    """
    library = BUILD / "make-images-physical-size.so"
    flags = run(["pkg-config", "--cflags", "--libs", "Qt6Gui"]).split()
    run(["c++", "-std=c++17", "-shared", "-fPIC",
         f"-DWIDTH_MM={SCREEN_MM[0]}", f"-DHEIGHT_MM={SCREEN_MM[1]}",
         str(PHYSICAL_SIZE), *flags, "-o", str(library)])
    return library


# ---------------------------------------------------------------- the list


def languages():
    """Every language the app is translated into, English first.

    The list comes from the .ts files in translations/, so a new translation
    needs no edit here. English has no .ts file, so it is added by hand.
    """
    return ["en"] + [ts.stem.removeprefix("ZeGrapher_")
                     for ts in sorted((ROOT / "translations").glob("ZeGrapher_*.ts"))]


def chosen(captures, names):
    """The captures named on the command line, or all of them.

    An unknown name is an error, so a typo stops the run instead of silently
    doing nothing.
    """
    known = [c["name"] for c in captures]
    for name in names:
        if name not in known:
            fail(f"no capture is named {name}. The list has: " + ", ".join(known))
    return [c for c in captures if not names or c["name"] in names]


def write_rect(path, capture, index, rect):
    """Writes one rectangle back into the list, and leaves the rest of the file unchanged.

    A parsed YAML tree has no positions. yaml.compose keeps the line and
    column of every value, which the rewrite below uses to replace only the
    rectangle.
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
    """A YAML file's content, or an empty dict if the file is missing."""
    return (yaml.safe_load(path.read_text()) or {}) if path.is_file() else {}


def merge(base, over):
    """'over' merged into 'base', key by key and item by item.

    Two lists merge item by item, and items in only one list are kept as is.
    Any other value in 'over' replaces the one in 'base'.
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
    """A language's documents, written over the base documents in their folder.

    It is a context manager: the base documents are restored when the block
    exits, however it exits.

    The files in the documents folder are the base documents. overrides/<code>/
    holds the per-language changes: all.yaml applies to every document, and a
    file named after a document applies to that document. all.yaml is applied
    last, so it sets the run language.

    A non-YAML document, such as the CSV that states.patch imports, is replaced
    by the override with the same name.

    The documents are written in place, not to a copy in /tmp, because
    states.patch opens the CSV from the document folder. A copy in /tmp would
    show a /tmp path in the CSV import panel, and wouldn't load anything.
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
        """Each document's content in that language."""
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
        """Stops the run if a document doesn't open in the run's language.

        This catches a language without an all.yaml before it takes a whole set
        of captures in the base document language.
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
        """Writes the language's documents over the base documents."""
        for path, held in self.content(lang).items():
            path.write_bytes(held)


# ------------------------------------------------------------- the captures


@contextlib.contextmanager
def patched(patch):
    """Applies the patch to the sources for the duration of the block.

    The patch is reverted when the block exits, however it exits. None applies
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
    """Writes the app settings file for one document.

    The app reads its settings from settings.yaml in its config folder, never
    from a document. Each document in the list holds its capture's settings
    under 'app:'. This writes them to settings.yaml under 'config', with the
    build version first, as the app does on close.
    """
    held = yaml.safe_load(document.read_text(encoding="utf-8")) or {}
    settings = config / "ZeGrapher" / "settings.yaml"
    settings.parent.mkdir(parents=True, exist_ok=True)
    settings.write_text(yaml.safe_dump({"zegrapher": version, "app": held.get("app") or {}},
                                       allow_unicode=True, sort_keys=False),
                        encoding="utf-8")


def shoot(document, output, settle, preload, config):
    """Opens a document in ZeGrapher and captures its window, drop shadow included.

    The app reads its settings, window size included, from 'config', which is
    its XDG_CONFIG_HOME. Spectacle waits 'settle' milliseconds, then captures
    the active window. The app must draw its window within that time, and KWin
    must finish its window opening animation, or the capture shows a window
    that is still fading in.
    """
    output.parent.mkdir(parents=True, exist_ok=True)
    # spectacle doesn't write a file when no window is active, so a capture from
    # an earlier run would pass for this one
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
        fail(f"{document.name} didn't show a window within {settle} ms. Raise --settle")
    with Image.open(output) as shot:
        print(f"{output}: {shot.width}x{shot.height}")


def capture_all(captures, langs, settle):
    """Takes one capture per document and language, one patch group at a time.

    The group without a patch comes last, even when it is empty. Its build is
    then the last one, so the build directory ends up matching the unpatched
    sources.
    """
    if git("status", "--porcelain", "--", "src/QML").strip():
        fail("src/QML has uncommitted changes. Commit or stash them, then run this again")

    rescale()
    preload = physical_size_library()
    version = json.loads(run(["meson", "introspect", "--projectinfo", str(BUILD)]))["version"]
    patches = sorted({c["patch"] for c in captures if c.get("patch")})
    # the app config folder during the run, so the captures don't read or write
    # the settings in ~/.config
    with Documents() as documents, tempfile.TemporaryDirectory() as config:
        documents.check(langs)
        for patch in patches + [None]:
            print(f"== states that need {patch}" if patch else "== states from a document alone")
            with patched(near(patch) if patch else None):
                build()
                group = [c for c in captures if c.get("patch") == patch]
                # all languages share the document. It opens once per language,
                # with that language's overrides applied
                for capture, lang in itertools.product(group, langs):
                    documents.use(lang)
                    document = near(capture["document"])
                    write_settings(document, Path(config), version)
                    shoot(document, CAPTURES / lang / f"{capture['name']}.png",
                          settle, preload, Path(config))


# -------------------------------------------------------------- the picking


def pick_rect(capture, name, current, frame):
    """Serves pick.html for one picture, and waits for the rectangle drawn in it.

    Opens the page in a browser, with the capture and its 'frame' prompt. The
    current list rectangle is drawn on the capture. Returns the rectangle that
    the page sends back, or None if the page closes without one.
    """
    picked = None
    done = threading.Event()

    class Handler(http.server.SimpleHTTPRequestHandler):
        # served from the repository root: the page and its captures are in
        # different folders, and requests can't leave the served folder
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
        fail(f"{capture} is outside {ROOT}, so the page can't load it. Set "
             "CAPTURES to a directory inside the repository")

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
    """Asks for one rectangle per picture, on the first language's captures."""
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
    """One rounded corner, as a radius x radius opacity tile.

    Each pixel holds the share of its area inside the disc, measured over a
    grid of sub-pixels. That coverage antialiases the arc.
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
    """An opacity mask of that size, with its four corners rounded to that radius."""
    corner = corner_tile(radius)
    width, height = size
    mask = Image.new("L", size, 255)
    mask.paste(corner, (0, 0))
    mask.paste(corner.transpose(Image.Transpose.FLIP_LEFT_RIGHT), (width - radius, 0))
    mask.paste(corner.transpose(Image.Transpose.FLIP_TOP_BOTTOM), (0, height - radius))
    mask.paste(corner.transpose(Image.Transpose.ROTATE_180), (width - radius, height - radius))
    return mask


def box_of(rect, size, output, mirrored):
    """The corners of a WxH+X+Y rectangle, as Image.crop takes them.

    A right-to-left language mirrors the window, which is centered in the
    capture, so the rectangle is mirrored too.
    """
    try:
        extent, x, y = rect.split("+")
        width, height = (int(n) for n in extent.split("x"))
        x, y = int(x), int(y)
    except ValueError:
        fail(f"{output}: {rect} is not a WxH+X+Y rectangle")
    if mirrored:
        x = size[0] - x - width
    if x < 0 or y < 0 or x + width > size[0] or y + height > size[1]:
        fail(f"{output}: the rectangle {rect} goes past the {size[0]}x{size[1]} "
             f"capture. Draw it again with --reprompt-crops")
    return x, y, x + width, y + height


def reduce_colors(picture):
    """The picture with a 256-color palette.

    Pillow dithers only RGB pictures. So the opaque pixels get a median cut
    palette of the RGB picture, with Floyd-Steinberg dithering. The other
    pixels get ALPHA_LEVELS evenly spaced alpha levels with an ordered dither,
    each level in the mean color of its pixels.
    """
    rgba = picture.convert("RGBA")
    rgb = rgba.convert("RGB")
    alpha = rgba.getchannel("A")
    opaque = alpha.point(lambda a: 255 if a == 255 else 0)

    width, height = rgba.size
    rows = [(row * (width // 4 + 1))[:width] for row in BAYER]
    bayer = Image.frombytes("L", rgba.size, b"".join(rows[y % 4] for y in range(height)))

    # the level of each pixel, floor(a / step + b / 16) with
    # step = 255 / (ALPHA_LEVELS - 1), and 255 on an opaque pixel
    level = ImageMath.lambda_eval(
        lambda x: (x["a"] * (ALPHA_LEVELS - 1) * 16 + x["b"] * 255) / (255 * 16),
        a=alpha, b=bayer).convert("L")
    level.paste(255, mask=opaque)
    counts = level.histogram()
    levels = [i for i in range(ALPHA_LEVELS) if counts[i]]

    quantized = rgb.quantize(256 - len(levels), method=Image.Quantize.MEDIANCUT,
                             dither=Image.Dither.FLOYDSTEINBERG)
    first = quantized.getextrema()[1] + 1
    palette = quantized.getpalette()[:3 * first]
    transparency = [255] * first

    lut = [0] * 256
    for entry, i in enumerate(levels, start=first):
        lut[i] = entry
        mask = level.point(lambda v, i=i: 255 if v == i else 0)
        palette += [round(c) for c in ImageStat.Stat(rgb, mask).mean]
        transparency.append(round(ImageStat.Stat(alpha, mask).mean[0]))

    indices = level.point(lut)
    indices.paste(Image.frombytes("L", rgba.size, quantized.tobytes()), mask=opaque)
    reduced = Image.frombytes("P", rgba.size, indices.tobytes())
    reduced.putpalette(palette)
    if levels:
        reduced.info["transparency"] = bytes(transparency)
    return reduced


def cut(shot, rect, output, full_color, mirrored):
    """Writes one picture from a capture.

    A 'whole' picture is the capture encoded again, with the compositor's
    corners and shadow. Other pictures are cropped to their rectangle, and get
    the same rounded corners.
    """
    output.parent.mkdir(parents=True, exist_ok=True)
    with Image.open(shot) as capture:
        if rect == "whole":
            picture = capture.copy()
        else:
            picture = capture.crop(box_of(rect, capture.size, output, mirrored)).convert("RGBA")
            mask = corner_mask(picture.size, RADIUS)
            picture.putalpha(ImageChops.multiply(picture.getchannel("A"), mask))
    if not full_color:
        picture = reduce_colors(picture)
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
                fail(f"{shot} is missing. Take the captures again without --crop-only")
            for image in capture["images"]:
                cut(shot, image["rect"], near(image["output"].replace("{lang}", lang)),
                    image.get("full-color", False), lang in RIGHT_TO_LEFT)
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
            fail(f"{near(capture['document'])} is missing")

    every = languages()
    wanted = args.lang.replace(",", " ").split()
    for code in wanted:
        if code not in every:
            fail(f"the app has no {code} translation. It has: " + ", ".join(every))
    langs = [lang for lang in every if not wanted or lang in wanted]
    if args.reprompt_crops and langs[0] in RIGHT_TO_LEFT:
        fail(f"{langs[0]} mirrors the window. Draw the rectangles in a "
             "left-to-right language")

    if not args.crop_only:
        if os.environ.get(SEALED):
            # KWin started this run, which only takes the captures. The run
            # that started KWin draws the rectangles and crops the pictures on
            # the desktop, where a browser can open the picker page
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
