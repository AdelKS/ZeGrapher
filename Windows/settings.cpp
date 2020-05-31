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

    estheticSettingsWidget = new EstheticSettings(information);
    ui->graphGeneralSettingsLayout->addWidget(estheticSettingsWidget);

    loadSettingsFromDisk();

    connect(ui->reset, SIGNAL(released()), this, SLOT(resetToDefaults()));

    apply();

}
void Settings::showExportSettings()
{
    // TODO
}

void Settings::loadSettingsFromDisk()
{
    QSettings settings;   

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

void Settings::resetToDefaults()
{
    int res = QMessageBox::question(this, tr("Reset to default settings ?"), tr("Are you sure you want to restore the default settings ?"));
    if(res == QMessageBox::No)
        return;   

    // TODO: make all sub-widgets reset to defaults, where it applies
}

bool Settings::checkForUpdatesOnStart()
{
    return ui->updateCheckAtStart->isChecked();
}

void Settings::apply()
{
    axisSettingsWidget->processUserInput();
    viewSettings.axes = axisSettingsWidget->getSettings();

    rangeAdjustmentsWidget->processUserInput();
    viewSettings.range = rangeAdjustmentsWidget->getRange();

    gridSettingsWidget->processUserInput();
    viewSettings.grid = gridSettingsWidget->getSettings();

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
