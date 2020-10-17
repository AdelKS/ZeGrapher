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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

#include "information.h"
#include "Widgets/axissettingswidget.h"
#include "Widgets/rangeadjustments.h"
#include "Widgets/zegridsettingswidget.h"
#include "Widgets/graphsizeadjusments.h"
#include "Widgets/export.h"
#include "GraphDraw/mainview.h"
#include "Widgets/mathobjectsinput.h"
#include "Widgets/estheticsettings.h"
#include "Widgets/appsettings.h"
/* TODO:
 * - Give out axis settings to the rest of the program through information
 * - Save new settings to the hard disk: base, base power etc.
 * - create a color generator for new math objects, just like Matplotlib
 *
 * */


namespace Ui {
    class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(Information *info, QWidget *parent = nullptr);
    ~Settings();

public slots:
    void saveSettingsToDisk();
    void showExportSettings();

private slots:
    void apply();

private:
    Information *information;
    Ui::Settings *ui;
    
    QTimer timer;
    MathObjectsInput *inputWidget;
    ZeViewSettings viewSettings;
    ZeAxisName currentEditedGrid;   
    AxisSettingsWidget *axisSettingsWidget;    
    RangeAdjustments *rangeAdjustmentsWidget;
    ZeGridSettingsWidget *gridSettingsWidget;
    GraphSizeAdjusments *sizeAdjusmentsWidget;
    EstheticSettings *estheticSettingsWidget;
    Export *exportWidget;
    AppSettings *appSettingsWidget;

};

#endif // SETTINGS_H
