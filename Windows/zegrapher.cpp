/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane <adel.ks@zegrapher.com>
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



#include "Windows/zegrapher.h"

MainWindow::MainWindow() :
baseName("mainwindow_geometry/")
{      
    information = new Information();

    settingsWin = new Settings(information);
    rangeWin = new RangeAdjustments(information);
    inputWin = new MathObjectsInput(information);
    aboutWin = new about();
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
    
    /* Restore geometry */
    if(settings.allKeys().contains(baseName + "x"))
    {
        setGeometry(
            settings.value(baseName + "x").toInt(),
            settings.value(baseName + "y").toInt(),
            settings.value(baseName + "w").toInt(),
            settings.value(baseName + "h").toInt());
    }
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
    connect(showAboutWinAction, SIGNAL(triggered()), this, SLOT(showAboutWin()));
    
    QAction *showAboutQtWinAction = menuHelp->addAction(tr("About Qt"));
    connect(showAboutQtWinAction, SIGNAL(triggered()), this, SLOT(showAboutQtWin()));

    QAction *showPrintWinAction = menuFile->addAction(QIcon(":/icons/print.png"), tr("Print..."));
    showPrintWinAction->setShortcut(QKeySequence("Ctrl+P"));
    connect(showPrintWinAction, SIGNAL(triggered()), this, SLOT(showPrintWin()));

    QAction *showImageExportWinAction = menuFile->addAction(QIcon(":/icons/saveImage.png"), tr("Image export..."));
    showImageExportWinAction->setShortcut(QKeySequence("Ctrl+S"));
    connect(showImageExportWinAction, SIGNAL(triggered()), this, SLOT(showImageSaveWin()));

    QAction *showSettingsWinAction = menuFile->addAction(QIcon(":/icons/settings.png"), tr("Settings"));
    showSettingsWinAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(showSettingsWinAction, SIGNAL(triggered()), this, SLOT(showSettingsWin()));

    menuFile->addSeparator();

    QAction *exitAction = menuFile->addAction(QIcon(":/icons/quitter.png"), tr("Exit"));
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    QAction *showInputWinAction = menuWindows->addAction(QIcon(":/icons/functions.png"), tr("Functions"));
    showInputWinAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(showInputWinAction, SIGNAL(triggered()), this, SLOT(showFuncsWin()));

    QAction *showRangeWinAction = menuWindows->addAction(QIcon(":/icons/boundaries.png"), tr("Range edit"));
    showRangeWinAction->setShortcut(QKeySequence("Ctrl+B"));
    connect(showRangeWinAction, SIGNAL(triggered()), this, SLOT(showRangeWin()));

    QAction *showValuesWinAction = menuWindows->addAction(QIcon(":/icons/valuesTable.png"), tr("Values table"));
    showValuesWinAction->setShortcut(QKeySequence("Ctrl+Tab"));
    connect(showValuesWinAction, SIGNAL(triggered()), this, SLOT(showValuesWin()));

    QAction *showKeyboardAction = menuWindows->addAction(QIcon(":/icons/keyboard.png"), tr("numeric keyboard"));
    showKeyboardAction->setShortcut(QKeySequence("Ctrl+K"));
    connect(showKeyboardAction, SIGNAL(triggered()), this, SLOT(showKeyboard()));

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


void MainWindow::makeConnects()
{   
    connect(gridButton, SIGNAL(triggered(bool)), information, SLOT(setGridState(bool)));
    connect(scene, SIGNAL(sizeChanged(int,int)), imageExportWin, SLOT(setSize(int,int)));
    connect(inputWin, SIGNAL(displayKeyboard()), this, SLOT(showKeyboard()));
}

void MainWindow::showFuncsWin()
{
    inputWin->move(this->pos() - QPoint(inputWin->width(),0));
    inputWin->show();
    inputWin->activateWindow();
}

void MainWindow::showRangeWin()
{
    rangeWin->move(this->pos() + QPoint(this->width() + 20,0));
    rangeWin->show();
    rangeWin->activateWindow();
}

void MainWindow::showSettingsWin()
{
    settingsWin->move(this->pos() - QPoint(settingsWin->width(),0));
    settingsWin->show();
    settingsWin->activateWindow();
}

void MainWindow::showValuesWin()
{
    valuesWin->move(this->pos());
    valuesWin->show();
    valuesWin->activateWindow();
}

void MainWindow::showKeyboard()
{
    keyboard->move(inputWin->frameGeometry().bottomLeft());
    keyboard->show();
}

void MainWindow::showPrintWin()
{
    printWin->move(this->pos());
    printWin->show();
    printWin->activateWindow();
}

void MainWindow::showImageSaveWin()
{
    imageExportWin->move(this->pos());
    imageExportWin->show();
    imageExportWin->activateWindow();
}

void MainWindow::showAboutWin()
{
    aboutWin->move(this->pos() + QPoint(width()/2, height()/2));
    aboutWin->show();
    aboutWin->activateWindow();
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
    /* Save the window geometry */
    settings.setValue(baseName + "h", size().height());
    settings.setValue(baseName + "w", size().width());
    settings.setValue(baseName + "x", pos().x());
    settings.setValue(baseName + "y", pos().y());
    
    delete valuesWin;
    delete scene;   
    delete rangeWin;
    delete inputWin;
    delete settingsWin;
    delete aboutWin;
    delete imageExportWin;
}


