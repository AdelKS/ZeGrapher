fs = import('fs')

# The app shares two files with the site, and content/ holds one of each per
# language: the documentation panel, which is the manual of the app, and the
# words that ask for a donation. The resource file below is generated from that
# folder, so a new language needs no edit here. The files are listed at
# configure time, so run meson again after you add or remove a language or a
# picture.
content = meson.current_source_dir() / 'content'

# the number in the name of a panel is its place in the page of the website, and
# the app has no use for it. documentation.cpp reads 'documentation.md'
website_manual = '4-documentation.md'
app_manual = 'documentation.md'

# appdata/generate-metainfo reads this panel of every language, and writes the
# <description> of the metainfo file out of it
features_panel = '2-features.md'

# the short strings of a language: the words of the footer, and the words that
# ask for a donation. The app reads the donation out of it. A language that does
# not translate this file falls back to English, the same way build-website.py
# falls back for a panel
strings = 'strings.yaml'

langs = run_command(
  'sh', '-c', 'cd "$0" && ls -1 */' + website_manual + ' | cut -d/ -f1', content,
  check: true,
).stdout().strip().split('\n')

files_entries = []
foreach lang: langs
  files_entries += ('<file alias="' + lang / app_manual + '">'
                    + content / lang / website_manual + '</file>')

  strings_lang = fs.is_file(content / lang / strings) ? lang : 'en'
  files_entries += ('<file alias="' + lang / strings + '">'
                    + content / strings_lang / strings + '</file>')

  # the English pictures, with the pictures of the language written over them,
  # the same way build-website.py builds a page. A language that translates only
  # some of its pictures still shows every picture
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

# The app puts a button under the words that ask for a donation, and that button
# opens the page build.yaml names for the footer of the site. The address is
# written in one place only.
donate_url = run_command(
  'sed', '-n', 's/^[[:space:]]*donate:[[:space:]]*//p',
  meson.current_source_dir() / 'build-config' / 'build.yaml',
  check: true,
).stdout().strip().split('\n')[0]

if donate_url == ''
  error('website/build-config/build.yaml names no \'donate\' under \'footer: links\'')
endif

add_project_arguments('-D DONATE_URL="' + donate_url + '"', language: 'cpp')

# the files appdata/generate-metainfo reads, listed so that meson writes the
# metainfo file again when one of them changes
website_features = []
foreach lang: langs
  if fs.is_file(content / lang / features_panel)
    website_features += files('content' / lang / features_panel)
  endif
endforeach

website_build_conf = files('build-config' / 'build.yaml')
