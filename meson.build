project('ZeGrapher', 'cpp',
    version : '4.0.0',
    default_options : [
        'cpp_std=c++20',
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
zecalculator_dep = dependency('zecalculator')

subdir('src')
subdir('appdata')
