fs = import('fs')

# The app shares two files of the site, and content/ holds one of each per
# language: the documentation panel, which is the manual of the app, and the
# words that ask for a donation. The resource file is generated from that
# folder, so a new language needs no build edit. The listing happens at
# configure time: run meson again after you add or remove a language or a
# picture.
content = meson.current_source_dir() / 'content'

# the number in the name of a panel is its place in the page of the website,
# which the app has no use for. 'documentation.md' is what documentation.cpp reads
website_manual = '4-documentation.md'
app_manual = 'documentation.md'

# appdata/generate-metainfo reads this panel of every language, and writes the
# <description> of the metainfo file out of it
features_panel = '2-features.md'

# a language that does not translate this one falls back to English, the way
# build-website.py falls back for a panel
donation = 'donation.md'

langs = run_command(
  'sh', '-c', 'cd "$0" && ls -1 */' + website_manual + ' | cut -d/ -f1', content,
  check: true,
).stdout().strip().split('\n')

files_entries = []
foreach lang: langs
  files_entries += ('<file alias="' + lang / app_manual + '">'
                    + content / lang / website_manual + '</file>')

  donation_lang = fs.is_file(content / lang / donation) ? lang : 'en'
  files_entries += ('<file alias="' + lang / donation + '">'
                    + content / donation_lang / donation + '</file>')

  # the pictures of English, with the pictures of the language over them, the
  # way build-website.py builds a page: a language that translates some
  # pictures only still shows every picture
  pictures = run_command(
    'sh', '-c',
    'cd "$0" && { ls -1 en/images; ls -1 "$1"/images 2>/dev/null; } | sort -u |' +
    ' while read -r name; do' +
    '   if [ -f "$1/images/$name" ]; then echo "$1/images/$name";' +
    '   else echo "en/images/$name"; fi;' +
    ' done',
    content, lang,
    check: true,
  ).stdout().strip().split('\n')

  foreach picture: pictures
    files_entries += ('<file alias="' + lang / 'images' / fs.name(picture) + '">'
                      + content / picture + '</file>')
  endforeach
endforeach

website_qrc = configure_file(
  input: 'website.qrc.in',
  output: 'website.qrc',
  configuration: {'FILES': '\n'.join(files_entries)},
)

# The app puts a button under the words that ask for a donation, and it opens
# the page that build.yaml names for the footer of the site. One address, one
# place that holds it.
donate_url = run_command(
  'sed', '-n', 's/^[[:space:]]*donate:[[:space:]]*//p',
  meson.current_source_dir() / 'build-config' / 'build.yaml',
  check: true,
).stdout().strip().split('\n')[0]

if donate_url == ''
  error('website/build-config/build.yaml names no \'donate\' under \'footer: links\'')
endif

add_project_arguments('-D DONATE_URL="' + donate_url + '"', language: 'cpp')

# what appdata/generate-metainfo reads, so that the metainfo file is written
# again when one of these files changes
website_features = []
foreach lang: langs
  if fs.is_file(content / lang / features_panel)
    website_features += files('content' / lang / features_panel)
  endif
endforeach

website_build_conf = files('build-config' / 'build.yaml')
