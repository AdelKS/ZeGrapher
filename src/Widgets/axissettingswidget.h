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

#ifndef AXISSETTINGS_H
#define AXISSETTINGS_H

#include "structures.h"
#include "Utils/loader.hpp"

#include <QWidget>


namespace Ui {
class AxisSettingsWidget;
}

class AxisSettingsWidget : public QWidget
{
    Q_OBJECT

    Q_ENUM(ZeAxisType);

public:
    explicit AxisSettingsWidget(QWidget *parent = nullptr);
    ZeAxisSettings getAxisSettings(ZeAxisName name);
    ZeAxesSettings getAxesSettings();
    ZeGridSettings getGridSettings();
    ~AxisSettingsWidget();

public slots:
    void apply();
    void processUserInput();

signals:
    void settingsUpdated();

protected slots:
    void swapGridData();

protected:

    void loadSettings(const Loader::Direction direction);

    enum struct SelectedAxis {X, Y, BOTH};

    void axisTypeChanged();
    void swapAxisData();
    void loadAxisSettingsInUI();
    SelectedAxis getSelectedAxis() const;
    void saveSettingsToDisk();

    void makeConnects();

    Ui::AxisSettingsWidget *ui;


    SelectedAxis currentAxis;
    ZeAxisSettings axisDefaultSettings;
    ZeAxesSettings axesSettings;

    Ze1DGridSettings gridDefaultSettings1D;
    ZeGridSettings gridSettings;
};

#endif // AXISSETTINGS_H
