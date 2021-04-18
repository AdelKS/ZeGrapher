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

CONFIG += c++17
QMAKE_LFLAGS_RELEASE = -s

OBJECTS_DIR = .obj
MOC_DIR = .moc
UI_DIR = .ui

win32 {
# Modify this line in Windows to boost's folder to be able to compile
    INCLUDEPATH += C:/boost_1_72_0
}

INCLUDEPATH += .

SOURCES += \
    GraphDraw/basegraphdraw.cpp \
    GraphDraw/gridcalculator.cpp \
    GraphDraw/mainview.cpp \
    GraphDraw/mathobjectdraw.cpp \
    GraphDraw/subgraph.cpp \
    GraphDraw/viewmapper.cpp \
    GraphDraw/mainviewcontainer.cpp \
    Widgets/appsettings.cpp \
    Widgets/estheticsettings.cpp \
    Widgets/export.cpp \
    Widgets/graphsizeadjusments.cpp \
    Widgets/popupwidget.cpp \
    Widgets/sheetzoom.cpp \
    main.cpp \
    information.cpp \
    Windows/about.cpp \
    Widgets/tangentwidget.cpp \
    Widgets/straightlinewidget.cpp \
    Widgets/seqwidget.cpp \
    Widgets/qcolorbutton.cpp \
    Widgets/pareqwidget.cpp \
    Widgets/pareqcontroller.cpp \
    Widgets/parconfwidget.cpp \
    Widgets/keyboard.cpp \
    Widgets/integrationwidget.cpp \
    Widgets/funcwidget.cpp \
    Widgets/abstractfuncwidget.cpp \
    ValuesTable/valuestableconf.cpp \
    ValuesTable/valuestable.cpp \
    ValuesTable/seqtable.cpp \
    ValuesTable/pareqtable.cpp \
    ValuesTable/functable.cpp \
    ValuesTable/abstracttable.cpp \
    DataPlot/rowselectorwidget.cpp \
    DataPlot/rowactionswidget.cpp \
    DataPlot/datawindow.cpp \
    DataPlot/datatable.cpp \
    DataPlot/columnselectorwidget.cpp \
    DataPlot/columnactionswidget.cpp \
    Calculus/treecreator.cpp \
    Calculus/seqcalculator.cpp \
    Calculus/funcvaluessaver.cpp \
    Calculus/funccalculator.cpp \
    Calculus/exprcalculator.cpp \
    Calculus/colorsaver.cpp \
    Widgets/datawidget.cpp \
    DataPlot/csvhandler.cpp \
    Calculus/polynomial.cpp \
    Calculus/polynomialregression.cpp \
    Calculus/regression.cpp \
    Calculus/regressionvaluessaver.cpp \
    DataPlot/modelchoicewidget.cpp \
    DataPlot/modelwidget.cpp \
    DataPlot/polynomialmodelwidget.cpp \
    Widgets/numberlineedit.cpp \
    Widgets/rangeadjustments.cpp \
    Widgets/mathobjectsinput.cpp \
    Windows/settings.cpp \
    Windows/values.cpp \
    Windows/updatecheck.cpp \
    Windows/mainwindow.cpp \
    Widgets/expressionlineedit.cpp \
    Widgets/axissettingswidget.cpp

HEADERS  += \
    GraphDraw/basegraphdraw.h \
    GraphDraw/gridcalculator.h \
    GraphDraw/mainview.h \
    GraphDraw/mathobjectdraw.h \
    GraphDraw/subgraph.h \
    GraphDraw/viewmapper.h \
    GraphDraw/mainviewcontainer.h \
    Widgets/appsettings.h \
    Widgets/estheticsettings.h \
    Widgets/export.h \
    Widgets/graphsizeadjusments.h \
    Widgets/popupwidget.h \
    Widgets/sheetzoom.h \
    information.h \
    Windows/about.h \
    Widgets/tangentwidget.h \
    Widgets/straightlinewidget.h \
    Widgets/seqwidget.h \
    Widgets/qcolorbutton.h \
    Widgets/pareqwidget.h \
    Widgets/pareqcontroller.h \
    Widgets/parconfwidget.h \
    Widgets/keyboard.h \
    Widgets/integrationwidget.h \
    Widgets/funcwidget.h \
    Widgets/abstractfuncwidget.h \
    ValuesTable/valuestableconf.h \
    ValuesTable/valuestable.h \
    ValuesTable/seqtable.h \
    ValuesTable/pareqtable.h \
    ValuesTable/functable.h \
    ValuesTable/abstracttable.h \
    DataPlot/rowselectorwidget.h \
    DataPlot/rowactionswidget.h \
    DataPlot/datawindow.h \
    DataPlot/datatable.h \
    DataPlot/columnselectorwidget.h \
    DataPlot/columnactionswidget.h \
    Calculus/treecreator.h \
    Calculus/seqcolorssaver.h \
    Calculus/seqcalculator.h \
    Calculus/funcvaluessaver.h \
    Calculus/funccalculator.h \
    Calculus/exprcalculator.h \
    Calculus/colorsaver.h \
    Calculus/calculusdefines.h \
    Widgets/datawidget.h \
    DataPlot/csvhandler.h \
    Calculus/polynomial.h \
    Calculus/polynomialregression.h \
    Calculus/regression.h \
    Calculus/regressionvaluessaver.h \
    DataPlot/modelchoicewidget.h \
    DataPlot/modelwidget.h \
    DataPlot/polynomialmodelwidget.h \
    Widgets/numberlineedit.h \
    Widgets/rangeadjustments.h \
    Widgets/mathobjectsinput.h \
    Windows/settings.h \
    Windows/values.h \
    Windows/updatecheck.h \
    Windows/mainwindow.h \
    structures.h \
    Widgets/expressionlineedit.h \
    Widgets/axissettingswidget.h


FORMS    += \
    Widgets/appsettings.ui \
    Widgets/estheticsettings.ui \
    Widgets/export.ui \
    Widgets/graphsizeadjusments.ui \
    Widgets/sheetzoom.ui \
    Windows/about.ui \
    Widgets/datawidget.ui \
    DataPlot/startingactions.ui \
    DataPlot/sortoptions.ui \
    DataPlot/filloptions.ui \
    DataPlot/datawindow.ui \
    DataPlot/confirmdelete.ui \
    DataPlot/csvconfig.ui \
    DataPlot/modelchoicewidget.ui \
    DataPlot/polynomialmodelwidget.ui \
    Widgets/keyboard.ui \
    Widgets/mathobjectsinput.ui \
    Windows/settings.ui \
    Widgets/axissettingswidget.ui


RESOURCES += \
    ressources.qrc

RC_ICONS = ZeGrapher.ico
ICON = ZeGrapher.icns

OTHER_FILES +=

isEmpty(APPDATADIR):APPDATADIR=$${PREFIX}/share/metainfo
isEmpty(APPDIR):APPDIR=$${PREFIX}/share/applications
isEmpty(DSRDIR):DSRDIR=$${PREFIX}/share/$${TARGET}
isEmpty(ICONDIR):ICONDIR=$${PREFIX}/share/icons/hicolor/128x128/apps
isEmpty(BINDIR):BINDIR=$${PREFIX}/bin

appdata.path = $$INSTROOT$$APPDATADIR
desktop.path = $$INSTROOT$$APPDIR
icon.path = $$INSTROOT$$ICONDIR
target.path = $$INSTROOT$$BINDIR
translations.path = $$INSTROOT$$DSRDIR/locale

desktop.files = ZeGrapher.desktop
appdata.files = appdata/ZeGrapher.appdata.xml
icon.files = icons/ZeGrapher.png
translations.files = ZeGrapher_fr.qm

INSTALLS += appdata desktop icon target translations

DISTFILES += \
    TODO.md \
    appdata/ZeGrapher.appdata.xml \
    appdata/screenshots/zegrapher-all-windows.png \
    appdata/screenshots/zegrapher-plot-dialogue.png \
    appdata/screenshots/zegrapher-plot-iso.png \
    appdata/screenshots/zegrapher-plot.png \
    appdata/screenshots/zegrapher-save.png
