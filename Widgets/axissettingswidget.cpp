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

    QHBoxLayout *multiplierLayout = new QHBoxLayout();
    multiplierLayout->setMargin(0);
    multiplierLayout->addWidget(new QLabel("α ="));
    multiplierLayout->addWidget(constantMultiplierLineEdit);
    ui->tickSettingsForm->addRow(tr("Multiplier"), multiplierLayout);

    axisColorButton = new QColorButton();
    ui->axisSettingsForm->addRow(tr("Color"), axisColorButton);

    gridColorButton = new QColorButton(Qt::gray);
    ui->gridFormLayout->addRow(tr("Color"), gridColorButton);

    subgridColorButton = new QColorButton(Qt::gray);
    ui->subgridFormLayout->addRow(tr("Color"), subgridColorButton);

    loadDefaults();
    loadSettingsInUi(ui->xAxis->isChecked() ? ZeAxisName::X : ZeAxisName::Y);
    makeConnects();

    axisTypeChanged();
}

void AxisSettingsWidget::apply()
{
    processUserInput();

    information->setAxesSettings(axesSettings);
    information->setGridSettings(gridSettings);
}

void AxisSettingsWidget::axisTypeChanged()
{
    if(ui->logScale->isChecked())
    {
        ui->logScaleTickPosLabel->show();
        ui->baseWidget->show();

        ui->linearScaleTickPosLabel->hide();
    }
    else // ui->linScale is checked
    {
        ui->logScaleTickPosLabel->hide();
        ui->baseWidget->hide();

        ui->linearScaleTickPosLabel->show();
    }
}

void AxisSettingsWidget::swapAxisData()
{
    // process user input before switching to the the other axis
    processUserInput();

    ZeAxisName newChosenAxis = ui->xAxis->isChecked() ? ZeAxisName::X : ZeAxisName::Y;

    if(newChosenAxis != currentAxis)
        loadSettingsInUi(newChosenAxis);
}

void AxisSettingsWidget::loadSettingsInUi(ZeAxisName axisName)
{
    const ZeAxisSettings &axisSettings = axisName == ZeAxisName::X ? axesSettings.x : axesSettings.y;
    const Ze1DGridSettings &axisGridSettings = axisName == ZeAxisName::X ? gridSettings.x : gridSettings.y;

    {
        QSignalBlocker b1(ui->linearScale), b2(ui->logScale), b3(ui->axisLineWidth),
                b4(ui->tickRelSpacing), b5(constantMultiplierLineEdit),
                b6(baseLineEdit), b7(ui->gridGroup), b8(ui->subGridGroup),
                b9(ui->showSubGridRelativePos), b10(ui->subgridDivs),
                b11(gridColorButton), b12(subgridColorButton),
                b13(ui->gridLineWidth), b14(ui->subGridLineWidth);

        ui->linearScale->setChecked(axisSettings.axisType == ZeAxisType::LINEAR);
        ui->logScale->setChecked(axisSettings.axisType == ZeAxisType::LOG);

        ui->axisLineWidth->setValue(axisSettings.lineWidth);
        ui->tickRelSpacing->setValue(axisSettings.tickRelSpacing);

        if(axisSettings.axisType == ZeAxisType::LINEAR)
        {
            constantMultiplierLineEdit->setText(axisSettings.linSettings.constantMultiplierStr);
        }
        else
        {
            baseLineEdit->setText(axisSettings.logSettings.baseStr);

            constantMultiplierLineEdit->setText(axisSettings.logSettings.constantMultiplierStr);
        }

        ui->gridGroup->setChecked(axisGridSettings.showGrid);
        ui->subGridGroup->setChecked(axisGridSettings.showSubGrid);
        ui->showSubGridRelativePos->setChecked(axisGridSettings.showSubgridRelativeCoordinates);
        ui->subgridDivs->setValue(axisGridSettings.subgridSubDivs);
        gridColorButton->setColor(axisGridSettings.gridColor);
        subgridColorButton->setColor(axisGridSettings.subgridColor);
        ui->gridLineWidth->setValue(axisGridSettings.gridLineWidth);
        ui->subGridLineWidth->setValue(axisGridSettings.subgridLineWidth);
    }

    currentAxis = axisName;
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
    ZeAxisSettings &axisSettings = (currentAxis == ZeAxisName::X) ? axesSettings.x : axesSettings.y;

    axisSettings.axisType = ui->linearScale->isChecked() ? ZeAxisType::LINEAR :  ZeAxisType::LOG;
    axisSettings.color = axisColorButton->getCurrentColor();
    axisSettings.lineWidth = ui->axisLineWidth->value();
    axisSettings.tickRelSpacing = ui->tickRelSpacing->value();

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
            axisSettings.linSettings.constantMultiplier = axisDefaultSettings.linSettings.constantMultiplier;
            axisSettings.linSettings.constantMultiplierStr = axisDefaultSettings.linSettings.constantMultiplierStr;
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
            axisSettings.logSettings.base = axisDefaultSettings.logSettings.base;
            axisSettings.logSettings.baseStr = axisDefaultSettings.logSettings.baseStr;
        }

        constantMultiplierLineEdit->checkVal();
        if(constantMultiplierLineEdit->isValid())
        {
            axisSettings.logSettings.constantMultiplier = constantMultiplierLineEdit->getValue();
            axisSettings.logSettings.constantMultiplierStr = constantMultiplierLineEdit->text();
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
    settingsToUpdate.gridColor = gridColorButton->getCurrentColor();
    settingsToUpdate.subgridColor = subgridColorButton->getCurrentColor();
    settingsToUpdate.gridLineWidth = ui->gridLineWidth->value();
    settingsToUpdate.subgridLineWidth = ui->subGridLineWidth->value();
}

void AxisSettingsWidget::loadDefaults()
{
    axisDefaultSettings.axisType = ZeAxisType::LINEAR;
    axisDefaultSettings.color = Qt::black;
    axisDefaultSettings.lineWidth = 1.0;

    axisDefaultSettings.logSettings.base = 10;
    axisDefaultSettings.logSettings.baseStr = "10";
    axisDefaultSettings.logSettings.constantMultiplier = 1;
    axisDefaultSettings.logSettings.constantMultiplierStr = "1";

    axisDefaultSettings.linSettings.constantMultiplier = 1;
    axisDefaultSettings.logSettings.constantMultiplierStr = "1";
    axisDefaultSettings.linSettings.maxDigitsNum = 5;
    axisDefaultSettings.tickRelSpacing = 0;

    axesSettings.x = axisDefaultSettings;
    axesSettings.y = axisDefaultSettings;

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

    information->setAxesSettings(axesSettings);
    information->setGridSettings(gridSettings);
}

void AxisSettingsWidget::makeConnects()
{
    connect(gridColorButton, &QColorButton::colorChanged, this, &AxisSettingsWidget::apply);
    connect(subgridColorButton, &QColorButton::colorChanged, this, &AxisSettingsWidget::apply);

    connect(ui->subGridGroup, SIGNAL(clicked(bool)), this, SLOT(apply()));
    connect(ui->gridGroup, SIGNAL(clicked(bool)), this, SLOT(apply()));

    connect(ui->gridLineWidth, SIGNAL(valueChanged(double)), this, SLOT(apply()));
    connect(ui->subgridDivs, SIGNAL(valueChanged(int)), this, SLOT(apply()));

    connect(ui->tickRelSpacing, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->axisLineWidth, SIGNAL(valueChanged(double)), this, SLOT(apply()));

    connect(ui->maxDigits, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(constantMultiplierLineEdit, SIGNAL(returnPressed()), this, SLOT(apply()));

    connect(ui->linearScale, &QRadioButton::toggled, this, &AxisSettingsWidget::axisTypeChanged);
    connect(ui->logScale, &QRadioButton::toggled, this, &AxisSettingsWidget::axisTypeChanged);

    connect(ui->xAxis, &QPushButton::toggled, this, &AxisSettingsWidget::swapAxisData);
    connect(ui->xAxis, &QRadioButton::toggled, this, &AxisSettingsWidget::swapGridData);
}

void AxisSettingsWidget::swapGridData()
{
    ZeAxisName newChosenAxis = ui->xAxis->isChecked() ? ZeAxisName::X : ZeAxisName::Y;

    processUserInput();
    if(newChosenAxis != currentAxis)
        loadSettingsInUi(newChosenAxis);
}

AxisSettingsWidget::~AxisSettingsWidget()
{
    delete ui;
}
