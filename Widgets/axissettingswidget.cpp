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

#include "axissettingswidget.h"
#include "ui_axissettingswidget.h"

/* TODO:
 * - Save and load defaults from disk
 * - Base should be > 1 (Strictly)
 **/

AxisSettingsWidget::AxisSettingsWidget(QWidget *parent): QWidget(parent), ui(new Ui::AxisSettingsWidget)
{
    ui->setupUi(this);

    baseLineEdit = new NumberLineEdit();
    baseLineEdit->setValue(10);
    ui->baseFormLayout->addRow(tr("base b="), baseLineEdit);

    fixedMultiplierLineEdit = new NumberLineEdit();
    fixedMultiplierLineEdit->setValue(1);
    ui->multiplierFormLayout->addRow(tr("Fixed multiplier α="), fixedMultiplierLineEdit);

    resetToDefaults();
    currentAxis = ZeAxisName::X;
    loadAxisSettingsInUi(axesSettings.x);

    axisColorButton = new QColorButton();
    ui->generalSettingsLayout->addRow(tr("Color"), axisColorButton);

    connect(ui->linearScale, &QRadioButton::toggled, this, &AxisSettingsWidget::axisTypeChanged);
    connect(ui->logScale, &QRadioButton::toggled, this, &AxisSettingsWidget::axisTypeChanged);
    connect(ui->xAxis, &QPushButton::toggled, this, &AxisSettingsWidget::swapAxisData);
}

void AxisSettingsWidget::apply()
{
    processUserInput();

    emit settingsUpdated();
}

void AxisSettingsWidget::axisTypeChanged()
{
    if(ui->logScale->isChecked())
    {
        ui->replaceBaseWithValueLabel->setText(tr("Replace b<sup>n &frasl; d</sup> with value"));
        ui->logScaleTickPosLabel->show();
        ui->basePowDenomLabel->show();
        ui->basePowDenom->show();

        ui->linDivisions->hide();
        ui->linDivisionsLabel->hide();
        ui->linearScaleTickPosLabel->hide();
    }
    else // ui->linScale is checked
    {
        ui->replaceBaseWithValueLabel->setText(tr("Replace 10<sup>n</sup> with value"));
        ui->logScaleTickPosLabel->hide();
        ui->basePowDenomLabel->hide();
        ui->basePowDenom->hide();

        ui->linDivisions->show();
        ui->linDivisionsLabel->show();
        ui->linearScaleTickPosLabel->show();
    }
}

void AxisSettingsWidget::swapAxisData()
{
    // process user input before switching to the the other axis
    processUserInput();

    ZeAxisName newChosenAxis = ui->xAxis->isChecked() ? ZeAxisName::X : ZeAxisName::Y;

    if(newChosenAxis != currentAxis)
        loadAxisSettingsInUi(newChosenAxis);

}

void AxisSettingsWidget::loadAxisSettingsInUi(ZeAxisName name)
{
    currentAxis = name;
    const ZeAxisSettings &settings = name == ZeAxisName::X ? axesSettings.x : axesSettings.y;
    loadAxisSettingsInUi(settings);
}

void AxisSettingsWidget::loadAxisSettingsInUi(const ZeAxisSettings &settings)
{
    ui->linearScale->setChecked(settings.axisType == ZeAxisType::LINEAR);
    ui->logScale->setChecked(settings.axisType == ZeAxisType::LOG);

    baseLineEdit->setText(settings.baseStr);
    ui->powerDenominator->setValue(settings.basePowDenom);
    ui->powerNumerator->setValue(settings.basePowNum);

    ui->decimalBase->setChecked(settings.coordinateFormatting.decimalBase);
    ui->decimalGlobalConstant->setChecked(settings.coordinateFormatting.decimalGlobalConstant);
}

ZeAxisSettings AxisSettingsWidget::getSettings(ZeAxisName name)
{
    if(name == ZeAxisName::X)
        return axesSettings.x;
    else return axesSettings.y;
}

ZeAxesSettings AxisSettingsWidget::getSettings()
{
    return axesSettings;
}

void AxisSettingsWidget::processUserInput()
{
    ZeAxisSettings axisSettings;

    // Setting currentSettings to the currently selected axis in the ui, before user change, if the user triggered the change
    ZeAxisSettings &currentSettings = currentAxis == ZeAxisName::X ? axesSettings.x : axesSettings.y;

    axisSettings.axisType = ui->linearScale->isChecked() ? ZeAxisType::LINEAR :  ZeAxisType::LOG;

    baseLineEdit->checkVal();
    if(baseLineEdit->isValid())
    {
        axisSettings.base = baseLineEdit->getValue();
        axisSettings.baseStr = baseLineEdit->text();
    }
    else
    {
        axisSettings.base = currentSettings.base;
        axisSettings.baseStr = currentSettings.baseStr;
    }

    axisSettings.basePowDenom = ui->powerDenominator->value();
    axisSettings.basePowNum = ui->powerNumerator->value();

    axisSettings.coordinateFormatting.decimalBase = ui->decimalBase->isChecked();
    axisSettings.coordinateFormatting.decimalGlobalConstant = ui->decimalGlobalConstant->isChecked();

    if(currentAxis == ZeAxisName::X)
        axesSettings.x = axisSettings;
    else axesSettings.y = axisSettings;
}

void AxisSettingsWidget::resetToDefaults()
{
    ZeAxisSettings defaultSettings;

    defaultSettings.axisType = ZeAxisType::LINEAR;
    defaultSettings.base = 10;
    defaultSettings.baseStr = "10";
    defaultSettings.basePowDenom = 1;
    defaultSettings.basePowNum = 1;
    defaultSettings.coordinateFormatting.decimalBase = false;
    defaultSettings.coordinateFormatting.decimalGlobalConstant = false;
    defaultSettings.linearDivider = 10;
    defaultSettings.logDivisions = 8;

    axesSettings.x = axesSettings.y = defaultSettings;

    loadAxisSettingsInUi(ZeAxisName::X);

    emit settingsUpdated();
}

AxisSettingsWidget::~AxisSettingsWidget()
{
    delete ui;
}
