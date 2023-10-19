project('ZeGrapher', 'cpp',
    version : '4.0.0',
    default_options : [
        'cpp_std=c++20',
        'buildtype=release',
        'optimization=3',
        'default_library=static',
        'prefer_static=true',
        'c_args=-pipe',
        'cpp_args=-pipe',
        'warning_level=3',
    ]
)

qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets', 'Network', 'Svg', 'Quick', 'Qml', 'QuickWidgets'])
boost_dep = dependency('boost')
zecalculator_dep = dependency('zecalculator', default_options: ['default_library=static'])

subdir('src')
subdir('appdata')
subdir('test')
