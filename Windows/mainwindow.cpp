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
    information = new Information();    

    aboutWin = new About(this);
    updateCheckWin = new UpdateCheck(this);
    valuesWin = new Values(information, this);
    keyboard = new Keyboard();    
    settingsWin = new Settings(information, this);

    setIconSize(QSize(30, 30));

    // MainView has to be the last class to be instanced
    mainGraph = new MainView(information);
    setCentralWidget(mainGraph);


    setWindowIcon(QIcon(":/icons/ZeGrapher.png"));
    setMinimumSize(700,450);
    setWindowTitle("ZeGrapher");

    // Create dock widget

    inputDock = new QDockWidget(tr("User settings"), this);
    inputDock->setWidget(settingsWin);
    addDockWidget(Qt::LeftDockWidgetArea, inputDock);


    createMenus();

    loadWindowSavedGeomtries();

    if(settingsWin->checkForUpdatesOnStart())
        updateCheckWin->silentCheckForUpdate();
}

void MainWindow::createMenus()
{
    QMenu *menuFile = menuBar()->addMenu(tr("File"));
    QMenu *menuTools = menuBar()->addMenu(tr("Tools"));
    QMenu *menuWindows = menuBar()->addMenu(tr("Windows"));
    QMenu *menuHelp = menuBar()->addMenu("?");

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
    connect(showExportWinAction, &QAction::triggered, settingsWin, &Settings::showExportSettings);

    QAction *showSettingsWinAction = menuFile->addAction(QIcon(":/icons/settings.png"), tr("Settings"));
    showSettingsWinAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(showSettingsWinAction, SIGNAL(triggered()), inputDock, SLOT(show()));
    connect(showSettingsWinAction, SIGNAL(triggered()), inputDock, SLOT(raise()));
    // TODO: add slot in settingsWin so it displays the settings tab

    menuFile->addSeparator();

    QAction *exitAction = menuFile->addAction(QIcon(":/icons/quitter.png"), tr("Exit"));
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    QAction *showInputWinAction = menuWindows->addAction(QIcon(":/icons/functions.png"), tr("Functions"));
    showInputWinAction->setShortcut(QKeySequence("Ctrl+F"));
    // TODO: add slot in settingsWin so it displays the input tab

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

    toolBar->addAction(setOrthonormalAction);
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
    if(settings.contains("values_window/geometry"))
        valuesWin->setGeometry(settings.value("values_window/geometry").value<QRect>());
    if(settings.contains("virtual_keyboard_window/geometry"))
        keyboard->setGeometry(settings.value("virtual_keyboard_window/geometry").value<QRect>());
}

void MainWindow::saveWindowsGeometry()
{
    settings.setValue("main_window/geometry", geometry());

    if(! settingsWin->geometry().isNull())
        settings.setValue("settings_window/geometry", settingsWin->geometry());
    if(! valuesWin->geometry().isNull())
        settings.setValue("values_window/geometry", valuesWin->geometry());
    if(! keyboard->geometry().isNull())
        settings.setValue("virtual_keyboard_window/geometry", keyboard->geometry());
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


