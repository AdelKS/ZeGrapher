# The manual of the app is the documentation panel of the website, and content/
# holds one per language. scripts/content.py lists the manuals and their
# pictures, with the English file standing in for one that a language leaves
# out. The list is made at configure time, so run meson again after you add or
# remove a language or a picture.
content_py = files('scripts' / 'content.py')

website_qrc = configure_file(
  input: 'website.qrc.in',
  output: 'website.qrc',
  configuration: {
    'FILES': run_command(python, content_py, 'qrc', check: true).stdout().strip(),
  },
)
