inc = include_directories('../src')

test_sources = files(
  'algo_move_elements_right.cpp',
  'algo_erase.cpp',
  'binomial_coefficient.cpp',
  'versionparser.cpp'
)

fs = import('fs')

if get_option('test')
  foreach test_src: test_sources
      test_exe = executable(
        fs.stem(test_src), # name executable after source file without .cpp extension
        test_src,
        include_directories: inc,
        dependencies: [qt6_dep, zecalculator_dep]
      )

      test(fs.stem(test_src), test_exe) # register as a test
  endforeach
endif
