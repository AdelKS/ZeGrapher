# appdata/generate-metainfo writes the metainfo file that a software centre
# reads. It builds that file from ZeGrapher.metainfo.xml.in, release-notes.md
# and the panels of the site. The repository holds none of its output.

# appdata/releases.py reads release-notes.md and writes its headings to a YAML
# file, which the app embeds and WhatsNew reads. meson writes the file again
# when release-notes.md changes
release_notes_yaml = configure_file(
    input : 'release-notes.md',
    output : 'release-notes.yaml',
    command : [python, files('releases.py'), meson.project_source_root(),
               '--yaml', '@OUTPUT@'],
)

release_notes_qrc = configure_file(
    input : 'release-notes.qrc.in',
    output : 'release-notes.qrc',
    configuration : {'NOTES' : release_notes_yaml.full_path()},
)

if host_machine.system() == 'darwin' # macos
    install_data(app_icon_icns, install_dir : 'Contents/Resources')

    # '4.0.0_rc0-dev' -> release '4.0.0', major '4', minor '0'. Apple takes only
    # digits and dots in CFBundleShortVersionString, so the keys that hold a
    # number carry the release that a pre-release leads to
    zg_release = meson.project_version().split('_')[0].split('-')[0]
    zg_release_parts = zg_release.split('.')

    install_data(
        configure_file(
            input : 'Info.plist.in',
            output : 'Info.plist',
            configuration : {
                'VERSION' : meson.project_version(),
                'RELEASE' : zg_release,
                'MAJOR' : zg_release_parts[0],
                'MINOR' : zg_release_parts[1],
            },
        ),
        install_dir : 'Contents',
    )
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

    # a software centre reads this file, and macOS and Windows have no such
    # centre. The installed file must be named after the <id> it holds.
    # generate-metainfo writes the files it read to the depfile
    custom_target('metainfo',
                  output : 'com.zegrapher.ZeGrapher.metainfo.xml',
                  command : [python, files('generate-metainfo'),
                             meson.project_source_root(), '--output', '@OUTPUT@',
                             '--depfile', '@DEPFILE@'],
                  depfile : 'com.zegrapher.ZeGrapher.metainfo.xml.d',
                  build_by_default : true,
                  install : true,
                  install_dir : get_option('datadir') / 'metainfo')
    # https://www.freedesktop.org/software/appstream/docs/sect-Metadata-Application.html
endif
