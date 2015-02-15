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


#ifndef ZEGRAPHER_H
#define ZEGRAPHER_H

#include "Structures.h"
#include "GraphDraw/maingraph.h"
#include "Windows/fenetrebornes.h"
#include "Windows/fenetrefonctions.h"
#include "Windows/fenetreoptions.h"
#include "Windows/fentableauvaleurs.h"
#include "apropos.h"
#include "Export/imagesave.h"
#include "GraphDraw//printpreview.h"
#include "Export/print.h"
#include "Widgets/keyboard.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *evenement);
    void makeConnects();
    void createMenus();
    void createZoomWidgets();

protected slots:
    void showFuncsWin();
    void showRangeWin();
    void showOptionsWin();
    void showValuesTabWin();
    void showKeyboard();
    void showPrintWin();
    void showImageSaveWin();
    void showAboutWin();

private:

    Informations *informations;
    MainGraph *scene;
    GraphRange window;
    FenetreFonctions *fenFonctions;
    FenetreBornes *fenBornes;
    FenetreOptions *fenOptions;
    FenTableauValeurs *fenValeurs;
    Keyboard *keyboard;
    ImageSave *fenImage;
    apropos *fenAPropos;
    Print *fenPrint;
    QAction *boutonGrille;
    ImagePreview *fenImageScene;
    PrintPreview *fenPrintScene;

};

#endif // ZEGRAPHER_H
