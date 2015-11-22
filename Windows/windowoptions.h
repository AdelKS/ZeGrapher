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



#ifndef WINDOWOPTIONS_H
#define WINDOWOPTIONS_H

#include "Structures.h"
#include "information.h"
#include "Widgets/qcolorbutton.h"


namespace Ui {
    class windowoptions;
}

class WindowOptions : public QWidget
{
    Q_OBJECT

public:
    explicit WindowOptions(Information *info);
    ~WindowOptions();   

public slots:
    void saveSettings();

private slots:
    void apply();
    void readSavedSettings();

private:

    Information *information;
    Ui::windowoptions *ui;   
    Options parameters;
    QColorButton *colorAxes, *colorFond, *colorGrid;

};

#endif // WINDOWOPTIONS_H
