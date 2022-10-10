project('ZeGrapher', 'cpp',
    version : '3.0.1',
    default_options : [
        'cpp_std=c++17',
        'buildtype=release',
        'optimization=3',
        'default_library=shared',
        'c_args=-pipe',
        'cpp_args=-pipe',
        'warning_level=3',
    ]
)

qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets', 'Network', 'Svg'])
boost_dep = dependency('boost')

subdir('src')
subdir('appdata')
