# The website of ZeGrapher

The site holds one page per language. `scripts/build-website.py` builds each
page from the markdown files of that language, and writes the site to
`../build-website/`.

# The folders

- `assets/` — the css, the fonts, the icons and the images of the site. The
  logo is not here: the build copies `icons/ZeGrapher.svg` into the site
- `build-config/` — the icon of each panel, the gallery, the links of the
  footer, and the server rules
- `content/<lang>/` — the markdown files of one language, its pictures under
  `images/`, and `strings.yaml`. Each markdown file is one panel of the page
- `download/` — the bundles of one release. The download tiles link to them
- `scripts/` — the scripts that build the site, and that gather some of its
  assets. [scripts/README.md](./scripts/README.md) documents how to run them
- `templates/` — the HTML templates that the build script fills with the
  content

# Languages

To add a language, copy `content/en/` to `content/<code>/` and translate the
files. Keep the file names.

`1-home.md` opens with a front matter block. It starts and ends with a line of
three dashes, and it holds three lines to translate:

```
---
language: Français
page_title: ZeGrapher, logiciel de tracé de courbes libre
tagline: Logiciel de tracé de courbes libre, gratuit et simple d'emploi. Il trace fonctions, suites, équations paramétriques et tableaux de valeurs dans le plan.
---
```

- `language` — the name of the language, written in that language. Every page
  has a drop-down list at the top, which switches between the languages of the
  site. Each language appears there under this name, so a reader finds their
  own.
- `page_title` — the title of the page. The browser shows it in the tab.
- `tagline` — the line under the title on the home panel.

A language does not have to translate everything. If a panel, a picture or a
footer entry is missing, the page shows the English one instead.

`strings.yaml` is not a panel. It holds the short strings that no panel carries:
the words of each entry of the footer, and the words that ask for a donation.
Translate the values, and keep the names in front of them. The app reads the
donation out of the same file, so those words hold markdown and nothing else.
