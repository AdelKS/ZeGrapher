# The scripts of the website

Two scripts live here. Each one reads or writes the folders of the website,
which sits one level up. Run each command below from this folder.

- `build-website.py` — builds the site out of the markdown
- `fetch_icon.py` — downloads one icon
- `fetch_fonts.py` — downloads the web font

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

