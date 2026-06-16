project('glaze', 'cpp',
    version : run_command('version.sh', 'get-vcs', check: true).stdout().strip(),
    default_options : [
        'cpp_std=c++23'
    ],
)

meson.add_dist_script('version.sh', 'set-dist', meson.project_version())

inc = include_directories('include')

glaze_dep = declare_dependency(include_directories : inc)

meson.override_dependency('glaze', glaze_dep)
