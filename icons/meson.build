# ZeGrapher.svg is the icon of the app, and make-icons.py writes the rasters of
# it. appdata/meson.build installs the files that this file names.
app_icon_sizes = [16, 24, 32, 48, 64, 128, 256, 512]

app_icon_svg = files('ZeGrapher.svg')
app_icon_icns = files('ZeGrapher.icns')

app_icon_pngs = []
foreach size : app_icon_sizes
    app_icon_pngs += files('ZeGrapher_@0@.png'.format(size))
endforeach
