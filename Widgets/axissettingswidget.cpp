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

AxisSettingsWidget::AxisSettingsWidget(Information *information, QWidget *parent): QWidget(parent), ui(new Ui::AxisSettingsWidget)
{
    this->information = information;
    ui->setupUi(this);

    baseLineEdit = new NumberLineEdit();
    baseLineEdit->setValue(10);
    ui->baseFormLayout->addRow(tr("base b="), baseLineEdit);

    constantMultiplierLineEdit = new NumberLineEdit();
    constantMultiplierLineEdit->setValue(1);
    ui->multiplierFormLayout->addRow(tr("Fixed multiplier α="), constantMultiplierLineEdit);

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

    information->setAxesSettings(axesSettings);
}

void AxisSettingsWidget::axisTypeChanged()
{
    if(ui->logScale->isChecked())
    {
        ui->replaceBaseWithValueLabel->setText(tr("Replace b<sup>n &frasl; d</sup> with value"));
        ui->logScaleTickPosLabel->show();
        ui->basePowDenomLabel->show();
        ui->basePowDenom->show();

        ui->linearScaleTickPosLabel->hide();
    }
    else // ui->linScale is checked
    {
        ui->replaceBaseWithValueLabel->setText(tr("Replace 10<sup>n</sup> with value"));
        ui->logScaleTickPosLabel->hide();
        ui->basePowDenomLabel->hide();
        ui->basePowDenom->hide();

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

    ui->axisLineWidth->setValue(settings.lineWidth);


    if(settings.axisType == ZeAxisType::LINEAR)
    {
        constantMultiplierLineEdit->setText(settings.linSettings.constantMultiplierStr);
    }
    else
    {
        baseLineEdit->setText(settings.logSettings.baseStr);

        constantMultiplierLineEdit->setText(settings.logSettings.constantMultiplierStr);
    }



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
    axisSettings.color = axisColorButton->getCurrentColor();
    axisSettings.lineWidth = ui->axisLineWidth->value();

    if(axisSettings.axisType == ZeAxisType::LINEAR)
    {
        constantMultiplierLineEdit->checkVal();
        if(constantMultiplierLineEdit->isValid())
        {
            axisSettings.linSettings.constantMultiplier = constantMultiplierLineEdit->getValue();
            axisSettings.linSettings.constantMultiplierStr = constantMultiplierLineEdit->text();
        }
        else
        {
            axisSettings.linSettings.constantMultiplier = currentSettings.linSettings.constantMultiplier;
            axisSettings.linSettings.constantMultiplierStr = currentSettings.linSettings.constantMultiplierStr;
        }
        axisSettings.linSettings.maxDigitsNum = ui->maxDigits->value();
    }
    else
    {
        baseLineEdit->checkVal();
        if(baseLineEdit->isValid())
        {
            axisSettings.logSettings.base = baseLineEdit->getValue();
            axisSettings.logSettings.baseStr = baseLineEdit->text();
        }
        else
        {
            axisSettings.logSettings.base = currentSettings.logSettings.base;
            axisSettings.logSettings.baseStr = currentSettings.logSettings.baseStr;
        }

        constantMultiplierLineEdit->checkVal();
        if(constantMultiplierLineEdit->isValid())
        {
            axisSettings.logSettings.constantMultiplier = constantMultiplierLineEdit->getValue();
            axisSettings.logSettings.constantMultiplierStr = constantMultiplierLineEdit->text();
        }
        else
        {
            axisSettings.logSettings.constantMultiplier = currentSettings.logSettings.constantMultiplier;
            axisSettings.logSettings.constantMultiplierStr = currentSettings.logSettings.constantMultiplierStr;
        }
    }

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
    defaultSettings.color = Qt::black;
    defaultSettings.coordinateFormatting.decimalBase = false;
    defaultSettings.coordinateFormatting.decimalGlobalConstant = false;
    defaultSettings.lineWidth = 0.1;

    defaultSettings.logSettings.base = 10;
    defaultSettings.logSettings.baseStr = "10";
    defaultSettings.logSettings.constantMultiplier = 1;
    defaultSettings.logSettings.constantMultiplierStr = "1";

    defaultSettings.linSettings.constantMultiplier = 1;
    defaultSettings.logSettings.constantMultiplierStr = "1";
    defaultSettings.linSettings.maxDigitsNum = 5;

    axesSettings.x = axesSettings.y = defaultSettings;

    loadAxisSettingsInUi(ZeAxisName::X);

    emit settingsUpdated();
}

AxisSettingsWidget::~AxisSettingsWidget()
{
    delete ui;
}
