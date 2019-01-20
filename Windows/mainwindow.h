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




#ifndef ZEGRAPHER_H
#define ZEGRAPHER_H

#include "structures.h"
#include "GraphDraw/maingraph.h"
#include "Windows/rangeadjustments.h"
#include "Windows/mathobjectsinput.h"
#include "Windows/settings.h"
#include "Windows/values.h"
#include "about.h"
#include "GraphDraw/exportpreview.h"
#include "Export/export.h"
#include "Widgets/keyboard.h"
#include "Windows/updatecheck.h"

#include <QSettings>
#include <QString>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow();
    ~MainWindow();

protected slots:
    void showAboutQtWin();

protected:
    void closeEvent(QCloseEvent *evenement);

    void createDocks();
    void makeConnects();
    void createMenus();
    void createZoomWidgets();
    void loadWindowSavedGeomtries();
    void saveWindowsGeometry();


    Information *information;
    MainGraph *mainGraph;
    MathObjectsInput *inputWin;
    RangeAdjustments *rangeWin;
    Settings *settingsWin;
    Values *valuesWin;
    Keyboard *keyboard;
    about *aboutWin;
    Export *exportWin;
    UpdateCheck *updateCheckWin;
    QAction *gridButton;
    QDockWidget *mathInputDock, *rangeWinDock;

    QSettings settings;

};

#endif // ZEGRAPHER_H
