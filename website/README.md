# The website of ZeGrapher

The site holds one page per language. `scripts/build-website.py` builds it out
of the markdown of that language, and writes it to `../build-website/`.

# The folders

- `assets/` — the css, the fonts, the icons and the images of the site
- `build-config/` — the icon of each panel, the gallery, the links of the
  footer, and the server rules
- `content/<lang>/` — the markdown of one language, and its pictures under
  `images/`. `footer.md` holds the words of the footer, and every other file is
  one panel of the page
- `download/` — the bundles of one release, which the tiles link
- `scripts/` — the four tools that build the site, and that fetch or take what
  it shows. [scripts/README.md](./scripts/README.md) documents how to run them
- `templates/` — the HTML that every page is filled into

# Languages

To add a language, copy `content/en/` to `content/<code>/` and translate the
files. Keep the file names. The front matter of `1-home.md` names the language
in its own words, under `language`, and the menu of the header shows that name.
A panel that a language does not translate stays English, and so does a picture
and an entry of the footer. The note at the head of `footer.md` stays English
too: it names the entries of the footer, and no page holds it.
