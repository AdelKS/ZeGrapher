inc = include_directories('../src')

executable('bench',
  'function_bench.cpp',
  source_files, compiled_ressources, compiled_ui, moc_files,
  dependencies: [qt6_dep, boost_dep, zecalculator_dep],
  include_directories: inc)
