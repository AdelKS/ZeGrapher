# appdata/generate-metainfo writes the metainfo file that a software centre
# reads. The build writes it out of ZeGrapher.metainfo.xml.in, release-notes.md
# and the panels of the site, and the repository holds none of what it writes.

release_notes_qrc = files('release-notes.qrc')

if host_machine.system() == 'darwin' # macos
    install_data(app_icon_icns, install_dir : 'Contents/Resources')
    install_data('Info.plist', install_dir : 'Contents')
elif host_machine.system() == 'windows'
    # TODO: can we actually properly install stuff on windows ?
else
    # Treat everything else the same
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

    # a software centre reads this file, and macOS and Windows have none. The
    # name of the installed file must be the <id> that the file holds
    python = find_program('python3')

    custom_target('metainfo',
                  output : 'com.zegrapher.ZeGrapher.metainfo.xml',
                  command : [python, files('generate-metainfo'),
                             meson.project_source_root(), '--output', '@OUTPUT@'],
                  depend_files : files('ZeGrapher.metainfo.xml.in',
                                       'release-notes.md', 'releases.py')
                                 + website_features + website_build_conf,
                  build_by_default : true,
                  install : true,
                  install_dir : get_option('datadir') / 'metainfo')
    # https://www.freedesktop.org/software/appstream/docs/sect-Metadata-Application.html
endif
