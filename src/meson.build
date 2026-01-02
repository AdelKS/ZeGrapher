inc = include_directories('.')

source_files = files(
  './Calculus/polynomial.cpp',
  './Calculus/polynomialregression.cpp',
  './Calculus/regression.cpp',
  './Calculus/regressionvaluessaver.cpp',
  './Calculus/sampler.cpp',
  './DataPlot/csvpreviewmodel.cpp',
  './DataPlot/datatablemodel.cpp',
  './GraphDraw/axismapper.cpp',
  './GraphDraw/graph.cpp',
  './GraphDraw/gridcalculator.cpp',
  './GraphDraw/interactivegraph.cpp',
  './GraphDraw/mathobjectdraw.cpp',
  './GraphDraw/subgraph.cpp',
  './GraphDraw/viewmapper.cpp',
  './information.cpp',
  './MathObjects/BuildingBlocks/zcmathobjectbb.cpp',
  './MathObjects/constant.cpp',
  './MathObjects/data.cpp',
  './MathObjects/equation.cpp',
  './MathObjects/expr.cpp',
  './MathObjects/mathobject.cpp',
  './MathObjects/mathworld.cpp',
  './MathObjects/namedref.cpp',
  './MathObjects/parametric.cpp',
  './structures.cpp',
  './Utils/graphrange.cpp',
  './Utils/highlighter.cpp',
  './Utils/plotstyle.cpp',
  './Utils/state.cpp',
  './Utils/updatecheck.cpp',
  './Utils/zc-utils.cpp',
)

moc_header_files = files(
  './Calculus/polynomialregression.h',
  './Calculus/regression.h',
  './Calculus/regressionvaluessaver.h',
  './DataPlot/csvpreviewmodel.h',
  './DataPlot/datatablemodel.h',
  './GraphDraw/graph.h',
  './GraphDraw/gridcalculator.h',
  './GraphDraw/interactivegraph.h',
  './GraphDraw/subgraph.h',
  './information.h',
  './MathObjects/BuildingBlocks/stateful.h',
  './MathObjects/constant.h',
  './MathObjects/data.h',
  './MathObjects/equation.h',
  './MathObjects/expr.h',
  './MathObjects/mathobject.h',
  './MathObjects/mathworld.h',
  './MathObjects/namedref.h',
  './MathObjects/parametric.h',
  './Utils/graphrange.h',
  './Utils/highlighter.h',
  './Utils/plotstyle.h',
  './Utils/updatecheck.h',
)

qml_files = files(
  './QML/AppSettings.qml',
  './QML/ColorButton.qml',
  './QML/ConstantEdit.qml',
  './QML/CsvImportSettings.qml',
  './QML/DataEdit.qml',
  './QML/DataPane.qml',
  './QML/DataTable.qml',
  './QML/EquationEdit.qml',
  './QML/IconButton.qml',
  './QML/IconRoundButton.qml',
  './QML/ImageTumbler.qml',
  './QML/InteractiveGraphView.qml',
  './QML/LineEdit.qml',
  './QML/MainWindow.qml',
  './QML/MathObjectInput.qml',
  './QML/MathObjects.qml',
  './QML/NumberEdit.qml',
  './QML/ObjectStyle.qml',
  './QML/ParametricEdit.qml',
  './QML/RangeAdjust.qml',
  './QML/TextTumbler.qml',
  './QML/UserInputPanel.qml',
  './QML/ValueEdit.qml',
  './QML/ZcExprEdit.qml',
  './QML/ZeLabel.qml',
  './QML/ZeTextEdit.qml',
)

qmlcpp_files = files(
  './DataPlot/csvpreviewmodel.h',
  './DataPlot/datatablemodel.h',
  './globalvars.h',
  './GraphDraw/interactivegraph.h',
  './information.h',
  './MathObjects/BuildingBlocks/stateful.h',
  './MathObjects/constant.h',
  './MathObjects/data.h',
  './MathObjects/equation.h',
  './MathObjects/expr.h',
  './MathObjects/mathobject.h',
  './MathObjects/mathworld.h',
  './MathObjects/namedref.h',
  './MathObjects/parametric.h',
  './structures.h',
  './Utils/graphrange.h',
  './Utils/highlighter.h',
  './Utils/plotstyle.h',
  './Utils/state.h',
  './Utils/updatecheck.h',
)

fs = import('fs')

qmlcpp_inc_dirs = []
foreach qmlcpp_file: qmlcpp_files
  qmlcpp_inc_dirs += [include_directories(fs.relative_to(fs.parent(qmlcpp_file), 'src'))]
endforeach

qt6 = import('qt6')

compiled_ressources = qt6.compile_resources(sources: files('../ressources.qrc'))
moc_files = qt6.compile_moc(
  headers : moc_header_files,
  include_directories: inc,
  dependencies: qt6_dep
)

zg_static_lib = static_library('ZeGrapherLib',
  source_files, compiled_ressources, moc_files,
  include_directories: inc,
  dependencies: [qt6_dep, boost_dep, zecalculator_dep])

qml_mods = qt6.qml_module(
  'ZeGrapher',
  qml_sources: qml_files,
  moc_headers: qmlcpp_files,
  include_directories: qmlcpp_inc_dirs,
  dependencies: [qt6_dep],
)

executable('ZeGrapher',
  sources: ['main.cpp', compiled_ressources, qml_mods],
  link_with: zg_static_lib,
  include_directories: [inc] + qmlcpp_inc_dirs,
  dependencies: [qt6_dep, boost_dep, zecalculator_dep],
  install: true)
