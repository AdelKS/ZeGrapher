# The scripts of the website

The scripts in this folder read and write the other folders of `website/`.
Run each command below from this folder.

- `build-website.py` — builds the site from the markdown
- `content.py` — finds the files of each language, for the build of the app and
  for the scripts here
- `fetch_icon.py` — downloads one icon
- `fetch_fonts.py` — downloads the web font
- `make-images.py` — takes the pictures of the app

# Build

Run `./build-website.py` to write the site to `../../build-website/`. The
script needs [pandoc](https://pandoc.org/) on `PATH`, and PyYAML.

See the docstring at the top of [build-website.py](./build-website.py) for more details.

# Icons

To add an icon

1. Find the name of a solid icon on
   [fontawesome.com](https://fontawesome.com/search?o=r&m=free&s=solid).
2. Run `./fetch_icon.py <name>`. The tool writes `<name>.svg` to
   [../assets/icons/](../assets/icons/).
3. Name the icon in the `icon-map` of
   [../build-config/build.yaml](../build-config/build.yaml).

# Fonts

Run `./fetch_fonts.py` to (re)fetch the fonts in
[../assets/fonts/](../assets/fonts/).

# Pictures

The README of the project and the website show pictures of the app.
`make-images.py` takes them using information from the
[./make-images](./make-images) folder:

- `pictures.yaml` — every picture to take, and how to optionally crop it
- `documents/` — the `.zg` files that the app opens for a picture
- `states.patch` — a patch on the QML sources, for the states no document
  reaches on its own, such as a checked box or an unfolded panel
- `pick.html` — a page that shows a capture in the browser, so you draw the
  crop rectangle with the mouse

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

CAUTION: Do not touch the keyboard or the mouse while the command runs. It
captures the active window.

The command needs:

- a KDE session, because it takes each capture with `spectacle`
- a meson build directory of the app, which it builds in
- PyYAML and Pillow

Run `./make-images.py --help` for its options, its environment variables, and
the files it reads.
