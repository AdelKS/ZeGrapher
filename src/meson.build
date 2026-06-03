inc = include_directories('.')

# the block between BEGIN and END comments bellow is generated with
# `update-meson-sources` script in the same directory

# BEGIN AUTOGEN SRC-LIST

source_files = [
  'Animation/animatedconstant.cpp',
  'Animation/animationconductor.cpp',
  'Calculus/polynomial.cpp',
  'Calculus/polynomialregression.cpp',
  'Calculus/regression.cpp',
  'Calculus/regressionvaluessaver.cpp',
  'Calculus/sampler.cpp',
  'DataPlot/csvpreviewmodel.cpp',
  'DataPlot/datatablemodel.cpp',
  'GraphDraw/axismapper.cpp',
  'GraphDraw/datapoints.cpp',
  'GraphDraw/graph.cpp',
  'GraphDraw/gridcalculator.cpp',
  'GraphDraw/interactivegraph.cpp',
  'GraphDraw/mathobjectdraw.cpp',
  'GraphDraw/subgraph.cpp',
  'GraphDraw/viewmapper.cpp',
  'information.cpp',
  'MathObjects/BuildingBlocks/base.cpp',
  'MathObjects/BuildingBlocks/zcmathobjectbb.cpp',
  'MathObjects/constant.cpp',
  'MathObjects/data.cpp',
  'MathObjects/equation.cpp',
  'MathObjects/expr.cpp',
  'MathObjects/mathobject.cpp',
  'MathObjects/mathworld.cpp',
  'MathObjects/namedref.cpp',
  'MathObjects/parametric.cpp',
  'structures.cpp',
  'Utils/appsettings.cpp',
  'Utils/graphrange.cpp',
  'Utils/graphsettings.cpp',
  'Utils/highlighter.cpp',
  'Utils/plotstyle.cpp',
  'Utils/sampledcurve.cpp',
  'Utils/state.cpp',
  'Utils/themedcolor.cpp',
  'Utils/updatecheck.cpp',
  'Utils/zc-utils.cpp',
]

moc_header_files = [
  'Calculus/polynomialregression.h',
  'Calculus/regression.h',
  'Calculus/regressionvaluessaver.h',
  'GraphDraw/gridcalculator.h',
  'GraphDraw/subgraph.h',
  'Utils/palettewatcher.h',
]

qml_files = [
  'QML/AppSettings.qml',
  'QML/ColorButton.qml',
  'QML/ConstantEdit.qml',
  'QML/CsvImportSettings.qml',
  'QML/CurvesOverlay.qml',
  'QML/DataEdit.qml',
  'QML/DataPane.qml',
  'QML/DataTable.qml',
  'QML/EquationEdit.qml',
  'QML/GraphSettings.qml',
  'QML/GridSettings.qml',
  'QML/IconButton.qml',
  'QML/IconRoundButton.qml',
  'QML/ImageTumbler.qml',
  'QML/InteractiveGraphView.qml',
  'QML/LineEdit.qml',
  'QML/MainWindow.qml',
  'QML/MathObjectInput.qml',
  'QML/MathObjects.qml',
  'QML/NumberEdit.qml',
  'QML/ObjectStyle.qml',
  'QML/ParametricEdit.qml',
  'QML/RangeAdjust.qml',
  'QML/TextTumbler.qml',
  'QML/UserInputPanel.qml',
  'QML/ValueEdit.qml',
  'QML/ZcExprEdit.qml',
  'QML/ZeCheckBox.qml',
  'QML/ZeDoubleSpinBox.qml',
  'QML/ZeLabel.qml',
  'QML/ZeRadioButton.qml',
  'QML/ZeSpinBox.qml',
  'QML/ZeTabButton.qml',
  'QML/ZeTextEdit.qml',
  'QML/ZoomSettings.qml',
]

qmlcpp_files = [
  'Animation/animatedconstant.h',
  'Animation/animationconductor.h',
  'DataPlot/csvpreviewmodel.h',
  'DataPlot/datatablemodel.h',
  'globalvars.h',
  'GraphDraw/datapoints.h',
  'GraphDraw/graph.h',
  'GraphDraw/interactivegraph.h',
  'information.h',
  'MathObjects/BuildingBlocks/base.h',
  'MathObjects/constant.h',
  'MathObjects/data.h',
  'MathObjects/equation.h',
  'MathObjects/expr.h',
  'MathObjects/mathobject.h',
  'MathObjects/mathworld.h',
  'MathObjects/namedref.h',
  'MathObjects/parametric.h',
  'structures.h',
  'Utils/appsettings.h',
  'Utils/axissettings.h',
  'Utils/curvestyle.h',
  'Utils/graphrange.h',
  'Utils/graphsettings.h',
  'Utils/gridsettings.h',
  'Utils/highlighter.h',
  'Utils/plotstyle.h',
  'Utils/state.h',
  'Utils/themedcolor.h',
  'Utils/updatecheck.h',
]

# END AUTOGEN SRC-LIST

fs = import('fs')

qmlcpp_inc_dirs = []
foreach qmlcpp_file: qmlcpp_files
  qmlcpp_inc_dirs += [include_directories(fs.parent(qmlcpp_file))]
endforeach

qt6 = import('qt6')

qt6_modules = [
  'Core',
  'Gui',
  'Widgets',
  'Network',
  'Svg',
  'Quick',
  'Qml',
  'QuickWidgets',
  'QmlIntegration',
  'ShaderTools'
]

qt6_dep = dependency('qt6',
  version: '>=6.10',
  modules: qt6_modules
)

qt6_metatypes_dir = qt6_dep.get_variable(pkgconfig: 'libdir') / 'qt6' / 'metatypes'

qt6_foreign_metatypes = []
foreach qt6_mod: qt6_modules
  metatype_file_path = qt6_metatypes_dir / 'qt6' + qt6_mod.to_lower() + '_metatypes.json'
  if fs.exists(metatype_file_path)
    qt6_foreign_metatypes += [metatype_file_path]
  endif
endforeach

boost_dep = dependency('boost')
zecalculator_dep = dependency('zecalculator')

compiled_ressources = qt6.compile_resources(sources: files('../ressources.qrc'))

# --- Qt RHI shader compilation -----------------------------------------------
# qsb cross-compiles GLSL into Qt's serialized shader format consumed by
# QSGMaterialShader::setShaderFileName(). The .qsb files are bundled into a
# Qt resource so they can be loaded via qrc:// at runtime.
qt_libexec = qt6_dep.get_variable(pkgconfig: 'libexecdir')
qt_bindir  = qt6_dep.get_variable(pkgconfig: 'bindir')

qsb = find_program('qsb-qt6', 'qsb', dirs: [qt_libexec, qt_bindir], required: true)
rcc = find_program('rcc-qt6', 'rcc', dirs: [qt_libexec, qt_bindir], required: true)

shader_sources = [
  'GraphDraw/shaders/markers.vert',
  'GraphDraw/shaders/markers.frag',
]

shader_qsb_files = []
foreach src : shader_sources
  args = ['--glsl', '300es,330,410', '--hlsl', '50', '--msl', '12', '-b']
  name = fs.name(src)
  shader_qsb_files += custom_target(name + '.qsb',
    input: src,
    output: name + '.qsb',
    command: [qsb] + args + ['-o', '@OUTPUT@', '@INPUT@'],
  )
endforeach

shaders_qrc = configure_file(
  input: 'GraphDraw/shaders/shaders.qrc.in',
  output: 'shaders.qrc',
  configuration: {
    'MARKERS_VERT_QSB': shader_qsb_files[0].full_path(),
    'MARKERS_FRAG_QSB': shader_qsb_files[1].full_path(),
  },
)

shaders_resources_cpp = qt6.compile_resources(sources: shaders_qrc)
# -----------------------------------------------------------------------------

moc_files = qt6.compile_moc(
  headers : moc_header_files,
  include_directories: inc,
  dependencies: qt6_dep
)

qml_mods = qt6.qml_module(
  'ZeGrapher',
  qml_sources: qml_files,
  moc_headers: qmlcpp_files,
  include_directories: qmlcpp_inc_dirs,
  dependencies: [qt6_dep],
  qmltyperegistrar_extra_arguments: ['--foreign-types=' + ','.join(qt6_foreign_metatypes)],
)

compiler = meson.get_compiler('cpp')
if compiler.get_id() == 'gcc'
  add_project_arguments('-Wno-sfinae-incomplete', language: 'cpp')
endif

if compiler.get_id() == 'clang'
  add_project_arguments('-Wno-c++26-extensions', language: 'cpp')
endif

executable('ZeGrapher',
  sources: ['main.cpp', source_files, compiled_ressources, shaders_resources_cpp, moc_files, qml_mods],
  include_directories: [inc] + qmlcpp_inc_dirs,
  dependencies: [qt6_dep, boost_dep, zecalculator_dep],
  win_subsystem: get_option('loglevel') == 'off' ? 'windows' : 'console', # useful for Windows: displays the app without spawning
                            # a console alongside it, no-op on other platforms.
  install: true)
