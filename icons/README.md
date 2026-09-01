# The icons of ZeGrapher

Most of this folder is the artwork of the interface. `resources.qrc` lists
those files, and the app loads them out of its resources.

The icon of the app is `ZeGrapher.svg`. It is the one file to edit, and
`make-icons.py` writes every raster out of it. Run the script from this folder
after each change of the drawing:

```sh
./make-icons.py
```

The script documents the files it writes, here and in the website folder.
`meson.build` names the files that meson installs.
