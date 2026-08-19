# The scripts of the website

Each script reads or writes the folders of the website,
which sits one level up. Run each command below from this folder.

- `build-website.py` — builds the site out of the markdown
- `fetch_icon.py` — downloads one icon
- `fetch_fonts.py` — downloads the web font
- `make-images.py` — takes the pictures of the app

# Build

Run `./build-website.py` to write the site to `../../build-website/`. The
script needs [pandoc](https://pandoc.org/) on `PATH`, and PyYAML.

`build-website.py` documents the name of a panel, the front matter, the
generated blocks and the files that a release writes.

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
`make-images.py` takes them, from the list in
[make-images/pictures.yaml](./make-images/pictures.yaml). The `make-images/`
folder holds everything that list names: the documents the app opens, the
patches for the states no document reaches on its own, and the page that draws
a crop rectangle.

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
does every language the app has a translation for.

One document serves every language, and
[make-images/documents/overrides/](./make-images/documents/overrides/) holds
what changes in a language. Under the folder of a language code:

- `all.yaml` goes over every document of `make-images/documents/`. It carries
  the language the app opens in.
- a file named after a document goes over that document alone. A YAML document
  takes the keys of that file. Any other document, such as `free-fall.csv`, is
  replaced whole.

`all.yaml` comes last, so the language of the run is the one `all.yaml` names.

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

To draw a crop rectangle by hand instead of counting it, add
`--reprompt-crops`:

```sh
./make-images.py --reprompt-crops tab-graph
```

CAUTION: Do not touch the keyboard or the mouse while the command runs. It
captures the active window.

The command needs a KDE session, PyYAML, Pillow, and a meson build directory
of the app. Run `./make-images.py --help` for its options, its environment
variables, and the files it reads.
