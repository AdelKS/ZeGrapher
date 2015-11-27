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


#include "Windows/settings.h"
#include "ui_settings.h"

Settings::Settings(Information *info)
{
    information = info;

    ui = new Ui::Settings;
    ui->setupUi(this);

    setWindowIcon(QIcon(":/icons/settings.png"));

    readSavedSettings();

    parameters.colorBackground = colorFond->getCurrentColor();
    parameters.colorAxes = colorAxes->getCurrentColor();
    parameters.colorGrid = colorGrid->getCurrentColor();

    connect(ui->distanceWidget, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->thicknessWidget, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->numSize, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->smoothing, SIGNAL(toggled(bool)), this, SLOT(apply()));
    connect(colorAxes, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));
    connect(colorFond, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));
    connect(colorGrid, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));

    apply();

}

void Settings::readSavedSettings()
{
    QSettings settings;

    colorAxes = new QColorButton(Qt::black);
    colorAxes->setFixedSize(25,25);
    ui->axesColorLayout->addWidget(colorAxes);

    colorFond = new QColorButton(Qt::white);
    colorFond->setFixedSize(25,25);
    ui->backgroundColorLayout->addWidget(colorFond);

    colorGrid = new QColorButton(Qt::gray);
    colorGrid->setFixedSize(25,25);
    ui->gridColorLayout->addWidget(colorGrid);

    settings.beginGroup("graph");

    if(settings.contains("background_color"))
        colorFond->setColor(settings.value("background_color").value<QColor>());
    if(settings.contains("axes_color"))
        colorAxes->setColor(settings.value("axes_color").value<QColor>());
    if(settings.contains("grid_color"))
        colorGrid->setColor(settings.value("grid_color").value<QColor>());
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

}

void Settings::saveSettings()
{
    QSettings settings;
    settings.beginGroup("graph");

    settings.setValue("background_color", colorFond->getCurrentColor());
    settings.setValue("axes_color", colorAxes->getCurrentColor());
    settings.setValue("grid_color", colorGrid->getCurrentColor());
    settings.setValue("antialiasing", ui->smoothing->isChecked());

    settings.endGroup();

    settings.beginGroup("graph/curves");

    settings.setValue("quality", ui->distanceWidget->value());
    settings.setValue("thickness", ui->thicknessWidget->value());

    settings.endGroup();

}

void Settings::apply()
{
    if(colorAxes->getCurrentColor() == colorFond->getCurrentColor())
        QMessageBox::warning(this, tr("Warning"), tr("Axes and background colors are identical"));
    else if(colorFond->getCurrentColor() == colorGrid->getCurrentColor())
        QMessageBox::warning(this, tr("Warning"), tr("Background and grid colors are identical"));
    else
    {
        double dist = ui->distanceWidget->value();

        parameters.smoothing = ui->smoothing->isChecked();
        parameters.istanceBetweenPoints = pow(2, 2-dist/2);
        parameters.curvesThickness = ui->thicknessWidget->value();
        parameters.colorAxes = colorAxes->getCurrentColor();
        parameters.colorBackground = colorFond->getCurrentColor();
        parameters.colorGrid = colorGrid->getCurrentColor();
        parameters.numSize = ui->numSize->value();     

        information->setOptions(parameters);
    }
}

Settings::~Settings()
{
    delete ui;
}
