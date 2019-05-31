/****************************************************************************
**  Copyright (c) 2019, Adel Kara Slimane <adel.ks@zegrapher.com>
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

/* TODO:
 * - Update grid switches
 * - Fix what's not working
 * */

MainWindow::MainWindow()
{
    setWindowIcon(QIcon(":/icons/software.png"));   
    setWindowTitle("ZeGrapher");

    information = new Information();

    inputWin = new MathObjectsInput(information, this);
    // settingsWin should be instanced after inputWin, because needs information->getFuncsList(), updated by inputWin
    settingsWin = new Settings(information, this);

    aboutWin = new about(this);
    updateCheckWin = new UpdateCheck(this);
    valuesWin = new Values(information, this);
    exportWin = new Export(information, this);
    keyboard = new Keyboard();

    setIconSize(QSize(30, 30));

    mainGraph = new MainGraph(information); // it has to be the last thing to create.
    setCentralWidget(mainGraph);

    createDocks();
    createMenus();

    makeConnects();

    loadWindowSavedGeomtries();

    if(settingsWin->checkForUpdatesOnStart())
        updateCheckWin->silentCheckForUpdate();
}

void MainWindow::createDocks()
{
    mathInputDock = new QDockWidget(tr("Math objects input"), this);
    mathInputDock->setWidget(inputWin);
    addDockWidget(Qt::LeftDockWidgetArea, mathInputDock);

    settingsWinDock = new QDockWidget(tr("Settings"), this);
    settingsWinDock->setWidget(settingsWin);
    addDockWidget(Qt::LeftDockWidgetArea, settingsWinDock);
    tabifyDockWidget(mathInputDock, settingsWinDock);
    settingsWinDock->close();
}

void MainWindow::createMenus()
{
    QMenu *menuFile = menuBar()->addMenu(tr("File"));
    QMenu *menuTools = menuBar()->addMenu(tr("Tools"));
    QMenu *menuWindows = menuBar()->addMenu(tr("Windows"));
    QMenu *menuHelp = menuBar()->addMenu("?");

    gridButton = menuTools->addAction(QIcon(":/icons/grid.png"), tr("Show/Hide the grid"));

    QAction *setOrthonormalAction = menuTools->addAction(QIcon(":/icons/orthonormalView.svg"), tr("Toggle orthonormal view"));
    setOrthonormalAction->setCheckable(true);
    connect(setOrthonormalAction, SIGNAL(triggered(bool)), information, SLOT(setOrthonormal(bool)));
    connect(information, SIGNAL(newOrthonormalityState(bool)), setOrthonormalAction, SLOT(setChecked(bool))); 

    QAction *showAboutWinAction = menuHelp->addAction(tr("About..."));
    connect(showAboutWinAction, SIGNAL(triggered()), aboutWin, SLOT(exec()));

    QAction *showAboutQtWinAction = menuHelp->addAction(tr("About Qt"));
    connect(showAboutQtWinAction, SIGNAL(triggered()), this, SLOT(showAboutQtWin()));

    QAction *showUpdateCheckWinAction = menuHelp->addAction(tr("Check for updates"));
    connect(showUpdateCheckWinAction, SIGNAL(triggered()), updateCheckWin, SLOT(checkForUpdate()));

    QAction *showExportWinAction = menuFile->addAction(QIcon(":/icons/export.svg"), tr("Export graph..."));
    showExportWinAction->setShortcut(QKeySequence("Ctrl+S"));
    connect(showExportWinAction, SIGNAL(triggered()), exportWin, SLOT(show()));
    connect(showExportWinAction, SIGNAL(triggered()), exportWin, SLOT(raise()));

    QAction *showSettingsWinAction = menuFile->addAction(QIcon(":/icons/settings.png"), tr("Settings"));
    showSettingsWinAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(showSettingsWinAction, SIGNAL(triggered()), settingsWinDock, SLOT(show()));
    connect(showSettingsWinAction, SIGNAL(triggered()), settingsWinDock, SLOT(raise()));

    menuFile->addSeparator();

    QAction *exitAction = menuFile->addAction(QIcon(":/icons/quitter.png"), tr("Exit"));
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    QAction *showInputWinAction = menuWindows->addAction(QIcon(":/icons/functions.png"), tr("Functions"));
    showInputWinAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(showInputWinAction, SIGNAL(triggered()), mathInputDock, SLOT(show()));
    connect(showInputWinAction, SIGNAL(triggered()), mathInputDock, SLOT(raise()));   

    QAction *showValuesWinAction = menuWindows->addAction(QIcon(":/icons/valuesTable.svg"), tr("Values table"));
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
    toolBar->addAction(setOrthonormalAction);
    toolBar->addAction(resetViewAction);
    toolBar->addSeparator();
    toolBar->addAction(showInputWinAction);
    toolBar->addAction(showValuesWinAction);
    toolBar->addAction(showExportWinAction);
    toolBar->addAction(showSettingsWinAction);
    toolBar->addAction(showKeyboardAction);

    statusBar();

    showInputWinAction->setStatusTip(tr("Enter functions, sequences, parametric equations, data..."));
    exitAction->setStatusTip(tr("Exit ZeGrapher."));
    showSettingsWinAction->setStatusTip(tr("Edit axes' color, background color, curve's quality..."));
    showValuesWinAction->setStatusTip(tr("Display the values taken by functions, sequences and parametric equations on tables."));
    resetViewAction->setStatusTip(tr("Reset to default view"));

    gridButton->setStatusTip(tr("Cycle trough Grid possibilities."));

    showKeyboardAction->setStatusTip(tr("Virtual keyboard."));
    showExportWinAction->setStatusTip(tr("Export graph or print."));
}

void MainWindow::loadWindowSavedGeomtries()
{
    /* Restore geometries */
    if(settings.contains("main_window/geometry"))
        setGeometry(settings.value("main_window/geometry").value<QRect>());
    if(settings.contains("settings_window/geometry"))
        settingsWin->setGeometry(settings.value("settings_window/geometry").value<QRect>());   
    if(settings.contains("input_window/geometry"))
        inputWin->setGeometry(settings.value("input_window/geometry").value<QRect>());
    if(settings.contains("values_window/geometry"))
        valuesWin->setGeometry(settings.value("values_window/geometry").value<QRect>());
    if(settings.contains("export_window/geometry"))
        exportWin->setGeometry(settings.value("export_window/geometry").value<QRect>());
    if(settings.contains("virtual_keyboard_window/geometry"))
        keyboard->setGeometry(settings.value("virtual_keyboard_window/geometry").value<QRect>());
}

void MainWindow::saveWindowsGeometry()
{
    settings.setValue("main_window/geometry", geometry());

    if(! settingsWin->geometry().isNull())
        settings.setValue("settings_window/geometry", settingsWin->geometry());    
    if(! inputWin->geometry().isNull())
        settings.setValue("input_window/geometry", inputWin->geometry());
    if(! valuesWin->geometry().isNull())
        settings.setValue("values_window/geometry", valuesWin->geometry());
    if(! exportWin->geometry().isNull())
        settings.setValue("export_window/geometry", exportWin->geometry());
    if(! keyboard->geometry().isNull())
        settings.setValue("virtual_keyboard_window/geometry", keyboard->geometry());
}


void MainWindow::makeConnects()
{
    connect(gridButton, SIGNAL(triggered(bool)), information, SLOT(setGridState(bool)));
    connect(inputWin, SIGNAL(displayKeyboard()), keyboard, SLOT(show()));
}

void MainWindow::updateGridButtonIcon()
{
    ZeGraphSettings graphSettings = information->getGraphSettings();

    if(graphSettings.gridSettings.gridType == ZeGridType::NO_GRID)
        gridButton->setIcon(QIcon(":/icons/no_grid.png"));
    else if(graphSettings.gridSettings.gridType == ZeGridType::GRID)
        gridButton->setIcon(QIcon(":/icons/grid.png"));
    else gridButton->setIcon(QIcon(":/icons/grid_subgrid.png"));
}

void MainWindow::showAboutQtWin()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::closeEvent(QCloseEvent *evenement)
{
    /* Save windows geometry */
    saveWindowsGeometry();
    settingsWin->saveSettings();
    keyboard->close();
    evenement->accept();
}

MainWindow::~MainWindow()
{
    delete keyboard;
}


