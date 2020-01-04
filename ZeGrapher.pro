#-------------------------------------------------
#
# Project created by QtCreator 2011-03-25T16:51:48
#
#-------------------------------------------------

QT += widgets printsupport network

TARGET = ZeGrapher
TEMPLATE = app
TRANSLATIONS = ZeGrapher_fr.ts
CONFIG += c++11
QMAKE_LFLAGS_RELEASE = -s

OBJECTS_DIR = .obj
MOC_DIR = .moc
UI_DIR = .ui

INCLUDEPATH += .

win32 {
    INCLUDEPATH += C:/boost/boost_1_72_0
}


SOURCES += \
    Widgets/popupwidget.cpp \
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
    GraphDraw/printpreview.cpp \
    GraphDraw/maingraph.cpp \
    GraphDraw/imagepreview.cpp \
    GraphDraw/graphdraw.cpp \
    Export/print.cpp \
    Export/imagesave.cpp \
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
    Windows/rangeadjustments.cpp \
    Windows/mathobjectsinput.cpp \
    Windows/settings.cpp \
    Windows/values.cpp \
    Windows/updatecheck.cpp \
    Windows/mainwindow.cpp

HEADERS  += \
    Widgets/popupwidget.h \
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
    GraphDraw/printpreview.h \
    GraphDraw/maingraph.h \
    GraphDraw/imagepreview.h \
    GraphDraw/graphdraw.h \
    Export/print.h \
    Export/imagesave.h \
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
    Windows/rangeadjustments.h \
    Windows/mathobjectsinput.h \
    Windows/settings.h \
    Windows/values.h \
    Windows/updatecheck.h \
    Windows/mainwindow.h \
    structures.h


FORMS    += \
    Windows/about.ui \
    Widgets/datawidget.ui \
    Export/print.ui \
    Export/imagesave.ui \
    DataPlot/startingactions.ui \
    DataPlot/sortoptions.ui \
    DataPlot/filloptions.ui \
    DataPlot/datawindow.ui \   
    DataPlot/confirmdelete.ui \
    DataPlot/csvconfig.ui \
    DataPlot/modelchoicewidget.ui \
    DataPlot/polynomialmodelwidget.ui \
    Widgets/keyboard.ui \
    Windows/rangeadjustments.ui \
    Windows/mathobjectsinput.ui \
    Windows/settings.ui


RESOURCES += \
    ressources.qrc

RC_ICONS = appicon.ico

OTHER_FILES += \
    TODO.txt

isEmpty(APPDATADIR):APPDATADIR=$${PREFIX}/share/appdata
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
appdata.files =  ZeGrapher.appdata.xml
icon.files = icons/ZeGrapher.png
translations.files = ZeGrapher_fr.qm

INSTALLS += appdata desktop icon target translations

DISTFILES += \
    Help/Rpin.png \
    Help/Tetapin.png \
    Help/Xpin.png \
    Help/Ypin.png \
    Help/basePin.png \
    Help/columnActions_de.png \
    Help/columnActions_en.png \
    Help/columnActions_fr.png \
    Help/columnFill_de.png \
    Help/columnFill_en.png \
    Help/columnFill_fr.png \
    Help/data_help_de.html \
    Help/data_help_de.md \
    Help/data_help_en.html \
    Help/data_help_en.md \
    Help/data_help_fr.html \
    Help/data_help_fr.md \
    Help/english-icon.png \
    Help/french-icon.png \
    Help/lineActions_de.png \
    Help/lineActions_en.png \
    Help/lineActions_fr.png \
    Help/pins.png \
    Help/pins_de.png \
    Help/pins_en.png \
    Help/polynomialCoefVals_en.png \
    Help/polynomialCoefVals_fr.png \
    Help/selector.png
