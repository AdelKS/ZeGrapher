inc = include_directories('../src')

test_sources = files(
  'function_bench.cpp',
  'algo_move_elements_right.cpp',
  'algo_erase.cpp'
)

fs = import('fs')

foreach test_src: test_sources
    test_exe = executable(
      fs.stem(test_src), # name executable after source file without .cpp extension
      test_src,
      link_with: zg_static_lib,
      include_directories: inc,
      dependencies: [qt6_dep, boost_dep, zecalculator_dep]
    )

    test(fs.stem(test_src), test_exe) # register as a test
endforeach
