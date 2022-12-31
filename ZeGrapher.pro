#-------------------------------------------------
#
# Project created by QtCreator 2011-03-25T16:51:48
#
#-------------------------------------------------


QT += widgets network svg

TARGET = ZeGrapher
TEMPLATE = app
TRANSLATIONS = ZeGrapher_fr.ts \
               ZeGrapher_de.ts \
               ZeGrapher_zh.ts

CONFIG += c++2a
QMAKE_LFLAGS_RELEASE = -s

QMAKE_CFLAGS += -Wall -Wextra
QMAKE_CXXFLAGS += -Wall -Wextra

DEFINES *= QT_USE_QSTRINGBUILDER

OBJECTS_DIR = .obj
MOC_DIR = .moc
UI_DIR = .ui

win32 {
# Modify this line in Windows to boost's folder to be able to compile
    INCLUDEPATH += C:/boost_1_72_0
}

INCLUDEPATH += src/

SOURCES += \
    src/GraphDraw/basegraphdraw.cpp \
    src/GraphDraw/gridcalculator.cpp \
    src/GraphDraw/mainview.cpp \
    src/GraphDraw/mathobjectdraw.cpp \
    src/GraphDraw/subgraph.cpp \
    src/GraphDraw/viewmapper.cpp \
    src/GraphDraw/mainviewcontainer.cpp \
    src/Widgets/appsettings.cpp \
    src/Widgets/export.cpp \
    src/Widgets/graphsettings.cpp \
    src/Widgets/graphsizeadjusments.cpp \
    src/Widgets/popupwidget.cpp \
    src/Widgets/sheetzoom.cpp \
    src/main.cpp \
    src/information.cpp \
    src/Windows/about.cpp \
    src/Widgets/tangentwidget.cpp \
    src/Widgets/straightlinewidget.cpp \
    src/Widgets/seqwidget.cpp \
    src/Widgets/qcolorbutton.cpp \
    src/Widgets/pareqwidget.cpp \
    src/Widgets/pareqcontroller.cpp \
    src/Widgets/parconfwidget.cpp \
    src/Widgets/keyboard.cpp \
    src/Widgets/integrationwidget.cpp \
    src/Widgets/funcwidget.cpp \
    src/Widgets/abstractfuncwidget.cpp \
    src/ValuesTable/valuestableconf.cpp \
    src/ValuesTable/valuestable.cpp \
    src/ValuesTable/seqtable.cpp \
    src/ValuesTable/pareqtable.cpp \
    src/ValuesTable/functable.cpp \
    src/ValuesTable/abstracttable.cpp \
    src/DataPlot/rowselectorwidget.cpp \
    src/DataPlot/rowactionswidget.cpp \
    src/DataPlot/datawindow.cpp \
    src/DataPlot/datatable.cpp \
    src/DataPlot/columnselectorwidget.cpp \
    src/DataPlot/columnactionswidget.cpp \
    src/Calculus/treecreator.cpp \
    src/Calculus/seqcalculator.cpp \
    src/Calculus/funcvaluessaver.cpp \
    src/Calculus/funccalculator.cpp \
    src/Calculus/exprcalculator.cpp \
    src/Calculus/colorsaver.cpp \
    src/Widgets/datawidget.cpp \
    src/DataPlot/csvhandler.cpp \
    src/Calculus/polynomial.cpp \
    src/Calculus/polynomialregression.cpp \
    src/Calculus/regression.cpp \
    src/Calculus/regressionvaluessaver.cpp \
    src/DataPlot/modelchoicewidget.cpp \
    src/DataPlot/modelwidget.cpp \
    src/DataPlot/polynomialmodelwidget.cpp \
    src/Widgets/numberlineedit.cpp \
    src/Widgets/rangeadjustments.cpp \
    src/Widgets/mathobjectsinput.cpp \
    src/Windows/settings.cpp \
    src/Windows/values.cpp \
    src/Windows/updatecheck.cpp \
    src/Windows/mainwindow.cpp \
    src/Widgets/expressionlineedit.cpp \
    src/Widgets/axissettingswidget.cpp \
    src/structures.cpp

HEADERS  += \
    src/GraphDraw/basegraphdraw.h \
    src/GraphDraw/gridcalculator.h \
    src/GraphDraw/mainview.h \
    src/GraphDraw/mathobjectdraw.h \
    src/GraphDraw/subgraph.h \
    src/GraphDraw/viewmapper.h \
    src/GraphDraw/mainviewcontainer.h \
    src/Utils/loader.hpp \
    src/Widgets/appsettings.h \
    src/Widgets/export.h \
    src/Widgets/graphsettings.h \
    src/Widgets/graphsizeadjusments.h \
    src/Widgets/popupwidget.h \
    src/Widgets/sheetzoom.h \
    src/information.h \
    src/Windows/about.h \
    src/Widgets/tangentwidget.h \
    src/Widgets/straightlinewidget.h \
    src/Widgets/seqwidget.h \
    src/Widgets/qcolorbutton.h \
    src/Widgets/pareqwidget.h \
    src/Widgets/pareqcontroller.h \
    src/Widgets/parconfwidget.h \
    src/Widgets/keyboard.h \
    src/Widgets/integrationwidget.h \
    src/Widgets/funcwidget.h \
    src/Widgets/abstractfuncwidget.h \
    src/ValuesTable/valuestableconf.h \
    src/ValuesTable/valuestable.h \
    src/ValuesTable/seqtable.h \
    src/ValuesTable/pareqtable.h \
    src/ValuesTable/functable.h \
    src/ValuesTable/abstracttable.h \
    src/DataPlot/rowselectorwidget.h \
    src/DataPlot/rowactionswidget.h \
    src/DataPlot/datawindow.h \
    src/DataPlot/datatable.h \
    src/DataPlot/columnselectorwidget.h \
    src/DataPlot/columnactionswidget.h \
    src/Calculus/treecreator.h \
    src/Calculus/seqcolorssaver.h \
    src/Calculus/seqcalculator.h \
    src/Calculus/funcvaluessaver.h \
    src/Calculus/funccalculator.h \
    src/Calculus/exprcalculator.h \
    src/Calculus/colorsaver.h \
    src/Calculus/calculusdefines.h \
    src/Widgets/datawidget.h \
    src/DataPlot/csvhandler.h \
    src/Calculus/polynomial.h \
    src/Calculus/polynomialregression.h \
    src/Calculus/regression.h \
    src/Calculus/regressionvaluessaver.h \
    src/DataPlot/modelchoicewidget.h \
    src/DataPlot/modelwidget.h \
    src/DataPlot/polynomialmodelwidget.h \
    src/Widgets/numberlineedit.h \
    src/Widgets/rangeadjustments.h \
    src/Widgets/mathobjectsinput.h \
    src/Windows/settings.h \
    src/Windows/values.h \
    src/Windows/updatecheck.h \
    src/Windows/mainwindow.h \
    src/structures.h \
    src/Widgets/expressionlineedit.h \
    src/Widgets/axissettingswidget.h


FORMS += \
    src/Widgets/appsettings.ui \
    src/Widgets/export.ui \
    src/Widgets/graphsettings.ui \
    src/Widgets/graphsizeadjusments.ui \
    src/Widgets/sheetzoom.ui \
    src/Windows/about.ui \
    src/Widgets/datawidget.ui \
    src/DataPlot/startingactions.ui \
    src/DataPlot/sortoptions.ui \
    src/DataPlot/filloptions.ui \
    src/DataPlot/datawindow.ui \
    src/DataPlot/confirmdelete.ui \
    src/DataPlot/csvconfig.ui \
    src/DataPlot/modelchoicewidget.ui \
    src/DataPlot/polynomialmodelwidget.ui \
    src/Widgets/keyboard.ui \
    src/Widgets/mathobjectsinput.ui \
    src/Windows/settings.ui \
    src/Widgets/axissettingswidget.ui


RESOURCES += \
    ressources.qrc
