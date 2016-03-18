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



#include "Windows/settings.h"
#include "ui_settings.h"

Settings::Settings(Information *info)
{
    information = info;

    ui = new Ui::Settings;
    ui->setupUi(this);

    setWindowIcon(QIcon(":/icons/settings.png"));

    axesColorButton = new QColorButton(Qt::black);
    axesColorButton->setFixedSize(25,25);
    ui->axesColorLayout->addWidget(axesColorButton);

    backgroundColorButton = new QColorButton(Qt::white);
    backgroundColorButton->setFixedSize(25,25);
    ui->backgroundColorLayout->addWidget(backgroundColorButton);

    gridColorButton = new QColorButton(Qt::gray);
    gridColorButton->setFixedSize(25,25);
    ui->gridColorLayout->addWidget(gridColorButton);

    defaultColorButton = new QColorButton(Qt::black);
    defaultColorButton->setFixedSize(25,25);
    ui->defaultColorLayout->addWidget(defaultColorButton);

    readSavedSettings();

    parameters.backgroundColor = backgroundColorButton->getCurrentColor();
    parameters.axesColor = axesColorButton->getCurrentColor();
    parameters.gridColor = gridColorButton->getCurrentColor();
    parameters.defaultColor = defaultColorButton->getCurrentColor();

    connect(ui->distanceWidget, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->thicknessWidget, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->numSize, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->smoothing, SIGNAL(toggled(bool)), this, SLOT(apply()));
    connect(axesColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));
    connect(backgroundColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));
    connect(gridColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));
    connect(defaultColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));


    apply();

}

void Settings::readSavedSettings()
{
    QSettings settings;

    settings.beginGroup("graph");

    if(settings.contains("background_color"))
        backgroundColorButton->setColor(settings.value("background_color").value<QColor>());
    if(settings.contains("axes_color"))
        axesColorButton->setColor(settings.value("axes_color").value<QColor>());
    if(settings.contains("grid_color"))
        gridColorButton->setColor(settings.value("grid_color").value<QColor>());
    if(settings.contains("default_color"))
        defaultColorButton->setColor(settings.value("default_color").value<QColor>());
    if(settings.contains("antiasliasing"))
        ui->smoothing->setChecked(settings.value("antialiasing").toBool());

    settings.endGroup();

    settings.beginGroup("graph/font");

    if(settings.contains("pixel_size"))
        ui->numSize->setValue(settings.value("pixel_size").toInt());


    settings.endGroup();

    settings.beginGroup("graph/curves");

    if(settings.contains("quality"))
        ui->distanceWidget->setValue(settings.value("quality").toInt());
    if(settings.contains("thickness"))
        ui->thicknessWidget->setValue(settings.value("thickness").toInt());

    settings.endGroup();

    if(settings.contains("update_check_at_start"))
        ui->updateCheckAtStart->setChecked(settings.value("update_check_at_start").toBool());

}

void Settings::saveSettings()
{
    QSettings settings;
    settings.beginGroup("graph");

    settings.setValue("background_color", backgroundColorButton->getCurrentColor());
    settings.setValue("axes_color", axesColorButton->getCurrentColor());
    settings.setValue("grid_color", gridColorButton->getCurrentColor());
    settings.setValue("default_color", defaultColorButton->getCurrentColor());

    settings.setValue("antialiasing", ui->smoothing->isChecked());

    settings.endGroup();

    settings.beginGroup("graph/curves");

    settings.setValue("quality", ui->distanceWidget->value());
    settings.setValue("thickness", ui->thicknessWidget->value());

    settings.endGroup();

    settings.setValue("update_check_at_start", ui->updateCheckAtStart->isChecked());

}

void Settings::resetToDefaultVals()
{

}

void Settings::apply()
{
    if(axesColorButton->getCurrentColor() == backgroundColorButton->getCurrentColor())
        QMessageBox::warning(this, tr("Warning"), tr("Axes and background colors are identical"));
    else if(backgroundColorButton->getCurrentColor() == gridColorButton->getCurrentColor())
        QMessageBox::warning(this, tr("Warning"), tr("Background and grid colors are identical"));
    else
    {
        double dist = ui->distanceWidget->value();

        parameters.smoothing = ui->smoothing->isChecked();
        parameters.distanceBetweenPoints = pow(2, 2-dist/2);
        parameters.curvesThickness = ui->thicknessWidget->value();
        parameters.axesColor = axesColorButton->getCurrentColor();
        parameters.backgroundColor = backgroundColorButton->getCurrentColor();
        parameters.gridColor = gridColorButton->getCurrentColor();
        parameters.defaultColor = defaultColorButton->getCurrentColor();
        parameters.numSize = ui->numSize->value();
        parameters.updateCheckAtStart = ui->updateCheckAtStart->isChecked();

        information->setSettingsVals(parameters);
    }
}

Settings::~Settings()
{
    delete ui;
}
