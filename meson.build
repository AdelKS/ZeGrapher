project('ZeGrapher', 'cpp',
    version : '4.0.0',
    default_options : [
        'cpp_std=c++20',
        'buildtype=release',
        'optimization=3',
        'b_ndebug=if-release',
        'c_args=-pipe',
        'cpp_args=-pipe',
        'warning_level=3',
    ],
    meson_version: '>=1.7'
)

if get_option('debug')
    add_project_arguments('-D QT_QML_DEBUG', language: 'cpp')
endif

if get_option('loglevel') != 'debug'
    add_project_arguments('-D QT_NO_DEBUG_OUTPUT', language : 'cpp')
endif

if not ['debug', 'info'].contains(get_option('loglevel'))
    add_project_arguments('-D QT_NO_INFO_OUTPUT', language : 'cpp')
endif

if get_option('loglevel') == 'off'
    add_project_arguments('-D QT_NO_WARNING_OUTPUT', language : 'cpp')
endif

if (get_option('b_ndebug') == 'if-release' and get_option('buildtype') == 'release') or get_option('b_ndebug') == 'true'
    add_project_arguments('-D QT_NO_DEBUG', language : 'cpp')
endif

qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets', 'Network', 'Svg', 'Quick', 'Qml', 'QuickWidgets'])
boost_dep = dependency('boost')
zecalculator_dep = dependency('zecalculator')

subdir('src')
subdir('appdata')
subdir('test')
