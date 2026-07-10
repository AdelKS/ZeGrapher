if host_machine.system() == 'darwin' # macos
    install_data(app_icon_icns, install_dir : 'Contents/Resources')
    install_data('Info.plist', install_dir : 'Contents')
elif host_machine.system() == 'windows'
    # TODO: can we actually properly install stuff on windows ?
else
    # Treat everything else the same
    install_data('ZeGrapher.metainfo.xml', install_dir : get_option('datadir') / 'metainfo')
    # https://www.freedesktop.org/software/appstream/docs/chap-Quickstart.html

    zg_dest_icons = []
    foreach size : app_icon_sizes
        zg_dest_icons += '@0@x@0@/apps/ZeGrapher.png'.format(size)
    endforeach
    zg_dest_icons += 'scalable/apps/ZeGrapher.svg'

    install_data(sources : app_icon_pngs + app_icon_svg,
                 install_dir : get_option('datadir') / 'icons/hicolor',
                 rename : zg_dest_icons)
    # https://specifications.freedesktop.org/icon-theme-spec/latest/ar01s07.html

    install_data('ZeGrapher.desktop', install_dir : get_option('datadir') / 'applications')
    # https://specifications.freedesktop.org/desktop-entry-spec/latest/ar01s02.html

    install_data('ZeGrapher.mime.xml', install_dir : get_option('datadir') / 'mime/packages', rename : 'ZeGrapher.xml')
    # https://specifications.freedesktop.org/shared-mime-info-spec/latest/
endif
