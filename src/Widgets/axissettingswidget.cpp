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

    loadDefaults();
    loadAxisSettingsInUI();
    makeConnects();

    axisTypeChanged();
}

void AxisSettingsWidget::apply()
{
    processUserInput();

    information.setAxesSettings(axesSettings);
    information.setGridSettings(gridSettings);
}

void AxisSettingsWidget::axisTypeChanged()
{
    if(ui->logScale->isChecked())
    {
        ui->logScaleTickPosLabel->show();
        ui->baseLabel->show();
        ui->base->show();

        ui->linearScaleTickPosLabel->hide();
    }
    else // ui->linScale is checked
    {
        ui->logScaleTickPosLabel->hide();
        ui->base->hide();
        ui->baseLabel->hide();

        ui->linearScaleTickPosLabel->show();
    }
}

void AxisSettingsWidget::swapAxisData()
{
    // process user input before switching to the the other axis
    processUserInput();

    ZeAxisName newChosenAxis = ui->xAxis->isChecked() ? ZeAxisName::X : ZeAxisName::Y;

    if(newChosenAxis != currentAxis)
        loadAxisSettingsInUI();
}

void AxisSettingsWidget::loadAxisSettingsInUI()
{
    ui->axisColor->setColor(axesSettings.color);
    ui->axisLineWidth->setValue(axesSettings.lineWidth);

    const ZeAxisSettings &axisSettings = ui->xAxis->isChecked() ? axesSettings.x : axesSettings.y;
    const Ze1DGridSettings &axisGridSettings = ui->xAxis->isChecked() ? gridSettings.x : gridSettings.y;

    {
        QSignalBlocker b1(ui->linearScale), b2(ui->logScale), b3(ui->axisLineWidth),
                b4(ui->tickRelSpacing), b5(ui->constantMultiplier),
                b6(ui->base), b7(ui->gridGroup), b8(ui->subGridGroup),
                b9(ui->showSubGridRelativePos), b10(ui->subgridDivs),
                b11(ui->gridColor), b12(ui->subgridColor),
                b13(ui->gridLineWidth), b14(ui->subGridLineWidth);

        ui->linearScale->setChecked(axisSettings.axisType == ZeAxisType::LINEAR);
        ui->logScale->setChecked(axisSettings.axisType == ZeAxisType::LOG);

        ui->tickRelSpacing->setValue(axisSettings.tickRelSpacing);

        if(axisSettings.axisType == ZeAxisType::LINEAR)
        {
            ui->constantMultiplier->setText(axisSettings.linSettings.constantMultiplierStr);
        }
        else
        {
            ui->base->setText(axisSettings.logSettings.baseStr);

            ui->constantMultiplier->setText(axisSettings.logSettings.constantMultiplierStr);
        }

        ui->gridGroup->setChecked(axisGridSettings.showGrid);
        ui->subGridGroup->setChecked(axisGridSettings.showSubGrid);
        ui->showSubGridRelativePos->setChecked(axisGridSettings.showSubgridRelativeCoordinates);
        ui->subgridDivs->setValue(axisGridSettings.subgridSubDivs);
        ui->gridColor->setColor(axisGridSettings.gridColor);
        ui->subgridColor->setColor(axisGridSettings.subgridColor);
        ui->gridLineWidth->setValue(axisGridSettings.gridLineWidth);
        ui->subGridLineWidth->setValue(axisGridSettings.subgridLineWidth);
    }

    currentAxis = ui->xAxis->isChecked() ? ZeAxisName::X : ZeAxisName::Y;
}

ZeAxisSettings AxisSettingsWidget::getAxisSettings(ZeAxisName name)
{
    if(name == ZeAxisName::X)
        return axesSettings.x;
    else return axesSettings.y;
}

ZeAxesSettings AxisSettingsWidget::getAxesSettings()
{
    return axesSettings;
}

ZeGridSettings AxisSettingsWidget::getGridSettings()
{
    return gridSettings;
}

void AxisSettingsWidget::processUserInput()
{
    axesSettings.color = ui->axisColor->getCurrentColor();
    axesSettings.lineWidth = ui->axisLineWidth->value();

    ZeAxisSettings &axisSettings = (currentAxis == ZeAxisName::X) ? axesSettings.x : axesSettings.y;

    axisSettings.axisType = ui->linearScale->isChecked() ? ZeAxisType::LINEAR :  ZeAxisType::LOG;
    axisSettings.tickRelSpacing = ui->tickRelSpacing->value();

    if(axisSettings.axisType == ZeAxisType::LINEAR)
    {
        ui->constantMultiplier->checkVal();
        if(ui->constantMultiplier->isValid())
        {
            axisSettings.linSettings.constantMultiplier = ui->constantMultiplier->getValue();
            axisSettings.linSettings.constantMultiplierStr = ui->constantMultiplier->text();
        }
        else
        {
            axisSettings.linSettings.constantMultiplier = axisDefaultSettings.linSettings.constantMultiplier;
            axisSettings.linSettings.constantMultiplierStr = axisDefaultSettings.linSettings.constantMultiplierStr;
        }
        axisSettings.linSettings.maxDigitsNum = ui->maxDigits->value();
    }
    else
    {
        ui->base->checkVal();
        if(ui->base->isValid())
        {
            axisSettings.logSettings.base = ui->base->getValue();
            axisSettings.logSettings.baseStr = ui->base->text();
        }
        else
        {
            axisSettings.logSettings.base = axisDefaultSettings.logSettings.base;
            axisSettings.logSettings.baseStr = axisDefaultSettings.logSettings.baseStr;
        }

        ui->constantMultiplier->checkVal();
        if(ui->constantMultiplier->isValid())
        {
            axisSettings.logSettings.constantMultiplier = ui->constantMultiplier->getValue();
            axisSettings.logSettings.constantMultiplierStr = ui->constantMultiplier->text();
        }
        else
        {
            axisSettings.logSettings.constantMultiplier = axisDefaultSettings.logSettings.constantMultiplier;
            axisSettings.logSettings.constantMultiplierStr = axisDefaultSettings.logSettings.constantMultiplierStr;
        }
    }

    Ze1DGridSettings &settingsToUpdate = currentAxis == ZeAxisName::X ? gridSettings.x : gridSettings.y;

    settingsToUpdate.showGrid = ui->gridGroup->isChecked();
    settingsToUpdate.showSubGrid = ui->subGridGroup->isChecked();
    settingsToUpdate.showSubgridRelativeCoordinates = ui->showSubGridRelativePos->isChecked();
    settingsToUpdate.subgridSubDivs = ui->subgridDivs->value();
    settingsToUpdate.gridColor = ui->gridColor->getCurrentColor();
    settingsToUpdate.subgridColor = ui->subgridColor->getCurrentColor();
    settingsToUpdate.gridLineWidth = ui->gridLineWidth->value();
    settingsToUpdate.subgridLineWidth = ui->subGridLineWidth->value();
}

void AxisSettingsWidget::loadDefaults()
{
    axisDefaultSettings.axisType = ZeAxisType::LINEAR;

    axisDefaultSettings.logSettings.base = 10;
    axisDefaultSettings.logSettings.baseStr = "10";
    axisDefaultSettings.logSettings.constantMultiplier = 1;
    axisDefaultSettings.logSettings.constantMultiplierStr = "1";

    axisDefaultSettings.linSettings.constantMultiplier = 1;
    axisDefaultSettings.logSettings.constantMultiplierStr = "1";
    axisDefaultSettings.linSettings.maxDigitsNum = 4;
    axisDefaultSettings.tickRelSpacing = 0;

    axesSettings.x = axisDefaultSettings;
    axesSettings.y = axisDefaultSettings;

    axesSettings.color = Qt::black;
    axesSettings.lineWidth = 1.1;

    // Grid settings

    ui->gridGroup->setChecked(true);

    gridDefaultSettings1D.showGrid = true;
    gridDefaultSettings1D.showSubGrid = false;
    gridDefaultSettings1D.showSubgridRelativeCoordinates = false;
    gridDefaultSettings1D.subgridSubDivs = 1;
    gridDefaultSettings1D.gridColor = Qt::gray;
    // TODO: update grid default colors
    gridDefaultSettings1D.subgridColor = Qt::gray;

    // TODO: fine tune grid and subgrid widths, check what antialiasing does.
    gridDefaultSettings1D.gridLineWidth = 0.6;
    gridDefaultSettings1D.subgridLineWidth = 0.3;

    gridSettings.x = gridSettings.y = gridDefaultSettings1D;

    information.setAxesSettings(axesSettings);
    information.setGridSettings(gridSettings);
}

void AxisSettingsWidget::makeConnects()
{
    connect(ui->axisLineWidth, SIGNAL(valueChanged(double)), this, SLOT(apply()));
    connect(ui->axisColor, &QColorButton::colorChanged, this, &AxisSettingsWidget::apply);

    connect(ui->gridColor, &QColorButton::colorChanged, this, &AxisSettingsWidget::apply);
    connect(ui->subgridColor, &QColorButton::colorChanged, this, &AxisSettingsWidget::apply);

    connect(ui->subGridGroup, SIGNAL(clicked(bool)), this, SLOT(apply()));
    connect(ui->gridGroup, SIGNAL(clicked(bool)), this, SLOT(apply()));

    connect(ui->gridLineWidth, SIGNAL(valueChanged(double)), this, SLOT(apply()));
    connect(ui->subgridDivs, SIGNAL(valueChanged(int)), this, SLOT(apply()));

    connect(ui->tickRelSpacing, SIGNAL(valueChanged(int)), this, SLOT(apply()));

    connect(ui->maxDigits, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->constantMultiplier, SIGNAL(returnPressed()), this, SLOT(apply()));

    connect(ui->linearScale, &QRadioButton::toggled, this, &AxisSettingsWidget::axisTypeChanged);
    connect(ui->logScale, &QRadioButton::toggled, this, &AxisSettingsWidget::axisTypeChanged);

    connect(ui->xAxis, &QPushButton::toggled, this, &AxisSettingsWidget::swapAxisData);
    connect(ui->xAxis, &QRadioButton::toggled, this, &AxisSettingsWidget::swapGridData);
}

void AxisSettingsWidget::swapGridData()
{
    ZeAxisName newChosenAxis = ui->xAxis->isChecked() ? ZeAxisName::X : ZeAxisName::Y;

    processUserInput();
    if (newChosenAxis != currentAxis)
        loadAxisSettingsInUI();
}

AxisSettingsWidget::~AxisSettingsWidget()
{
    delete ui;
}
