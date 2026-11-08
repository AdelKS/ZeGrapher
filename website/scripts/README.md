# Website scripts

These scripts read and write the other `website/` folders. Run each command
below from this folder.

- `build-website.py` — builds the site from the markdown files
- `content.py` — finds each language's files, for the app build and for the
  scripts here
- `fetch_icon.py` — downloads an icon
- `fetch_fonts.py` — downloads the web font
- `make-images.py` — takes the app screenshots

# Build

Run `./build-website.py` to write the site to `../../build-website/`. The
script needs [pandoc](https://pandoc.org/) on `PATH`, and PyYAML.

The docstring at the top of [build-website.py](./build-website.py) has the
details.

# Icons

To add an icon:

1. Find the name of a solid icon on
   [fontawesome.com](https://fontawesome.com/search?o=r&m=free&s=solid).
2. Run `./fetch_icon.py <name>`. The script writes `<name>.svg` to
   [../assets/icons/](../assets/icons/).
3. For a panel icon, add it to the `icon-map` in
   [../build-config/build.yaml](../build-config/build.yaml). Other icons are
   named in `build-website.py`, where it inlines them.

# Fonts

Run `./fetch_fonts.py` to download the fonts again to
[../assets/fonts/](../assets/fonts/).

# Pictures

The project README and the website show screenshots of the app.
`make-images.py` takes them, using the files in
[./make-images](./make-images):

- `pictures.yaml` — every picture to take, its optional crop rectangle, and
  whether it keeps every color
- `documents/` — the `.zg` files that the app opens for the pictures. A
  document's `app:` block holds the app settings for its picture, and the
  script writes them to the app settings file
- `states.patch` — a patch to the QML sources, for states that no document can
  reach, such as a checked box or an unfolded panel
- `pick.html` — a page that shows a capture in the browser, so you can draw
  the crop rectangle with the mouse
- `physical-size.cpp` — a library that the app loads during a capture, so it
  sees the physical size of the monitor that the pictures are measured on

To take every picture again, run:

```sh
./make-images.py
```

To redo some pictures only, name their captures:

```sh
./make-images.py input-states tab-graph
```

To take one or a few languages only, name them:

```sh
./make-images.py --lang fr tab-csv
./make-images.py --lang en,fr
```

The other languages keep their pictures. Without `--lang`, the command takes
the pictures in every language the app is translated into.

All languages share each document, and
[make-images/documents/overrides/](./make-images/documents/overrides/) holds
the per-language changes. In each language code folder:

- `all.yaml` applies to every document in `make-images/documents/`. It sets
  the app language.
- a file named after a document applies to that document only. A `.zg` or
  `.yaml` document is merged key by key. Any other document, such as
  `free-fall.csv`, is replaced as a whole.

`all.yaml` is applied last, so it overrides a file named after a document.

To add a language:

1. Write the app translation to `translations/ZeGrapher_<code>.ts`.
2. Write `make-images/documents/overrides/<code>/all.yaml`, with the language
   code in it.
3. Write an override for each document with text in it, such as the notes of
   a data sheet or the header row of a CSV file.

If a language has no `all.yaml`, the command stops and names the file to write.

To crop the existing captures again, add `--crop-only`:

```sh
./make-images.py --crop-only
```

The command doesn't take captures or build anything.

To adjust the crop rectangles, add `--reprompt-crops`:

```sh
./make-images.py --reprompt-crops tab-graph
```

The command opens a web page for each cropped picture. Adjust the rectangle
with the mouse, then save it: the script writes it back to
[make-images/pictures.yaml](./make-images/pictures.yaml). All languages share
the same rectangle.

The command starts its own KWin, and takes the captures on a virtual screen.
You can keep working while it runs: it never touches the desktop, and nothing
on the desktop can spoil a capture. `make-images.py` documents that screen, and
the scale of the crop rectangles.

If a capture shows a window that is still opening, the app needed more time to
draw it. Raise the delay with `--settle`:

```sh
./make-images.py --settle 4000 tab-graph
```

The app starts with the text cursor in the y<sub>max</sub> field, and the
cursor blinks. If a capture shows the cursor, take it again.

The command needs:

- `kwin_wayland`, `spectacle` and `kscreen-doctor`, from KDE
- `dbus-run-session` and a systemd user session
- a C++ compiler, `pkg-config` and the Qt headers, for `physical-size.cpp`
- a meson build directory of the app, which it builds in
- PyYAML and Pillow

Run `./make-images.py --help` for its options, environment variables and input
files.
