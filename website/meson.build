# The app shares two files with the site, and content/ holds one of each per
# language: the documentation panel, which is the app manual, and the donation
# appeal. scripts/content.py lists them and the manual's pictures, falling back
# to the English file when a language lacks one. The list is made at configure
# time, so rerun meson after adding or removing a language or picture.
content_py = files('scripts' / 'content.py')

website_qrc = configure_file(
  input: qrc_template,
  output: 'website.qrc',
  configuration: {
    'PREFIX': '/website',
    'FILES': run_command(python, content_py, 'qrc', check: true).stdout().strip(),
  },
)

# BuildInfo gives QML the site, GitHub repository, contact address and
# donation page from build.yaml, so no other file hardcodes them. The C++ code
# reads them and the app version from buildconfig.h, which meson regenerates
# when build.yaml or the version changes
configure_file(
  input: 'build-config' / 'build.yaml',
  output: 'buildconfig.h',
  command: [python, content_py, 'header', meson.project_version()],
  capture: true,
)
buildconfig_inc = include_directories('.')
