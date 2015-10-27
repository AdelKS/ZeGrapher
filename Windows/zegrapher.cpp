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


MainWindow::MainWindow()
{      
    informations = new Informations();

    fenOptions = new FenetreOptions(informations);
    fenBornes = new FenetreBornes(informations);
    fenFonctions = new FenetreFonctions(informations);      
    fenAPropos = new apropos();
    fenImage = new ImageSave(informations);
    fenValeurs = new FenTableauValeurs(informations);
    fenPrint = new Print(informations);
    keyboard = new Keyboard();

    scene = new MainGraph(informations); // it has to be the last thing to create.


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
}

void MainWindow::createMenus()
{
    QMenu *menuFichier = menuBar()->addMenu(tr("Fichier"));
    QMenu *menuOutils = menuBar()->addMenu(tr("Outils"));
    QMenu *menuAffichage = menuBar()->addMenu(tr("Fenêtres"));
    QMenu *menuAide = menuBar()->addMenu("?");   

    boutonGrille = menuOutils->addAction(QIcon(":/icons/quadrillage.png"), tr("Afficher/Cacher le quadrillage"));
    boutonGrille->setCheckable(true);

    QAction *actionRepereOrthonorme = menuOutils->addAction(tr("Base orthonormale"));
    actionRepereOrthonorme->setCheckable(true);
    connect(actionRepereOrthonorme, SIGNAL(triggered(bool)), informations, SLOT(setOrthonormal(bool)));
    connect(informations, SIGNAL(newOrthonormalityState(bool)), actionRepereOrthonorme, SLOT(setChecked(bool)));

    QAction *resetView = menuOutils->addAction(QIcon(":/icons/resetToDefaultView.png"), tr("Rétablir vue par défaut"));
    connect(resetView, SIGNAL(triggered()), fenBornes, SLOT(resetToStandardView()));

    QAction *afficherFenAPropos = menuAide->addAction(tr("à Propos..."));
    connect(afficherFenAPropos, SIGNAL(triggered()), this, SLOT(showAboutWin()));

    QAction *print = menuFichier->addAction(QIcon(":/icons/print.png"), tr("Imprimer"));
    print->setShortcut(QKeySequence("Ctrl+P"));
    connect(print, SIGNAL(triggered()), this, SLOT(showPrintWin()));

    QAction *saveImage = menuFichier->addAction(QIcon(":/icons/enregistrerImage.png"), tr("Enregistrer en image"));
    saveImage->setShortcut(QKeySequence("Ctrl+S"));
    connect(saveImage, SIGNAL(triggered()), this, SLOT(showImageSaveWin()));

    QAction *actionAfficherFenOptions = menuFichier->addAction(QIcon(":/icons/settings.png"), tr("Options"));
    actionAfficherFenOptions->setShortcut(QKeySequence("Ctrl+O"));
    connect(actionAfficherFenOptions, SIGNAL(triggered()), this, SLOT(showOptionsWin()));

    menuFichier->addSeparator();

    QAction *actionQuitter = menuFichier->addAction(QIcon(":/icons/quitter.png"), tr("Quitter"));
    actionQuitter->setShortcut(QKeySequence("Ctrl+Q"));
    connect(actionQuitter, SIGNAL(triggered()), this, SLOT(close()));

    QAction *actionAfficherFenFonctions = menuAffichage->addAction(QIcon(":/icons/fonctions.png"), tr("Fonctions"));
    actionAfficherFenFonctions->setShortcut(QKeySequence("Ctrl+F"));
    connect(actionAfficherFenFonctions, SIGNAL(triggered()), this, SLOT(showFuncsWin()));

    QAction *actionAfficherFenBornes = menuAffichage->addAction(QIcon(":/icons/bornes.png"), tr("Bornes"));
    actionAfficherFenBornes->setShortcut(QKeySequence("Ctrl+B"));
    connect(actionAfficherFenBornes, SIGNAL(triggered()), this, SLOT(showRangeWin()));

    QAction *tabValeurs = menuAffichage->addAction(QIcon(":/icons/tableauDeValeurs.png"), tr("Tableau de valeurs"));
    tabValeurs->setShortcut(QKeySequence("Ctrl+Tab"));
    connect(tabValeurs, SIGNAL(triggered()), this, SLOT(showValuesTabWin()));

    QAction *dispKeyboard = menuAffichage->addAction(QIcon(":/icons/keyboard.png"), tr("Clavier numérique"));
    dispKeyboard->setShortcut(QKeySequence("Ctrl+K"));
    connect(dispKeyboard, SIGNAL(triggered()), this, SLOT(showKeyboard()));

    QToolBar *toolBar = new QToolBar(tr("fenêtres et actions"));
    addToolBar(Qt::LeftToolBarArea, toolBar);

    toolBar->addAction(boutonGrille);
    toolBar->addAction(resetView);
    toolBar->addSeparator();
    toolBar->addAction(actionAfficherFenFonctions);    
    toolBar->addAction(actionAfficherFenBornes);  
    toolBar->addAction(tabValeurs);    
    toolBar->addAction(saveImage);    
    toolBar->addAction(print);  
    toolBar->addAction(dispKeyboard);

    statusBar();

    actionAfficherFenBornes->setStatusTip(tr("Affiche la fenêtre où l'on peut modifier les bornes de représentation: Xmin, Xmax..."));
    actionAfficherFenFonctions->setStatusTip(tr("Affiche le fenêtre où l'on peut écrire les expressions des fonctions à représenter."));
    actionQuitter->setStatusTip(tr("Quitte l'application."));
    actionAfficherFenOptions->setStatusTip(tr("Affiche la fenêtre où l'on peut modifier les options de représentation."));
    tabValeurs->setStatusTip(tr("Affiche une fenêtre contenant le tableau de valeurs pour chaque fonction."));
    resetView->setStatusTip(tr("Rétablit la vue par défaut: Xmin = Ymin = -10 , Xmax = Ymax = 10"));
    boutonGrille->setStatusTip(tr("Afficher/Cacher le quadrillage."));
    saveImage->setStatusTip(tr("Enregistrer le graphique dans une image."));
    dispKeyboard->setStatusTip(tr("Affiche un clavier numérique virtuel."));
    print->setStatusTip(tr("Imprimer ou exporter le graphique vers fichier PDF."));
}


void MainWindow::makeConnects()
{   
    connect(boutonGrille, SIGNAL(triggered(bool)), informations, SLOT(setGridState(bool)));
    connect(scene, SIGNAL(sizeChanged(int,int)), fenImage, SLOT(setSize(int,int)));
    connect(fenFonctions, SIGNAL(displayKeyboard()), this, SLOT(showKeyboard()));
}

void MainWindow::showFuncsWin()
{
    fenFonctions->move(this->pos() - QPoint(fenFonctions->width(),0));
    fenFonctions->show();    
    fenFonctions->activateWindow();
}

void MainWindow::showRangeWin()
{
    fenBornes->move(this->pos() + QPoint(this->width() + 20,0));
    fenBornes->show();
    fenBornes->activateWindow();
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
    keyboard->move(fenFonctions->frameGeometry().bottomLeft());
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
    fenBornes->close();

    fenFonctions->closeAllOpenedWindows();
    fenFonctions->close();

    fenOptions->saveSettings();
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
    delete fenBornes;
    delete fenFonctions;
    delete fenOptions;
    delete fenAPropos;
    delete fenImage;
}




