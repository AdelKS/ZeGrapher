# The manual of the app is the documentation panel of the website, and content/
# holds one per language. scripts/content.py lists the manuals and their
# pictures, with the English file standing in for one that a language leaves
# out. The list is made at configure time, so run meson again after you add or
# remove a language or a picture.
content_py = files('scripts' / 'content.py')

website_qrc = configure_file(
  input: qrc_template,
  output: 'website.qrc',
  configuration: {
    'PREFIX': '/website',
    'FILES': run_command(python, content_py, 'qrc', check: true).stdout().strip(),
  },
)

# BuildInfo gives QML the site, the GitHub repository and the contact address
# that build.yaml names, so no other file writes them. The C++ code reads them
# and the version of the app from buildconfig.h. meson writes that header again
# when build.yaml or the version changes
configure_file(
  input: 'build-config' / 'build.yaml',
  output: 'buildconfig.h',
  command: [python, content_py, 'header', meson.project_version()],
  capture: true,
)
buildconfig_inc = include_directories('.')
