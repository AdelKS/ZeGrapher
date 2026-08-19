# Every .ts file of this folder holds the translations of one language. When a
# qsTr() string changes in the sources, refresh each of them:
#
#   lupdate6 src -ts translations/ZeGrapher_<code>.ts -no-obsolete -locations none
#
# Run that from the root of the repository. '-locations none' leaves out the
# file and the line of each string. Those change with every edit of the
# sources, and they say nothing about the translation.

qt6 = import('qt6')
fs = import('fs')

# 'ls' fails when the folder holds no .ts file. A build with no translation is
# legal, and the app then shows English alone
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
  input: 'translations.qrc.in',
  output: 'translations.qrc',
  configuration: {'QM_FILES': '\n'.join(qm_entries)},
)
