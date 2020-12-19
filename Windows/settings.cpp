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

#include "Windows/settings.h"
#include "ui_settings.h"

Settings::Settings(Information *info, QWidget *parent): QWidget(parent)
{
    information = info;

    ui = new Ui::Settings;
    ui->setupUi(this);

    setWindowFlags(Qt::Window);
    setWindowTitle(tr("Settings"));
    setWindowIcon(QIcon(":/icons/settings.png"));

    // to be instanced first so information->funcsList gets populated
    inputWidget = new MathObjectsInput(information, this);
    ui->objectsInputLayout->addWidget(inputWidget);

    rangeAdjustmentsWidget = new RangeAdjustments(information->getFuncsList(), information);
    ui->rangeAdjustmentsLayout->addWidget(rangeAdjustmentsWidget);

    axisSettingsWidget = new AxisSettingsWidget(information);
    ui->axesLayout->addWidget(axisSettingsWidget);

    sizeAdjusmentsWidget = new GraphSizeAdjusments(information);
    ui->sizeAdjustmentsLayout->addWidget(sizeAdjusmentsWidget);

    exportWidget = new Export(information);
    ui->exportLayout->addWidget(exportWidget);

    estheticSettingsWidget = new EstheticSettings(information);
    ui->graphGeneralSettingsLayout->addWidget(estheticSettingsWidget);

    appSettingsWidget = new AppSettings(information);
    ui->appSettingsLayout->addWidget(appSettingsWidget);

    apply();

}

void Settings::showExportSettings()
{
    // TODO
}

void Settings::saveSettingsToDisk()
{
    appSettingsWidget->saveSettingsToDisk();
    estheticSettingsWidget->saveSettingsToDisk();
}

void Settings::apply()
{
    axisSettingsWidget->processUserInput();
    viewSettings.axes = axisSettingsWidget->getSettings();

    rangeAdjustmentsWidget->processUserInput();
    viewSettings.range = rangeAdjustmentsWidget->getRange();

    sizeAdjusmentsWidget->processUserInput();
    viewSettings.graph.sizeSettings = sizeAdjusmentsWidget->getSettings();

    estheticSettingsWidget->processUserInput();
    viewSettings.graph.estheticSettings = estheticSettingsWidget->getSettings();

    information->setViewSettings(viewSettings);

}

Settings::~Settings()
{
    delete ui;
}
