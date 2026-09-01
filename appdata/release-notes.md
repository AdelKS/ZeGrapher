# Release notes

What the author writes about a release, under a `##` heading. The heading names
the git tag of that release, then the day it went out, as
`## v4.0.0 (2026-09-01)`. It can also name a span, as
`## <from> - <to> (<date>)`. The span holds every release after `<from>`, up to
`<to>`. `<from>` stays out of the span: it is the release that the span counts
its changes from. Every release of a span carries the same text, and the day is
the day that `<to>` went out.

The headings come newest first, and no two of them hold the same release. A
heading can name a tag that does not exist yet, and it then names the day the
release is due.

appdata/generate-metainfo turns this file into the `<releases>` of the metainfo
file, which is what a software centre shows. The app draws a span as one entry,
with a link to the page of that release on GitHub.

## v3.1.1 - v4.0.0 (2026-09-01)

- Modern looking & GPU accelerated interface, written in [Qt QML](https://doc.qt.io/qt-6/qtqml-index.html)
- Write and use a new math parsing library [ZeCalculator](https://github.com/AdelKS/ZeCalculator)
  - Show useful error messages and what in the expression is wrong
  - Allows defining arbitrary number of objects
  - All objects can be called in any other's expression
- Improved parametric plots with animation
- Export to SVG, PDF and image formats that looks exactly like what's on the screen
- Near complete rewrite of the code base over the course of years of (discontinued) work
- AI/LLMs have only been used in the few months before the release,
  for brainstorming and non-important tasks (reworked several times to fit to my taste)
