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

    winOptions = new WindowOptions(information);
    winBoundaries = new WindowBoundary(information);
    winFunctions = new WindowFunctions(information);      
    winAbout = new about();
    winImage = new ImageSave(information);
    winValues = new WindowValuesTable(information);
    winPrint = new Print(information);
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

    winImage->setSize(scene->width(), scene->height());

    makeConnects();
    
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

    QAction *actionBasisOrthonorme = menuTools->addAction(tr("Orthonormal basis"));
    actionBasisOrthonorme->setCheckable(true);
    connect(actionBasisOrthonorme, SIGNAL(triggered(bool)), information, SLOT(setOrthonormal(bool)));
    connect(information, SIGNAL(newOrthonormalityState(bool)), actionBasisOrthonorme, SLOT(setChecked(bool)));

    QAction *resetView = menuTools->addAction(QIcon(":/icons/resetToDefaultView.png"), tr("Reset to default view"));
    connect(resetView, SIGNAL(triggered()), winBoundaries, SLOT(resetToStandardView()));

    QAction *afficherwinAbout = menuHelp->addAction(tr("About..."));
    connect(afficherwinAbout, SIGNAL(triggered()), this, SLOT(showAboutWin()));

    QAction *print = menuFile->addAction(QIcon(":/icons/print.png"), tr("Print..."));
    print->setShortcut(QKeySequence("Ctrl+P"));
    connect(print, SIGNAL(triggered()), this, SLOT(showPrintWin()));

    QAction *saveImage = menuFile->addAction(QIcon(":/icons/saveImage.png"), tr("Save image..."));
    saveImage->setShortcut(QKeySequence("Ctrl+S"));
    connect(saveImage, SIGNAL(triggered()), this, SLOT(showImageSaveWin()));

    QAction *actionShowWinOptions = menuFile->addAction(tr("Options"));
    actionShowWinOptions->setShortcut(QKeySequence("Ctrl+O"));
    connect(actionShowWinOptions, SIGNAL(triggered()), this, SLOT(showOptionsWin()));

    menuFile->addSeparator();

    QAction *actionQuitter = menuFile->addAction(QIcon(":/icons/quitter.png"), tr("Quit"));
    actionQuitter->setShortcut(QKeySequence("Ctrl+Q"));
    connect(actionQuitter, SIGNAL(triggered()), this, SLOT(close()));

    QAction *actionShowWinFunctions = menuWindows->addAction(QIcon(":/icons/functions.png"), tr("Functions"));
    actionShowWinFunctions->setShortcut(QKeySequence("Ctrl+F"));
    connect(actionShowWinFunctions, SIGNAL(triggered()), this, SLOT(showFuncsWin()));

    QAction *actionShowWinBoundaries = menuWindows->addAction(QIcon(":/icons/boundaries.png"), tr("Boundaries"));
    actionShowWinBoundaries->setShortcut(QKeySequence("Ctrl+B"));
    connect(actionShowWinBoundaries, SIGNAL(triggered()), this, SLOT(showRangeWin()));

    QAction *tabValues = menuWindows->addAction(QIcon(":/icons/valuesTable.png"), tr("Values table"));
    tabValues->setShortcut(QKeySequence("Ctrl+Tab"));
    connect(tabValues, SIGNAL(triggered()), this, SLOT(showValuesTabWin()));

    QAction *dispKeyboard = menuWindows->addAction(QIcon(":/icons/keyboard.png"), tr("numeric keyboard"));
    dispKeyboard->setShortcut(QKeySequence("Ctrl+K"));
    connect(dispKeyboard, SIGNAL(triggered()), this, SLOT(showKeyboard()));

    QToolBar *toolBar = new QToolBar(tr("Windows and actions"));
    addToolBar(Qt::LeftToolBarArea, toolBar);

    toolBar->addAction(gridButton);
    toolBar->addAction(resetView);
    toolBar->addSeparator();
    toolBar->addAction(actionShowWinFunctions);    
    toolBar->addAction(actionShowWinBoundaries);  
    toolBar->addAction(tabValues);    
    toolBar->addAction(saveImage);    
    toolBar->addAction(print);  
    toolBar->addAction(dispKeyboard);

    statusBar();

    actionShowWinBoundaries->setStatusTip(tr("Show the window which allows to modify the shown data boundaries: Xmin, Xmax..."));
    actionShowWinFunctions->setStatusTip(tr("Show the window where you can type functions to trace."));
    actionQuitter->setStatusTip(tr("Quit ZeGrapher."));
    actionShowWinOptions->setStatusTip(tr("Show settings window."));
    tabValues->setStatusTip(tr("Show settings window"));
    resetView->setStatusTip(tr("Reset to default view"));
    gridButton->setStatusTip(tr("Show/Hide grid"));
    saveImage->setStatusTip(tr("Save the graph on a picture."));
    dispKeyboard->setStatusTip(tr("Show a virtual numerical keyboard."));
    print->setStatusTip(tr("Print or export in PDF."));
}


void MainWindow::makeConnects()
{   
    connect(gridButton, SIGNAL(triggered(bool)), information, SLOT(setGridState(bool)));
    connect(scene, SIGNAL(sizeChanged(int,int)), winImage, SLOT(setSize(int,int)));
    connect(winFunctions, SIGNAL(displayKeyboard()), this, SLOT(showKeyboard()));
}

void MainWindow::showFuncsWin()
{
    winFunctions->move(this->pos() - QPoint(winFunctions->width(),0));
    winFunctions->show();    
    winFunctions->activateWindow();
}

void MainWindow::showRangeWin()
{
    winBoundaries->move(this->pos() + QPoint(this->width() + 20,0));
    winBoundaries->show();
    winBoundaries->activateWindow();
}

void MainWindow::showOptionsWin()
{
    winOptions->move(this->pos() - QPoint(winOptions->width(),0));
    winOptions->show();
    winOptions->activateWindow();
}

void MainWindow::showValuesTabWin()
{
    winValues->move(this->pos());
    winValues->show();
    winValues->activateWindow();
}

void MainWindow::showKeyboard()
{
    keyboard->move(winFunctions->frameGeometry().bottomLeft());
    keyboard->show();
}

void MainWindow::showPrintWin()
{
    winPrint->move(this->pos());
    winPrint->show();
    winPrint->activateWindow();
}

void MainWindow::showImageSaveWin()
{
    winImage->move(this->pos());
    winImage->show();
    winImage->activateWindow();
}

void MainWindow::showAboutWin()
{
    winAbout->move(this->pos() + QPoint(width()/2, height()/2));
    winAbout->show();
    winAbout->activateWindow();
}

void MainWindow::closeEvent(QCloseEvent *evenement)
{
    winBoundaries->close();

    winFunctions->closeAllOpenedWindows();
    winFunctions->close();

    winOptions->saveSettings();
    winOptions->close();

    winAbout->close();
    winValues->close();
    winImage->close();
    winPrint->close();
    keyboard->close();

    evenement->accept();
}

MainWindow::~MainWindow()
{   
    delete winValues;
    delete scene;   
    delete winBoundaries;
    delete winFunctions;
    delete winOptions;
    delete winAbout;
    delete winImage;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    
   settings.setValue(baseName + "h", event->size().height());
   settings.setValue(baseName + "w", event->size().width());
}

void MainWindow::moveEvent(QMoveEvent* event)
{
   settings.setValue(baseName + "x", event->pos().x());
   settings.setValue(baseName + "y", event->pos().y());
}  

