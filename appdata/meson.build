if host_machine.system() == 'darwin' # macos
    install_data('ZeGrapher.icns', install_dir : 'Contents/Resources')
    install_data('Info.plist', install_dir : 'Contents')
elif host_machine.system() == 'windows'
    # TODO: can we actually properly install stuff on windows ?
else
    # Treat everything else the same
    install_data('ZeGrapher.metainfo.xml', install_dir : get_option('datadir') / 'metainfo')
    # https://www.freedesktop.org/software/appstream/docs/chap-Quickstart.html

    zg_source_icons = [
        'ZeGrapher_16.png',
        'ZeGrapher_32.png',
        'ZeGrapher_64.png',
        'ZeGrapher_128.png',
        'ZeGrapher_256.png',
        'ZeGrapher.svg'
    ]
    zg_dest_icons = [
        '16x16/apps/ZeGrapher.png',
        '32x32/apps/ZeGrapher.png',
        '64x64/apps/ZeGrapher.png',
        '128x128/apps/ZeGrapher.png',
        '256x256/apps/ZeGrapher.png',
        'scalable/apps/ZeGrapher.svg'
    ]
    install_data(sources : zg_source_icons, install_dir : get_option('datadir') / 'icons/hicolor', rename : zg_dest_icons)
    # https://specifications.freedesktop.org/icon-theme-spec/latest/ar01s07.html

    install_data('ZeGrapher.desktop', install_dir : get_option('datadir') / 'applications')
    # https://specifications.freedesktop.org/desktop-entry-spec/latest/ar01s02.html
endif
