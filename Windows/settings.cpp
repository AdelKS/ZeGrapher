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

    graphSettings = info->getGraphSettings();

    ui = new Ui::Settings;
    ui->setupUi(this);

    setWindowIcon(QIcon(":/icons/settings.png"));

    axesColorButton = new QColorButton(Qt::black);
    axesColorButton->setFixedSize(20,20);
    ui->formLayout->addRow(tr("Axes color:"), axesColorButton);

    backgroundColorButton = new QColorButton(Qt::white);
    backgroundColorButton->setFixedSize(20,20);
    ui->formLayout->addRow(tr("Background color:"), backgroundColorButton);


    gridColorButton = new QColorButton(Qt::gray);
    gridColorButton->setFixedSize(20,20);
    ui->formLayout->addRow(tr("Grid color:"), gridColorButton);

    defaultColorButton = new QColorButton(Qt::black);
    defaultColorButton->setFixedSize(20,20);
    ui->formLayout->addRow(tr("Curve default color:"), defaultColorButton);

    readSavedSettings();

    graphSettings.backgroundColor = backgroundColorButton->getCurrentColor();
    graphSettings.axesColor = axesColorButton->getCurrentColor();
    graphSettings.gridColor = gridColorButton->getCurrentColor();
    graphSettings.defaultColor = defaultColorButton->getCurrentColor();

    connect(ui->distanceWidget, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->thicknessWidget, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->graphFontSize, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->graphFont, SIGNAL(currentFontChanged(QFont)), this, SLOT(apply()));
    connect(ui->smoothing, SIGNAL(toggled(bool)), this, SLOT(apply()));
    connect(axesColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));
    connect(backgroundColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));
    connect(gridColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));
    connect(defaultColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));
    connect(ui->reset, SIGNAL(released()), this, SLOT(resetToDefaultVals()));


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

    settings.beginGroup("font");

    if(settings.contains("pixel_size"))
        ui->graphFontSize->setValue(settings.value("pixel_size").toInt());
    if(settings.contains("family"))
        ui->graphFont->setCurrentFont(QFont(settings.value("family").toString()));

    settings.endGroup();

    settings.beginGroup("curves");

    if(settings.contains("quality"))
        ui->distanceWidget->setValue(settings.value("quality").toInt());
    if(settings.contains("thickness"))
        ui->thicknessWidget->setValue(settings.value("thickness").toInt());

    settings.endGroup();
    settings.endGroup();// end of "graph"

    settings.beginGroup("app");

    if(settings.contains("update_check_at_start"))
        ui->updateCheckAtStart->setChecked(settings.value("update_check_at_start").toBool());

    if(settings.contains("language"))
    {
        if(settings.value("language").toString() == "en")
            ui->languageComboBox->setCurrentIndex(0);
        else  ui->languageComboBox->setCurrentIndex(1);
    }

    settings.beginGroup("font");

    QFontInfo fontInfo(qApp->font());

    if(settings.contains("pixel_size"))
        ui->appFontSize->setValue(settings.value("pixel_size").toInt());
    else ui->appFontSize->setValue(fontInfo.pixelSize());

    if(settings.contains("family"))
        ui->appFontFamily->setFont(QFont(settings.value("family").toString()));
    else ui->appFontFamily->setFont(fontInfo.family());

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

    settings.beginGroup("font");
    settings.setValue("pixel_size", ui->graphFontSize->value());
    settings.setValue("family", ui->graphFont->currentFont().family());

    settings.endGroup();

    settings.beginGroup("curves");

    settings.setValue("quality", ui->distanceWidget->value());
    settings.setValue("thickness", ui->thicknessWidget->value());

    settings.endGroup();

    settings.endGroup();

    settings.beginGroup("app");

    settings.setValue("update_check_at_start", ui->updateCheckAtStart->isChecked());

    if(ui->languageComboBox->currentIndex() == 0)
        settings.setValue("language", "en");
    else settings.setValue("language", "fr");

    settings.beginGroup("font");

    settings.setValue("pixel_size", ui->appFontSize->value());
    settings.setValue("family", ui->appFontFamily->currentFont().family());
}

void Settings::resetToDefaultVals()
{
    int res = QMessageBox::question(this, tr("Reset to default values ?"), tr("Are you sure you want to restore the default values ?"));
    if(res == QMessageBox::No)
        return;

    axesColorButton->setColor(Qt::black);
    backgroundColorButton->setColor(Qt::white);
    gridColorButton->setColor(Qt::gray);
    defaultColorButton->setColor(Qt::black);
    ui->distanceWidget->setValue(4);
    ui->graphFontSize->setValue(11);
    ui->thicknessWidget->setValue(1);
    ui->smoothing->setChecked(true);
    ui->updateCheckAtStart->setChecked(true);

    apply();
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

        graphSettings.smoothing = ui->smoothing->isChecked();
        graphSettings.distanceBetweenPoints = pow(2, 2-dist/2);
        graphSettings.curvesThickness = ui->thicknessWidget->value();
        graphSettings.axesColor = axesColorButton->getCurrentColor();
        graphSettings.backgroundColor = backgroundColorButton->getCurrentColor();
        graphSettings.gridColor = gridColorButton->getCurrentColor();
        graphSettings.defaultColor = defaultColorButton->getCurrentColor();
        graphSettings.updateCheckAtStart = ui->updateCheckAtStart->isChecked();

        QFont font(ui->graphFont->currentFont());
        font.setPixelSize(ui->graphFontSize->value());

        graphSettings.graphFont = font;

        information->setGraphSettings(graphSettings);
    }
}

Settings::~Settings()
{
    delete ui;
}
