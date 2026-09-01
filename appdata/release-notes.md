# Release notes

What the author writes about a release, under a `##` heading. The heading names
the git tag of that release, then the day it went out, as
`## v4.0.0 (2026-09-01)`. It can also name a span of releases, as
`## <from> - <to> (<date>)`. The span holds every release after `<from>`, up to
`<to>`. `<from>` itself stays out of the span: it is the release the changes are
counted from. Every release of a span carries the same text, and the date is the
day `<to>` went out.

The headings run newest first, and no two of them hold the same release. A
heading can name a tag that does not exist yet, and the date is then the day the
release is due.

appdata/generate-metainfo turns this file into the `<releases>` of the metainfo
file, which is what a software centre shows. The app shows a span as one entry,
with a link to the page of that release on GitHub.

## v3.1.1 - v4.0.0 (2026-09-01)

It took a long time, it took A LOT of work: I am happy to announce version 4.0!

- Modern-looking & GPU-accelerated interface, written in [Qt QML](https://doc.qt.io/qt-6/qtqml-index.html)
- A new math parsing library, [ZeCalculator](https://github.com/AdelKS/ZeCalculator)
  - Useful error messages that point at what is wrong in the expression
  - An arbitrary number of objects
  - Every object can be called in the expression of any other
- Improved parametric plots with animation
- Export to SVG, PDF and image formats, exactly as it looks on the screen
- Near-complete rewrite of the code base
