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

# meson makes the list at configure time, so run meson again after you add or
# remove a language. A build without translations shows English only
ts_files = run_command(
  python, '-c',
  'import pathlib, sys; print(*sorted(p.name for p in pathlib.Path(sys.argv[1]).glob("*.ts")))',
  meson.current_source_dir(),
  check: true,
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
