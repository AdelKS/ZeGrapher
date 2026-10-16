inc = include_directories('../src')

if get_option('test')
  qt6_test_dep = dependency('qt6', modules: ['Test'])

  tests = [
    'algo_move_element',
    'algo_move_elements_right',
    'algo_erase',
    'binomial_coefficient',
    'versionparser',
    'releasenotes',
    'documents',
    'sampler',
    'datatablemodel',
    'csv',
    'gridcalculator',
  ]
  test_args = {
    'documents': [meson.project_source_root() / 'website/scripts/make-images/documents'],
  }

  foreach name: tests
    test_exe = executable(name, name + '.cpp',
      link_with: zegrapher_lib,
      include_directories: [inc, buildconfig_inc] + qmlcpp_inc_dirs,
      dependencies: [qt6_dep, qt6_test_dep, zecalculator_dep, glaze_dep])

    test(name, test_exe, args: test_args.get(name, []))
  endforeach
endif
