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

Settings::Settings(QWidget *parent): QWidget(parent)
{
    ui = new Ui::Settings;
    ui->setupUi(this);

    setWindowFlags(Qt::Window);
    setWindowTitle(tr("Settings"));
    setWindowIcon(QIcon(":/icons/settings.png"));

    rangeAdjustmentsQMLWidget = new QQuickWidget;
    rangeAdjustmentsQMLWidget->setSource(QUrl::fromLocalFile(":/src/QML/UserInputPanel.qml"));
    rangeAdjustmentsQMLWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    ui->mathInputLayout->addWidget(rangeAdjustmentsQMLWidget);
    rangeAdjustmentsQMLWidget->setMinimumHeight(200);

    // to be instanced first so information.funcsList gets populated
    inputWidget = new MathObjectsInput(this);
    // ui->mathInputLayout->addWidget(inputWidget);

    axisSettingsWidget = new AxisSettingsWidget();
    ui->axesLayout->addWidget(axisSettingsWidget);

    sizeAdjusmentsWidget = new GraphSizeSettings();
    ui->sizeAdjustmentsLayout->addWidget(sizeAdjusmentsWidget);

    exportWidget = new Export();
    ui->exportLayout->addWidget(exportWidget);

    graphSettingsWidget = new GraphSettings();
    ui->graphGeneralSettingsLayout->addWidget(graphSettingsWidget);

    appSettingsWidget = new AppSettings();
    ui->appSettingsLayout->addWidget(appSettingsWidget);

    updateVisibleWidgets();

    connect(ui->mathInputButton, SIGNAL(toggled(bool)), this, SLOT(updateVisibleWidgets()));
    connect(ui->exportButton, SIGNAL(toggled(bool)), this, SLOT(updateVisibleWidgets()));
}

void Settings::updateVisibleWidgets()
{
    if (ui->mathInputButton->isChecked())
    {
        ui->mathInputWidget->show();

        ui->settingsWidget->hide();
    }
    else
    {
        ui->mathInputWidget->hide();

        ui->settingsWidget->show();
    }

}

void Settings::showExportSettings()
{

}

Settings::~Settings()
{
    delete ui;
}
