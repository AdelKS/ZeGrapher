# The app shares two files with the site, and content/ holds one of each per
# language: the documentation panel, which is the manual of the app, and the
# words that ask for a donation. scripts/content.py lists them and the pictures
# of the manual, with the English file standing in for one that a language
# leaves out. The list is made at configure time, so run meson again after you
# add or remove a language or a picture.
content_py = files('scripts' / 'content.py')

website_qrc = configure_file(
  input: qrc_template,
  output: 'website.qrc',
  configuration: {
    'PREFIX': '/website',
    'FILES': run_command(python, content_py, 'qrc', check: true).stdout().strip(),
  },
)

# BuildInfo gives QML the GitHub repository that build.yaml names, so no other
# file writes the address
repository_url = run_command(python, content_py, 'repository', check: true).stdout().strip()

add_project_arguments('-D REPOSITORY_URL="' + repository_url + '"', language: 'cpp')

# The app puts a button under the words that ask for a donation, and that button
# opens the page build.yaml names for the footer of the site. The address is
# written in one place only.
donate_url = run_command(python, content_py, 'donate-url', check: true).stdout().strip()

add_project_arguments('-D DONATE_URL="' + donate_url + '"', language: 'cpp')
