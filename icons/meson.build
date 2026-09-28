# ZeGrapher.svg is the app icon, and rasterize-zegrapher-svg.py writes its
# rasters. appdata/meson.build installs the files listed here, and
# src/meson.build links the Windows resource into the executable.
app_icon_sizes = [16, 24, 32, 48, 64, 128, 256, 512]

app_icon_svg = files('ZeGrapher.svg')
app_icon_icns = files('ZeGrapher.icns')

app_icon_pngs = []
foreach size : app_icon_sizes
    app_icon_pngs += files('ZeGrapher_@0@.png'.format(size))
endforeach

# the icon that Windows Explorer shows for ZeGrapher.exe
app_icon_windows = []
if host_machine.system() == 'windows'
    app_icon_windows = import('windows').compile_resources(
        'ZeGrapher.rc',
        depend_files : 'ZeGrapher.ico',
        include_directories : include_directories('.'),
    )
endif
