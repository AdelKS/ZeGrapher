inc = include_directories('../src')

test_sources = files(
  'function_bench.cpp',
  'algo_move_elements_right.cpp',
)

fs = import('fs')

foreach test_src: test_sources
    test_exe = executable(
      fs.stem(test_src), # name executable after source file without .cpp extension
      test_src,
      source_files, compiled_ressources, compiled_ui, moc_files,
      dependencies: [qt6_dep, boost_dep, zecalculator_dep],
      include_directories: inc
    )

    test(fs.stem(test_src), test_exe) # register as a test
endforeach
