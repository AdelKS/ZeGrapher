# Every .ts file of this folder holds the translations of one language. When a
# qsTr() string changes in the sources, refresh each of them:
#
#   lupdate6 src -ts translations/ZeGrapher_<code>.ts -no-obsolete -locations none
#
# Run that from the root of the repository. '-locations none' leaves out the
# file and the line of each string. Those change with every edit of the sources,
# and they tell a translator nothing.

qt6 = import('qt6')
fs = import('fs')

# 'ls' fails when the folder holds no .ts file. A build without translations is
# allowed, and the app then shows English only. meson makes the list at
# configure time, so run meson again after you add or remove a language
ts_files = run_command(
  'sh', '-c', 'cd "$0" && ls -1 *.ts', meson.current_source_dir(),
  check: false,
).stdout().split()

qm_files = qt6.compile_translations(ts_files: ts_files)

qm_entries = []
foreach qm : qm_files
  qm_entries += '<file alias="' + fs.name(qm.full_path()) + '">' + qm.full_path() + '</file>'
endforeach

translations_qrc = configure_file(
  input: qrc_template,
  output: 'translations.qrc',
  configuration: {'PREFIX': '/translations', 'FILES': '\n'.join(qm_entries)},
)
