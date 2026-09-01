# The icons of ZeGrapher

Most of this folder is the artwork of the interface. `resources.qrc` lists
those files, and the app loads them from its resources.

Do not edit the `ZeGrapher_*.png`, `ZeGrapher.ico` and `ZeGrapher.icns` files.
They are generated from `ZeGrapher.svg`. To re-generate them, run:

```sh
./rasterize-zegrapher-svg.py
```

The docstring at the top of
[rasterize-zegrapher-svg.py](./rasterize-zegrapher-svg.py) lists the files it
writes.
