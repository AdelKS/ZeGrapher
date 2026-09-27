# The scripts of the website

The scripts in this folder read and write the other folders of `website/`.
Run each command below from this folder.

- `content.py` — finds the files of each language, for the build of the app and
  for the scripts here
- `make-images.py` — takes the pictures of the app

# Pictures

The README of the project and the website show pictures of the app.
`make-images.py` takes them using information from the
[./make-images](./make-images) folder:

- `pictures.yaml` — every picture to take, and how to optionally crop it
- `documents/` — the `.zg` files that the app opens for a picture. The `app:`
  block of a document holds the settings of the app for that picture, and the
  script writes it to the settings file of the app
- `states.patch` — a patch on the QML sources, for the states no document
  reaches on its own, such as a checked box or an unfolded panel
- `pick.html` — a page that shows a capture in the browser, so you draw the
  crop rectangle with the mouse
- `physical-size.cpp` — a library that the app loads during a capture, so it
  sees the physical size of the monitor the pictures are measured on

To take every picture again, run:

```sh
./make-images.py
```

To redo some pictures only, name their captures:

```sh
./make-images.py input-states tab-graph
```

To do one language only, or a few, name them:

```sh
./make-images.py --lang fr tab-csv
./make-images.py --lang en,fr
```

The other languages keep the pictures they have. Without `--lang`, the command
takes the pictures of every language the app is translated into.

One document serves every language, and
[make-images/documents/overrides/](./make-images/documents/overrides/) holds
what changes in a language. Under the folder of a language code:

- `all.yaml` applies to every document of `make-images/documents/`. It holds
  the language the app opens in.
- a file named after a document applies to that document alone. A `.zg` or
  `.yaml` document is merged key by key. Any other document, such as
  `free-fall.csv`, is replaced whole.

`all.yaml` is applied last, so it wins over a file named after a document.

To add a language:

1. Write the translation of the app to `translations/ZeGrapher_<code>.ts`.
2. Write `make-images/documents/overrides/<code>/all.yaml`, with the language
   code in it.
3. Write an override for each document that holds words, such as the notes of
   a data sheet or the header row of a CSV file.

If a language has no `all.yaml`, the command stops and names the file to write.

To cut the pictures again out of the captures already taken, add
`--crop-only`:

```sh
./make-images.py --crop-only
```

The command takes no new capture, and it builds nothing.

To fine tune the crop areas, add `--reprompt-crops`:

```sh
./make-images.py --reprompt-crops tab-graph
```

The command opens a web page for each cropped picture, where you adjust its
crop area with the mouse then save it (saved back into
[make-images/pictures.yaml](./make-images/pictures.yaml)). The same crop area
is used in every language.

The command starts a KWin of its own, and takes the captures on a screen no
monitor shows. Keep working while it runs: it never touches the desktop, and
nothing on the desktop can spoil a capture. `make-images.py` documents that
screen, and the scale every crop area is drawn in.

A window that a capture caught while it was still opening means the app needed
longer to draw it. Give it more time with `--settle`:

```sh
./make-images.py --settle 4000 tab-graph
```

The app starts with the text cursor in the y<sub>max</sub> field, and the
cursor blinks. A capture can catch it while it shows. Take that capture again.

The command needs:

- `kwin_wayland`, `spectacle`, `kscreen-doctor` and `dbus-run-session`, all of
  KDE
- a C++ compiler, `pkg-config` and the headers of Qt, for `physical-size.cpp`
- a meson build directory of the app, which it builds in
- PyYAML and Pillow

Run `./make-images.py --help` for its options, its environment variables, and
the files it reads.
