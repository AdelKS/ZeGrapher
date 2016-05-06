/****************************************************************************
**  Copyright (c) 2016, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/


#include "Windows/mainwindow.h"

MainWindow::MainWindow()
{      
    information = new Information();

    settingsWin = new Settings(information);
    rangeWin = new RangeAdjustments(information);
    inputWin = new MathObjectsInput(information);
    aboutWin = new about(this);
    updateCheckWin = new UpdateCheck(this);
    imageExportWin = new ImageSave(information);
    valuesWin = new Values(information);
    printWin = new Print(information);
    keyboard = new Keyboard();

    scene = new MainGraph(information); // it has to be the last thing to create.


    setWindowIcon(QIcon(":/icons/software.png"));
    setMinimumSize(700,450);
    setWindowTitle("ZeGrapher");

    createMenus();

    window.Xmax = window.Ymax = 10;
    window.Xmin = window.Ymin = -10;
    window.Xscale = window.Yscale = 1;

    setCentralWidget(scene);    

    imageExportWin->setSize(scene->width(), scene->height());

    makeConnects();

    loadWindowSavedGeomtries();

    if(information->getSettingsVals().updateCheckAtStart)
        updateCheckWin->silentCheckForUpdate();
}

void MainWindow::createMenus()
{
    QMenu *menuFile = menuBar()->addMenu(tr("File"));
    QMenu *menuTools = menuBar()->addMenu(tr("Tools"));
    QMenu *menuWindows = menuBar()->addMenu(tr("Windows"));
    QMenu *menuHelp = menuBar()->addMenu("?");   

    gridButton = menuTools->addAction(QIcon(":/icons/grid.png"), tr("Show/Hide the grid"));
    gridButton->setCheckable(true);

    QAction *setOrthonormalBasisAction = menuTools->addAction(tr("Toggle orthonormal view"));
    setOrthonormalBasisAction->setCheckable(true);
    connect(setOrthonormalBasisAction, SIGNAL(triggered(bool)), information, SLOT(setOrthonormal(bool)));
    connect(information, SIGNAL(newOrthonormalityState(bool)), setOrthonormalBasisAction, SLOT(setChecked(bool)));

    QAction *resetViewAction = menuTools->addAction(QIcon(":/icons/resetToDefaultView.png"), tr("Reset to default view"));
    connect(resetViewAction, SIGNAL(triggered()), rangeWin, SLOT(resetToStandardView()));

    QAction *showAboutWinAction = menuHelp->addAction(tr("About..."));
    connect(showAboutWinAction, SIGNAL(triggered()), aboutWin, SLOT(exec()));
    
    QAction *showAboutQtWinAction = menuHelp->addAction(tr("About Qt"));
    connect(showAboutQtWinAction, SIGNAL(triggered()), this, SLOT(showAboutQtWin()));

    QAction *showUpdateCheckWinAction = menuHelp->addAction(tr("Check for updates"));
    connect(showUpdateCheckWinAction, SIGNAL(triggered()), updateCheckWin, SLOT(checkForUpdate()));

    QAction *showPrintWinAction = menuFile->addAction(QIcon(":/icons/print.png"), tr("Print..."));
    showPrintWinAction->setShortcut(QKeySequence("Ctrl+P"));
    connect(showPrintWinAction, SIGNAL(triggered()), printWin, SLOT(show()));    
    connect(showPrintWinAction, SIGNAL(triggered()), printWin, SLOT(raise()));

    QAction *showImageExportWinAction = menuFile->addAction(QIcon(":/icons/saveImage.png"), tr("Image export..."));
    showImageExportWinAction->setShortcut(QKeySequence("Ctrl+S"));
    connect(showImageExportWinAction, SIGNAL(triggered()), imageExportWin, SLOT(show()));
    connect(showImageExportWinAction, SIGNAL(triggered()), imageExportWin, SLOT(raise()));

    QAction *showSettingsWinAction = menuFile->addAction(QIcon(":/icons/settings.png"), tr("Settings"));
    showSettingsWinAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(showSettingsWinAction, SIGNAL(triggered()), settingsWin, SLOT(show()));
    connect(showSettingsWinAction, SIGNAL(triggered()), settingsWin, SLOT(raise()));

    menuFile->addSeparator();

    QAction *exitAction = menuFile->addAction(QIcon(":/icons/quitter.png"), tr("Exit"));
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    QAction *showInputWinAction = menuWindows->addAction(QIcon(":/icons/functions.png"), tr("Functions"));
    showInputWinAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(showInputWinAction, SIGNAL(triggered()), inputWin, SLOT(show()));
    connect(showInputWinAction, SIGNAL(triggered()), inputWin, SLOT(raise()));

    QAction *showRangeWinAction = menuWindows->addAction(QIcon(":/icons/boundaries.png"), tr("Range edit"));
    showRangeWinAction->setShortcut(QKeySequence("Ctrl+B"));
    connect(showRangeWinAction, SIGNAL(triggered()), rangeWin, SLOT(show()));    
    connect(showRangeWinAction, SIGNAL(triggered()), rangeWin, SLOT(raise()));

    QAction *showValuesWinAction = menuWindows->addAction(QIcon(":/icons/valuesTable.png"), tr("Values table"));
    showValuesWinAction->setShortcut(QKeySequence("Ctrl+Tab"));
    connect(showValuesWinAction, SIGNAL(triggered()), valuesWin, SLOT(show()));    
    connect(showValuesWinAction, SIGNAL(triggered()), valuesWin, SLOT(raise()));

    QAction *showKeyboardAction = menuWindows->addAction(QIcon(":/icons/keyboard.png"), tr("numeric keyboard"));
    showKeyboardAction->setShortcut(QKeySequence("Ctrl+K"));
    connect(showKeyboardAction, SIGNAL(triggered()), keyboard, SLOT(show()));    
    connect(showKeyboardAction, SIGNAL(triggered()), keyboard, SLOT(raise()));

    QToolBar *toolBar = new QToolBar(tr("Windows and actions"));
    addToolBar(Qt::LeftToolBarArea, toolBar);

    toolBar->addAction(gridButton);
    toolBar->addAction(resetViewAction);
    toolBar->addSeparator();
    toolBar->addAction(showInputWinAction);
    toolBar->addAction(showRangeWinAction);
    toolBar->addAction(showValuesWinAction);
    toolBar->addAction(showImageExportWinAction);
    toolBar->addAction(showPrintWinAction);
    toolBar->addAction(showKeyboardAction);

    statusBar();

    showRangeWinAction->setStatusTip(tr("Edit the displayed range: Xmin, Xmax..."));
    showInputWinAction->setStatusTip(tr("Enter functions, sequences, parametric equations, data..."));
    exitAction->setStatusTip(tr("Exit ZeGrapher."));
    showSettingsWinAction->setStatusTip(tr("Edit axes' color, background color, curve's quality..."));
    showValuesWinAction->setStatusTip(tr("Display the values taken by functions, sequences and parametric equations on tables."));
    resetViewAction->setStatusTip(tr("Reset to default view"));
    gridButton->setStatusTip(tr("Show/Hide grid"));
    showImageExportWinAction->setStatusTip(tr("Export the graph as an image."));
    showKeyboardAction->setStatusTip(tr("Virtual keyboard."));
    showPrintWinAction->setStatusTip(tr("Print, or export in PDF."));
}

void MainWindow::loadWindowSavedGeomtries()
{
    /* Restore geometries */
    if(settings.contains("main_window/geometry"))
        setGeometry(settings.value("main_window/geometry").value<QRect>());
    if(settings.contains("settings_window/geometry"))
        settingsWin->setGeometry(settings.value("settings_window/geometry").value<QRect>());
    if(settings.contains("range_window/geometry"))
        rangeWin->setGeometry(settings.value("range_window/geometry").value<QRect>());
    if(settings.contains("input_window/geometry"))
        inputWin->setGeometry(settings.value("input_window/geometry").value<QRect>());
    if(settings.contains("image_export_window/geometry"))
        imageExportWin->setGeometry(settings.value("image_export_window/geometry").value<QRect>());
    if(settings.contains("values_window/geometry"))
        valuesWin->setGeometry(settings.value("values_window/geometry").value<QRect>());
    if(settings.contains("print_window/geometry"))
        printWin->setGeometry(settings.value("print_window/geometry").value<QRect>());
    if(settings.contains("virtual_keyboard_window/geometry"))
        keyboard->setGeometry(settings.value("virtual_keyboard_window/geometry").value<QRect>());

}

void MainWindow::saveWindowsGeometry()
{
    settings.setValue("main_window/geometry", geometry());
    settings.setValue("settings_window/geometry", settingsWin->geometry());
    settings.setValue("range_window/geometry", rangeWin->geometry());
    settings.setValue("input_window/geometry", inputWin->geometry());
    settings.setValue("image_export_window/geometry", imageExportWin->geometry());
    settings.setValue("values_window/geometry", valuesWin->geometry());
    settings.setValue("print_window/geometry", printWin->geometry());
    settings.setValue("virtual_keyboard_window/geometry", keyboard->geometry());
}


void MainWindow::makeConnects()
{   
    connect(gridButton, SIGNAL(triggered(bool)), information, SLOT(setGridState(bool)));
    connect(scene, SIGNAL(sizeChanged(int,int)), imageExportWin, SLOT(setSize(int,int)));
    connect(inputWin, SIGNAL(displayKeyboard()), keyboard, SLOT(show()));
}

void MainWindow::showAboutQtWin()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::closeEvent(QCloseEvent *evenement)
{
    rangeWin->close();

    inputWin->closeAllOpenedWindows();
    inputWin->close();

    settingsWin->saveSettings();
    settingsWin->close();

    aboutWin->close();
    valuesWin->close();
    imageExportWin->close();
    printWin->close();
    keyboard->close();

    evenement->accept();
}

MainWindow::~MainWindow()
{   
    /* Save windows geometry */
    saveWindowsGeometry();
    
    delete valuesWin;
    delete scene;   
    delete rangeWin;
    delete inputWin;
    delete settingsWin;
    delete aboutWin;
    delete imageExportWin;
}


