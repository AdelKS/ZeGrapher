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




#ifndef SETTINGS_H
#define SETTINGS_H

#include "Structures.h"
#include "information.h"
#include "Widgets/qcolorbutton.h"


namespace Ui {
    class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(Information *info, QWidget *parent = 0);
    ~Settings();

public slots:
    void saveSettings();

private slots:
    void resetToDefaultVals();
    void apply();
    void readSavedSettings();

private:

    Information *information;
    Ui::Settings *ui;
    SettingsVals parameters;
    QColorButton *axesColorButton, *backgroundColorButton, *gridColorButton, *defaultColorButton;

};

#endif // SETTINGS_H
