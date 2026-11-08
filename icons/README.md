# ZeGrapher icons

Most of this folder is interface artwork. `resources.qrc` lists those files,
and the app loads them from its resources.

Don't edit `ZeGrapher_*.png`, `ZeGrapher.ico` or `ZeGrapher.icns`:
`rasterize-zegrapher-svg.py` generates them from `ZeGrapher.svg`. To generate
them again, run:

```sh
./rasterize-zegrapher-svg.py
```

On Windows, `meson.build` compiles `ZeGrapher.rc`, which embeds
`ZeGrapher.ico` in `ZeGrapher.exe`.

The website uses `ZeGrapher.svg` as its logo and favicon.
`website/scripts/build-website.py` copies the file into the site at build time,
so the repository has a single copy.
