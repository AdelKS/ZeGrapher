inc = include_directories('.')

# update-meson-sources, next to this file, writes the block between the BEGIN
# and END comments below

# BEGIN AUTOGEN SRC-LIST

source_files = [
  'Animation/animatedconstant.cpp',
  'Animation/animationconductor.cpp',
  'Calculus/polynomial.cpp',
  'Calculus/polynomialregression.cpp',
  'Calculus/regression.cpp',
  'Calculus/sampler.cpp',
  'DataPlot/csvpreviewmodel.cpp',
  'DataPlot/datatablemodel.cpp',
  'globals.cpp',
  'GraphDraw/graph.cpp',
  'GraphDraw/graphlayout.cpp',
  'GraphDraw/graphpainter.cpp',
  'GraphDraw/gridcalculator.cpp',
  'GraphDraw/interactivegraph.cpp',
  'GraphDraw/linenode.cpp',
  'GraphDraw/markernode.cpp',
  'GraphDraw/viewmapper.cpp',
  'information.cpp',
  'initresources.cpp',
  'MathObjects/BuildingBlocks/base.cpp',
  'MathObjects/BuildingBlocks/zcmathobjectbb.cpp',
  'MathObjects/constant.cpp',
  'MathObjects/data.cpp',
  'MathObjects/datasheet.cpp',
  'MathObjects/equation.cpp',
  'MathObjects/expr.cpp',
  'MathObjects/mathobject.cpp',
  'MathObjects/mathworld.cpp',
  'MathObjects/namedref.cpp',
  'MathObjects/parametric.cpp',
  'Utils/appsettings.cpp',
  'Utils/axissettings.cpp',
  'Utils/documentation.cpp',
  'Utils/donation.cpp',
  'Utils/graphrange.cpp',
  'Utils/graphsettings.cpp',
  'Utils/gridsettings.cpp',
  'Utils/highlighter.cpp',
  'Utils/plotstyle.cpp',
  'Utils/sampledcurve.cpp',
  'Utils/sampling.cpp',
  'Utils/sizesettings.cpp',
  'Utils/state.cpp',
  'Utils/textdocumenttools.cpp',
  'Utils/themedcolor.cpp',
  'Utils/updatecheck.cpp',
  'Utils/zc-utils.cpp',
]

moc_header_files = [
  'Calculus/polynomialregression.h',
  'Calculus/regression.h',
]

qml_files = [
  'QML/About.qml',
  'QML/AppSettings.qml',
  'QML/ButtonTray.qml',
  'QML/CardButton.qml',
  'QML/ColorButton.qml',
  'QML/ConstantEdit.qml',
  'QML/CsvFileDialog.qml',
  'QML/CsvImportSettings.qml',
  'QML/DataColumnEdit.qml',
  'QML/DataPane.qml',
  'QML/DataSheetEdit.qml',
  'QML/DataTable.qml',
  'QML/DocPane.qml',
  'QML/DonationBox.qml',
  'QML/DragHandle.qml',
  'QML/EdgeButton.qml',
  'QML/EquationEdit.qml',
  'QML/Foldable.qml',
  'QML/GraphSettings.qml',
  'QML/GridSettings.qml',
  'QML/IconButton.qml',
  'QML/IconRoundButton.qml',
  'QML/ImageTumbler.qml',
  'QML/LineEdit.qml',
  'QML/LinkLabel.qml',
  'QML/ListViewItem.qml',
  'QML/MainWindow.qml',
  'QML/MathObjectInput.qml',
  'QML/MathObjects.qml',
  'QML/NameEdit.qml',
  'QML/NumberEdit.qml',
  'QML/ObjectCard.qml',
  'QML/ObjectStyle.qml',
  'QML/ParametricEdit.qml',
  'QML/PlotControls.qml',
  'QML/RangeAdjust.qml',
  'QML/ReleaseNotes.qml',
  'QML/ReleaseNotesDialog.qml',
  'QML/ReorderList.qml',
  'QML/Reveal.qml',
  'QML/SamplingEdit.qml',
  'QML/SchrodingerBadge.qml',
  'QML/Separator.qml',
  'QML/TextTumbler.qml',
  'QML/UserInputPanel.qml',
  'QML/ValueEdit.qml',
  'QML/ZcExprEdit.qml',
  'QML/ZeCheckBox.qml',
  'QML/ZeDoubleSpinBox.qml',
  'QML/ZeLabel.qml',
  'QML/ZeRadioButton.qml',
  'QML/ZeSpinBox.qml',
  'QML/ZeTabBar.qml',
  'QML/ZeTabButton.qml',
  'QML/ZeTextEdit.qml',
  'QML/ZoomSettings.qml',
]

qml_singleton_files = [
  'QML/ZeStyle.qml',
]

qmlcpp_files = [
  'Animation/animatedconstant.h',
  'Animation/animationconductor.h',
  'DataPlot/csvpreviewmodel.h',
  'DataPlot/datatablemodel.h',
  'GraphDraw/graph.h',
  'GraphDraw/interactivegraph.h',
  'information.h',
  'MathObjects/BuildingBlocks/base.h',
  'MathObjects/constant.h',
  'MathObjects/data.h',
  'MathObjects/datasheet.h',
  'MathObjects/equation.h',
  'MathObjects/expr.h',
  'MathObjects/mathobject.h',
  'MathObjects/mathworld.h',
  'MathObjects/namedref.h',
  'MathObjects/parametric.h',
  'Utils/appsettings.h',
  'Utils/axissettings.h',
  'Utils/buildinfo.h',
  'Utils/documentation.h',
  'Utils/donation.h',
  'Utils/graphrange.h',
  'Utils/graphsettings.h',
  'Utils/gridsettings.h',
  'Utils/highlighter.h',
  'Utils/plotstyle.h',
  'Utils/sampling.h',
  'Utils/settinglimits.h',
  'Utils/sizesettings.h',
  'Utils/state.h',
  'Utils/textdocumenttools.h',
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
  'Network',
  'Svg',
  'Quick',
  'Qml',
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

compiled_resources = qt6.compile_resources(
  sources: files('../resources.qrc') + translations_qrc + website_qrc + release_notes_qrc
)

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
  'GraphDraw/shaders/lines.vert',
  'GraphDraw/shaders/lines.frag',
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
    'LINES_VERT_QSB': shader_qsb_files[2].full_path(),
    'LINES_FRAG_QSB': shader_qsb_files[3].full_path(),
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
  qml_singletons: qml_singleton_files,
  # the C++ types have properties of QtQuick types, such as QFont and
  # QQuickTextDocument
  depends_imports: ['QtQuick'],
  moc_headers: qmlcpp_files,
  include_directories: qmlcpp_inc_dirs + [buildconfig_inc],
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

zecalculator_dep = dependency('zecalculator', version: '>=0.13.6')
glaze_dep = dependency('glaze', version: '>=8.3')

# the app without main(), which the tests link too
zegrapher_lib = static_library('zegrapher',
  sources: [source_files, compiled_resources, shaders_resources_cpp, moc_files, qml_mods],
  include_directories: [inc, buildconfig_inc] + qmlcpp_inc_dirs,
  dependencies: [qt6_dep, zecalculator_dep, glaze_dep])

executable('ZeGrapher',
  sources: ['main.cpp', app_icon_windows],
  link_with: zegrapher_lib,
  include_directories: [inc, buildconfig_inc] + qmlcpp_inc_dirs,
  dependencies: [qt6_dep, zecalculator_dep, glaze_dep],
  win_subsystem: get_option('loglevel') == 'off' ? 'windows' : 'console', # useful for Windows: displays the app without spawning
                            # a console alongside it, no-op on other platforms.
  install: true)
