/****************************************************************************
**  Copyright (c) 2013, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.0.
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
#include <QDebug>

MainWindow::MainWindow() :
baseName("mainwindow_geometry/"),
settings("AdelKS & Co.", "Zegrapher")
{      
    information = new Informations();

    fenOptions = new WindowOptions(information);
    fenBoundarys = new WindowBoundary(information);
    fenFunctions = new WindowFunctions(information);      
    fenAPropos = new about();
    fenImage = new ImageSave(information);
    fenValeurs = new WindowValuesTable(information);
    fenPrint = new Print(information);
    keyboard = new Keyboard();

    scene = new MainGraph(information); // it has to be the last thing to create.


    setWindowIcon(QIcon(":/icons/logoLogiciel.png"));
    setMinimumSize(700,450);
    setWindowTitle("ZeGrapher");

    createMenus();

    window.Xmax = window.Ymax = 10;
    window.Xmin = window.Ymin = -10;
    window.Xscale = window.Yscale = 1;

    setCentralWidget(scene);    

    fenImage->setSize(scene->width(), scene->height());

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
    QMenu *menuFichier = menuBar()->addMenu(tr("File"));
    QMenu *menuOutils = menuBar()->addMenu(tr("Tools"));
    QMenu *menuAffichage = menuBar()->addMenu(tr("Windows"));
    QMenu *menuAide = menuBar()->addMenu("?");   

    boutonGrille = menuOutils->addAction(QIcon(":/icons/quadrillage.png"), tr("Show/Hide the grid"));
    boutonGrille->setCheckable(true);

    QAction *actionRepereOrthonorme = menuOutils->addAction(tr("Orthonormal basis"));
    actionRepereOrthonorme->setCheckable(true);
    connect(actionRepereOrthonorme, SIGNAL(triggered(bool)), information, SLOT(setOrthonormal(bool)));
    connect(information, SIGNAL(newOrthonormalityState(bool)), actionRepereOrthonorme, SLOT(setChecked(bool)));

    QAction *resetView = menuOutils->addAction(QIcon(":/icons/resetToDefaultView.png"), tr("Reset to default view"));
    connect(resetView, SIGNAL(triggered()), fenBoundarys, SLOT(resetToStandardView()));

    QAction *afficherFenAPropos = menuAide->addAction(tr("About..."));
    connect(afficherFenAPropos, SIGNAL(triggered()), this, SLOT(showAboutWin()));

    QAction *print = menuFichier->addAction(QIcon(":/icons/print.png"), tr("Print..."));
    print->setShortcut(QKeySequence("Ctrl+P"));
    connect(print, SIGNAL(triggered()), this, SLOT(showPrintWin()));

    QAction *saveImage = menuFichier->addAction(QIcon(":/icons/enregistrerImage.png"), tr("Save image..."));
    saveImage->setShortcut(QKeySequence("Ctrl+S"));
    connect(saveImage, SIGNAL(triggered()), this, SLOT(showImageSaveWin()));

    QAction *actionAfficherFenOptions = menuFichier->addAction(tr("Options"));
    actionAfficherFenOptions->setShortcut(QKeySequence("Ctrl+O"));
    connect(actionAfficherFenOptions, SIGNAL(triggered()), this, SLOT(showOptionsWin()));

    menuFichier->addSeparator();

    QAction *actionQuitter = menuFichier->addAction(QIcon(":/icons/quitter.png"), tr("Quit"));
    actionQuitter->setShortcut(QKeySequence("Ctrl+Q"));
    connect(actionQuitter, SIGNAL(triggered()), this, SLOT(close()));

    QAction *actionAfficherFenFunctions = menuAffichage->addAction(QIcon(":/icons/fonctions.png"), tr("Functions"));
    actionAfficherFenFunctions->setShortcut(QKeySequence("Ctrl+F"));
    connect(actionAfficherFenFunctions, SIGNAL(triggered()), this, SLOT(showFuncsWin()));

    QAction *actionAfficherFenBoundarys = menuAffichage->addAction(QIcon(":/icons/bornes.png"), tr("Boundarys"));
    actionAfficherFenBoundarys->setShortcut(QKeySequence("Ctrl+B"));
    connect(actionAfficherFenBoundarys, SIGNAL(triggered()), this, SLOT(showRangeWin()));

    QAction *tabValeurs = menuAffichage->addAction(QIcon(":/icons/tableauDeValeurs.png"), tr("Values table"));
    tabValeurs->setShortcut(QKeySequence("Ctrl+Tab"));
    connect(tabValeurs, SIGNAL(triggered()), this, SLOT(showValuesTabWin()));

    QAction *dispKeyboard = menuAffichage->addAction(QIcon(":/icons/keyboard.png"), tr("numeric keyboard"));
    dispKeyboard->setShortcut(QKeySequence("Ctrl+K"));
    connect(dispKeyboard, SIGNAL(triggered()), this, SLOT(showKeyboard()));

    QToolBar *toolBar = new QToolBar(tr("Windows and actions"));
    addToolBar(Qt::LeftToolBarArea, toolBar);

    toolBar->addAction(boutonGrille);
    toolBar->addAction(resetView);
    toolBar->addSeparator();
    toolBar->addAction(actionAfficherFenFunctions);    
    toolBar->addAction(actionAfficherFenBoundarys);  
    toolBar->addAction(tabValeurs);    
    toolBar->addAction(saveImage);    
    toolBar->addAction(print);  
    toolBar->addAction(dispKeyboard);

    statusBar();

    actionAfficherFenBoundarys->setStatusTip(tr("Show the window which allows to modify the shown data boundaries: Xmin, Xmax..."));
    actionAfficherFenFunctions->setStatusTip(tr("Show the window where you can type functions to trace."));
    actionQuitter->setStatusTip(tr("Quit ZeGrapher."));
    actionAfficherFenOptions->setStatusTip(tr("Affiche la fenêtre où l'on peut modifier les options de représentation."));//TODO translation continues here
    tabValeurs->setStatusTip(tr("Affiche une fenêtre contenant le tableau de valeurs pour chaque fonction."));
    resetView->setStatusTip(tr("Rétablit la vue par défaut: Xmin = Ymin = -10 , Xmax = Ymax = 10"));
    boutonGrille->setStatusTip(tr("Afficher/Cacher le quadrillage."));
    saveImage->setStatusTip(tr("Enregistrer le graphique dans une image."));
    dispKeyboard->setStatusTip(tr("Affiche un clavier numérique virtuel."));
    print->setStatusTip(tr("Imprimer ou exporter le graphique vers fichier PDF."));
}


void MainWindow::makeConnects()
{   
    connect(boutonGrille, SIGNAL(triggered(bool)), information, SLOT(setGridState(bool)));
    connect(scene, SIGNAL(sizeChanged(int,int)), fenImage, SLOT(setSize(int,int)));
    connect(fenFunctions, SIGNAL(displayKeyboard()), keyboard, SLOT(show()));
}

void MainWindow::showFuncsWin()
{
    fenFunctions->move(this->pos() - QPoint(fenFunctions->width(),0));
    fenFunctions->show();    
    fenFunctions->activateWindow();
}

void MainWindow::showRangeWin()
{
    fenBoundarys->move(this->pos() + QPoint(this->width() + 20,0));
    fenBoundarys->show();
    fenBoundarys->activateWindow();
}

void MainWindow::showOptionsWin()
{
    fenOptions->move(this->pos() - QPoint(fenOptions->width(),0));
    fenOptions->show();
    fenOptions->activateWindow();
}

void MainWindow::showValuesTabWin()
{
    fenValeurs->move(this->pos());
    fenValeurs->show();
    fenValeurs->activateWindow();
}

void MainWindow::showKeyboard()
{
    keyboard->move(fenFunctions->pos() + QPoint(0,fenFunctions->height()));
    keyboard->show();
}

void MainWindow::showPrintWin()
{
    fenPrint->move(this->pos());
    fenPrint->show();
    fenPrint->activateWindow();
}

void MainWindow::showImageSaveWin()
{
    fenImage->move(this->pos());
    fenImage->show();
    fenImage->activateWindow();
}

void MainWindow::showAboutWin()
{
    fenAPropos->move(this->pos() + QPoint(width()/2, height()/2));
    fenAPropos->show();
    fenAPropos->activateWindow();
}

void MainWindow::closeEvent(QCloseEvent *evenement)
{
    fenBoundarys->close();

    fenFunctions->closeAllOpenedWindows();
    fenFunctions->close();

    fenOptions->close();
    fenAPropos->close();
    fenValeurs->close();
    fenImage->close();
    fenPrint->close();
    keyboard->close();

    evenement->accept();
}

MainWindow::~MainWindow()
{   
    delete fenValeurs;
    delete scene;   
    delete fenBoundarys;
    delete fenFunctions;
    delete fenOptions;
    delete fenAPropos;
    delete fenImage;
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

