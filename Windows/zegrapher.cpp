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


MainWindow::MainWindow()
{      
    informations = new Informations();

    fenFonctions = new FenetreFonctions(informations);
    fenBornes = new FenetreBornes(informations);
    fenOptions = new FenetreOptions(informations);
    fenAPropos = new apropos();
    fenImage = new ImageSave(informations);
    fenValeurs = new FenTableauValeurs(informations);
    fenPrint = new Print(informations);
    scene = new MainGraph(informations);
    keyboard = new Keyboard();

    setWindowIcon(QIcon(":/icons/logoLogiciel.png"));
    setMinimumSize(800,600);
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

    QAction *afficherFenAPropos = menuAide->addAction(tr("à Propos..."));
    connect(afficherFenAPropos, SIGNAL(triggered()), fenAPropos, SLOT(show()));

    QAction *print = menuFichier->addAction(QIcon(":/icons/print.png"), tr("Imprimer..."));
    print->setShortcut(QKeySequence("Ctrl+P"));
    connect(print, SIGNAL(triggered()), fenPrint, SLOT(show()));

    QAction *saveImage = menuFichier->addAction(QIcon(":/icons/enregistrerImage.png"), tr("Enregistrer image..."));
    saveImage->setShortcut(QKeySequence("Ctrl+S"));
    connect(saveImage, SIGNAL(triggered()), fenImage, SLOT(show()));

    QAction *actionAfficherFenOptions = menuFichier->addAction(tr("Options"));
    actionAfficherFenOptions->setShortcut(QKeySequence("Ctrl+O"));
    connect(actionAfficherFenOptions, SIGNAL(triggered()), fenOptions, SLOT(show()));

    menuFichier->addSeparator();

    QAction *actionQuitter = menuFichier->addAction(QIcon(":/icons/quitter.png"), tr("Quitter"));
    actionQuitter->setShortcut(QKeySequence("Ctrl+Q"));
    connect(actionQuitter, SIGNAL(triggered()), this, SLOT(close()));

    QAction *actionAfficherFenFonctions = menuAffichage->addAction(QIcon(":/icons/fonctions.png"), tr("Fonctions"));
    actionAfficherFenFonctions->setShortcut(QKeySequence("Ctrl+F"));
    connect(actionAfficherFenFonctions, SIGNAL(triggered()), fenFonctions, SLOT(show()));

    QAction *actionAfficherFenBornes = menuAffichage->addAction(QIcon(":/icons/bornes.png"), tr("Bornes"));
    actionAfficherFenBornes->setShortcut(QKeySequence("Ctrl+B"));
    connect(actionAfficherFenBornes, SIGNAL(triggered()), fenBornes, SLOT(show()));

    QAction *tabValeurs = menuAffichage->addAction(QIcon(":/icons/tableauDeValeurs.png"), tr("Tableau de valeurs"));
    tabValeurs->setShortcut(QKeySequence("Ctrl+Tab"));
    connect(tabValeurs, SIGNAL(triggered()), fenValeurs, SLOT(show()));

    QAction *dispKeyboard = menuAffichage->addAction(QIcon(":/icons/keyboard.png"), tr("Clavier numérique"));
    dispKeyboard->setShortcut(QKeySequence("Ctrl+K"));
    connect(dispKeyboard, SIGNAL(triggered()), keyboard, SLOT(show()));

    QToolBar *toolBar = addToolBar("fenetres");

    toolBar->addAction(boutonGrille);
    toolBar->addSeparator();
    toolBar->addAction(actionAfficherFenFonctions);
    toolBar->addSeparator();
    toolBar->addAction(actionAfficherFenBornes);
    toolBar->addSeparator();
    toolBar->addAction(tabValeurs);
    toolBar->addSeparator();
    toolBar->addAction(saveImage);
    toolBar->addSeparator();
    toolBar->addAction(print);
    toolBar->addSeparator();
    toolBar->addAction(dispKeyboard);

    statusBar();

    actionAfficherFenBornes->setStatusTip(tr("Affiche la fenêtre où l'on peut modifier les bornes de représentation: Xmin, Xmax..."));
    actionAfficherFenFonctions->setStatusTip(tr("Affiche le fenêtre où l'on peut écrire les expressions des fonctions à représenter."));
    actionQuitter->setStatusTip(tr("Quitte l'application."));
    actionAfficherFenOptions->setStatusTip(tr("Affiche la fenêtre où l'on peut modifier les options de représentation."));
    tabValeurs->setStatusTip(tr("Affiche une fenêtre contenant le tableau de valeurs pour chaque fonction."));
    boutonGrille->setStatusTip(tr("Afficher/Cacher le quadrillage."));
    saveImage->setStatusTip(tr("Enregistrer le graphique dans une image."));
    dispKeyboard->setStatusTip(tr("Affiche un clavier numérique virtuel."));
    print->setStatusTip(tr("Imprimer ou exporter le graphique vers fichier PDF."));
}


void MainWindow::makeConnects()
{   
    connect(boutonGrille, SIGNAL(triggered(bool)), informations, SLOT(setGridState(bool)));
    connect(scene, SIGNAL(sizeChanged(int,int)), fenImage, SLOT(setSize(int,int)));
    connect(fenFonctions, SIGNAL(displayKeyboard()), keyboard, SLOT(show()));
}

void MainWindow::closeEvent(QCloseEvent *evenement)
{
    fenBornes->close();

    fenFonctions->closeAllOpenedWindows();
    fenFonctions->close();

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




