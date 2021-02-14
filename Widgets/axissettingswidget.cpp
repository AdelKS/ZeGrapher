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
    currentAxis = ZeAxisName::X;
    loadAxisSettingsInUi(axesSettings.x);

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
    ui->tickRelSpacing->setValue(settings.tickRelSpacing);

    if(settings.axisType == ZeAxisType::LINEAR)
    {
        constantMultiplierLineEdit->setText(settings.linSettings.constantMultiplierStr);
    }
    else
    {
        baseLineEdit->setText(settings.logSettings.baseStr);

        constantMultiplierLineEdit->setText(settings.logSettings.constantMultiplierStr);
    }
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

    if(currentAxis == ZeAxisName::X)
        axesSettings.x = axisSettings;
    else axesSettings.y = axisSettings;

    Ze1DGridSettings &settingsToUpdate = currentAxis == ZeAxisName::X ? gridSettings.alongX : gridSettings.alongY;

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
    ZeAxisSettings defaultSettings;

    defaultSettings.axisType = ZeAxisType::LINEAR;
    defaultSettings.color = Qt::black;
    defaultSettings.lineWidth = 0.1;

    defaultSettings.logSettings.base = 10;
    defaultSettings.logSettings.baseStr = "10";
    defaultSettings.logSettings.constantMultiplier = 1;
    defaultSettings.logSettings.constantMultiplierStr = "1";

    defaultSettings.linSettings.constantMultiplier = 1;
    defaultSettings.logSettings.constantMultiplierStr = "1";
    defaultSettings.linSettings.maxDigitsNum = 5;
    defaultSettings.tickRelSpacing = 0;

    axesSettings.x = axesSettings.y = defaultSettings;

    // Grid settings

    ui->gridGroup->setChecked(true);

    Ze1DGridSettings defaultGridSettings;

    defaultGridSettings.showGrid = true;
    defaultGridSettings.showSubGrid = false;
    defaultGridSettings.showSubgridRelativeCoordinates = false;
    defaultGridSettings.subgridSubDivs = 1;
    defaultGridSettings.gridColor = Qt::gray;
    // TODO: update grid default colors
    defaultGridSettings.subgridColor = Qt::gray;

    // TODO: fine tune grid and subgrid widths, check what antialiasing does.
    defaultGridSettings.gridLineWidth = 1;
    defaultGridSettings.subgridLineWidth = 0.5;

    gridSettings.alongX = defaultGridSettings;
    gridSettings.alongY = defaultGridSettings;

    loadGridSettingsInUi(ZeAxisName::X);

    emit settingsUpdated();
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
        loadGridSettingsInUi(newChosenAxis);
}

void AxisSettingsWidget::loadGridSettingsInUi(ZeAxisName name)
{
    currentAxis = name;

    const Ze1DGridSettings &unidimSettings = name == ZeAxisName::X ? gridSettings.alongX : gridSettings.alongY;
    loadGridSettingsInUi(unidimSettings);
}

void AxisSettingsWidget::loadGridSettingsInUi(const Ze1DGridSettings &unidimGridSettings)
{
    ui->gridGroup->setChecked(unidimGridSettings.showGrid);
    ui->subGridGroup->setChecked(unidimGridSettings.showSubGrid);
    ui->showSubGridRelativePos->setChecked(unidimGridSettings.showSubgridRelativeCoordinates);
    ui->subgridDivs->setValue(unidimGridSettings.subgridSubDivs);
    gridColorButton->setColor(unidimGridSettings.gridColor);
    subgridColorButton->setColor(unidimGridSettings.subgridColor);
    ui->gridLineWidth->setValue(unidimGridSettings.gridLineWidth);
    ui->subGridLineWidth->setValue(unidimGridSettings.subgridLineWidth);
}

AxisSettingsWidget::~AxisSettingsWidget()
{
    delete ui;
}
