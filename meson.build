project('ZeGrapher', 'cpp',
    version : '4.0.0',
    default_options : [
        'cpp_std=c++20',
        'buildtype=release',
        'optimization=3',
        'default_library=static',
        'prefer_static=true',
        'b_ndebug=if-release',
        'c_args=-pipe',
        'cpp_args=-pipe',
        'warning_level=3',
    ]
)

if not get_option('debug_logs')
    add_project_arguments('-D QT_NO_DEBUG_OUTPUT', language : 'cpp')
    add_project_arguments('-D QT_NO_INFO_OUTPUT', language : 'cpp')
    add_project_arguments('-D QT_NO_WARNING_OUTPUT', language : 'cpp')
endif

if (get_option('b_ndebug') == 'if-release' and get_option('buildtype') == 'release') or get_option('b_ndebug') == 'true'
    add_project_arguments('-D QT_NO_DEBUG', language : 'cpp')
endif

qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets', 'Network', 'Svg', 'Quick', 'Qml', 'QuickWidgets'])
boost_dep = dependency('boost')
zecalculator_dep = dependency('zecalculator', default_options: ['default_library=static'])

subdir('src')
subdir('appdata')
subdir('test')
