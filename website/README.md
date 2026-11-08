# ZeGrapher website

The site has one page per language. `scripts/build-website.py` builds each page
from that language's markdown files, and writes the site to
`../build-website/`.

# Folders

- `assets/` — the site's CSS, fonts, icons and images. The logo is not here:
  the build copies `icons/ZeGrapher.svg` into the site
- `build-config/` — panel icons, the gallery, footer links and server rules
- `content/<lang>/` — a language's markdown files, its pictures in `images/`,
  and `strings.yaml`. Each markdown file is one panel of the page
- `scripts/` — the scripts that build the site and fetch some of its assets.
  [scripts/README.md](./scripts/README.md) says how to run them
- `templates/` — the HTML templates that the build script fills in

# Languages

To add a language, copy `content/en/` to `content/<code>/` and translate the
files. Keep the file names.

Three strings in `strings.yaml` are for the whole page, not a panel:

- `language` — the language name, in that language. Each page has a language
  menu at the top, which lists the languages by this name so readers can find
  their own. This key has no English fallback, so every language must set it.
- `page_title` — the page title, which the browser shows in the tab.
- `tagline` — the line under the title on the home panel. Search engines also
  show it as the page description.

A language doesn't need to translate everything. If a panel, picture or footer
entry is missing, the page shows the English one.

`strings.yaml` holds the short strings outside the panels, such as the footer
text. Comments in the English file say where each string goes. Translate the
values, and keep the keys. The app reads the donation appeal from the same
file, so that string must be plain markdown.
