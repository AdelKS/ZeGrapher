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
    ui->rangeSettingsLayout->addWidget(rangeAdjustmentsWidget);

    axisSettingsWidget = new AxisSettingsWidget(information);
    ui->axisSettingsLayout->addWidget(axisSettingsWidget);

    gridSettingsWidget = new ZeGridSettingsWidget(information);
    ui->gridSettingsLayout->addWidget(gridSettingsWidget);

    sizeAdjusmentsWidget = new GraphSizeAdjusments(information);
    ui->sizeAdjustmentsLayout->addWidget(sizeAdjusmentsWidget);

    exportWidget = new Export(information);
    ui->exportLayout->addWidget(exportWidget);

    backgroundColorButton = new QColorButton(Qt::white); 
    ui->formLayout->addRow(tr("Background color:"), backgroundColorButton);   

    defaultColorButton = new QColorButton(Qt::black);  
    ui->formLayout->addRow(tr("Curve default color:"), defaultColorButton);

    readSavedSettings();

    viewSettings.graph.backgroundColor = backgroundColorButton->getCurrentColor();
    viewSettings.graph.defaultColor = defaultColorButton->getCurrentColor();

    makeConnects();

    apply();

}

void Settings::loadDefaults()
{
    viewSettings.grid = gridSettingsWidget->getSettings();
}

void Settings::showExportSettings()
{

}

void Settings::makeConnects()
{
    connect(ui->distanceWidget, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->thicknessWidget, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->graphFontSize, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->graphFont, SIGNAL(currentFontChanged(QFont)), this, SLOT(apply()));
    connect(ui->smoothing, SIGNAL(toggled(bool)), this, SLOT(apply()));
    connect(backgroundColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));

    connect(defaultColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));
    connect(ui->reset, SIGNAL(released()), this, SLOT(resetToDefaultVals()));

    connect(axisSettingsWidget, &AxisSettingsWidget::settingsUpdated, this, &Settings::apply);
    connect(gridSettingsWidget, &ZeGridSettingsWidget::settingsUpdated, this, &Settings::apply);
}

void Settings::readSavedSettings()
{
    QSettings settings;

    settings.beginGroup("graph");

    if(settings.contains("background_color"))
        backgroundColorButton->setColor(settings.value("background_color").value<QColor>());
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
        if(settings.value("language").toString() == "fr")
            ui->languageComboBox->setCurrentIndex(1);
        else if(settings.value("language").toString() == "de")
            ui->languageComboBox->setCurrentIndex(2);
        else ui->languageComboBox->setCurrentIndex(0);
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
    else if(ui->languageComboBox->currentIndex() == 1)
        settings.setValue("language", "fr");
    else if(ui->languageComboBox->currentIndex() == 2)
        settings.setValue("language", "de");

    settings.beginGroup("font");

    settings.setValue("pixel_size", ui->appFontSize->value());
    settings.setValue("family", ui->appFontFamily->currentFont().family());
}

void Settings::resetToDefaultVals()
{
    int res = QMessageBox::question(this, tr("Reset to default values ?"), tr("Are you sure you want to restore the default values ?"));
    if(res == QMessageBox::No)
        return;

    backgroundColorButton->setColor(Qt::white);
    defaultColorButton->setColor(Qt::black);
    ui->distanceWidget->setValue(4);
    ui->graphFontSize->setValue(11);
    ui->thicknessWidget->setValue(1);
    ui->smoothing->setChecked(true);
    ui->updateCheckAtStart->setChecked(true);

    apply();
}

bool Settings::checkForUpdatesOnStart()
{
    return ui->updateCheckAtStart->isChecked();
}

void Settings::updateGraphSettings()
{
    viewSettings.graph.backgroundColor = backgroundColorButton->getCurrentColor();

    double dist = ui->distanceWidget->value();

    viewSettings.graph.smoothing = ui->smoothing->isChecked();
    viewSettings.graph.distanceBetweenPoints = pow(2, 2 - dist/2);
    viewSettings.graph.curvesThickness = ui->thicknessWidget->value();

    viewSettings.graph.defaultColor = defaultColorButton->getCurrentColor();

    QFont font(ui->graphFont->currentFont());
    font.setPixelSize(ui->graphFontSize->value());

    viewSettings.graph.graphFont = font;
}

void Settings::apply()
{   
    updateGraphSettings();

    axisSettingsWidget->processUserInput();
    viewSettings.axes = axisSettingsWidget->getSettings();

    rangeAdjustmentsWidget->processUserInput();
    viewSettings.range = rangeAdjustmentsWidget->getRange();

    gridSettingsWidget->processUserInput();
    viewSettings.grid = gridSettingsWidget->getSettings();

    sizeAdjusmentsWidget->processUserInput();
    viewSettings.graph.sizeSettings = sizeAdjusmentsWidget->getSettings();

    information->setViewSettings(viewSettings);
}

Settings::~Settings()
{
    delete ui;
}
