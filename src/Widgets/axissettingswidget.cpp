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
#include "numberlineedit.h"
#include "qcolorbutton.h"
#include "information.h"

#include "axissettingswidget.h"
#include "ui_axissettingswidget.h"

/* TODO:
 * - Save and load defaults from disk
 * - Base should be > 1 (Strictly)
 **/

AxisSettingsWidget::AxisSettingsWidget(QWidget *parent): QWidget(parent), ui(new Ui::AxisSettingsWidget)
{
    ui->setupUi(this);

    loadSettings(LoadDirection::FROM_DISK);
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

AxisSettingsWidget::SelectedAxis AxisSettingsWidget::getSelectedAxis() const
{
    return ui->xAxis->isChecked() ? SelectedAxis::X : (ui->yAxis->isChecked() ? SelectedAxis::Y : SelectedAxis::BOTH);
}

void AxisSettingsWidget::swapAxisData()
{
    // process user input before switching to the the other axis
    processUserInput();

    SelectedAxis newChosenAxis = getSelectedAxis();

    if(newChosenAxis != currentAxis)
        loadAxisSettingsInUI();
}

void AxisSettingsWidget::loadAxisSettingsInUI()
{
    ui->axisColor->setColor(axesSettings.color);
    ui->axisLineWidth->setValue(axesSettings.lineWidth);

    currentAxis = getSelectedAxis();
    if (currentAxis == SelectedAxis::BOTH)
        return;

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

    ZeAxisSettings axisSettings;

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

    Ze1DGridSettings axisGridSettings;

    axisGridSettings.showGrid = ui->gridGroup->isChecked();
    axisGridSettings.showSubGrid = ui->subGridGroup->isChecked();
    axisGridSettings.showSubgridRelativeCoordinates = ui->showSubGridRelativePos->isChecked();
    axisGridSettings.subgridSubDivs = ui->subgridDivs->value();
    axisGridSettings.gridColor = ui->gridColor->getCurrentColor();
    axisGridSettings.subgridColor = ui->subgridColor->getCurrentColor();
    axisGridSettings.gridLineWidth = ui->gridLineWidth->value();
    axisGridSettings.subgridLineWidth = ui->subGridLineWidth->value();

    SelectedAxis currentAxis = getSelectedAxis();
    if (currentAxis == SelectedAxis::BOTH or currentAxis == SelectedAxis::X)
    {
        axesSettings.x = axisSettings;
        gridSettings.x = axisGridSettings;
    }
    if (currentAxis == SelectedAxis::BOTH or currentAxis == SelectedAxis::Y)
    {
        axesSettings.y = axisSettings;
        gridSettings.y = axisGridSettings;
    }
}

void AxisSettingsWidget::loadSettings(const LoadDirection direction)
{
    QSettings settings;

    auto load = [&](auto &internal, const QString name, const auto& default_val)
    {
        using InternalType = std::remove_cvref_t<decltype(internal)>;
        if (direction == LoadDirection::FROM_DISK)
            internal = settings.value(name, QVariant::fromValue(default_val)).template value<InternalType>();
        else settings.setValue(name, QVariant::fromValue(internal));
    };

    load(axesSettings.color, "graph/axes/color", QColor(Qt::black));
    load(axesSettings.lineWidth, "graph/axes/line_width", 1.1);

    auto loadAxisSettings = [&](ZeAxisSettings& axis, QChar axisName)
    {
        load(axis.axisType, "graph/axes/" + axisName + "/axis_type", ZeAxisType::LINEAR);
        load(axis.tickRelSpacing, "graph/axes/x/tick_relative_spacing", 0);

        load(axis.logSettings.base, "graph/axes" + axisName + "log/base", 10);
        load(axis.logSettings.baseStr, "graph/axes" + axisName + "log/base_str", QString("10"));
        load(axis.logSettings.constantMultiplier, "graph/axes" + axisName + "log/constant_multiplier", 1);
        load(axis.logSettings.constantMultiplierStr, "graph/axes" + axisName + "log/constant_multiplier_str", QString(""));

        load(axis.linSettings.maxDigitsNum, "graph/axes" + axisName + "linear/max_digits_num", 4);
        load(axis.linSettings.constantMultiplier, "graph/axes" + axisName + "linear/constant_multiplier" , 1);
        load(axis.linSettings.constantMultiplierStr, "graph/axes" + axisName + "linear/constant_multiplier_str" , QString(""));
    };

    loadAxisSettings(axesSettings.x, 'x');
    loadAxisSettings(axesSettings.y, 'y');

    auto loadGridSettings = [&](Ze1DGridSettings& grid, QChar axisName)
    {
        load(grid.showGrid, "graph/grid/" + axisName + "/show_grid", true);
        load(grid.showSubGrid, "graph/grid/" + axisName + "/show_subgrid", false);
        load(grid.showSubgridRelativeCoordinates, "graph/grid/" + axisName + "/show_subgrid_rel_coord", false);
        load(grid.subgridSubDivs, "graph/grid/" + axisName + "/subgrid_subdivs", 1);

        // TODO: update grid default colors
        load(grid.gridColor, "graph/grid/" + axisName + "/grid_color", QColor(Qt::gray));
        load(grid.subgridColor, "graph/grid/" + axisName + "/subgrid_color", QColor(Qt::gray));

        // TODO: fine tune grid and subgrid widths, check what antialiasing does.
        load(grid.gridLineWidth, "graph/grid/" + axisName + "/grid_linewidth", 0.6);
        load(grid.subgridLineWidth, "graph/grid/" + axisName + "/subgrid_linewidth", 0.3);
    };

    // Grid settings

    ui->gridGroup->setChecked(true);

    loadGridSettings(gridSettings.x, 'x');
    loadGridSettings(gridSettings.y, 'y');

    if (direction == LoadDirection::FROM_DISK)
    {
        information.setAxesSettings(axesSettings);
        information.setGridSettings(gridSettings);
    }
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
    connect(ui->yAxis, &QRadioButton::toggled, this, &AxisSettingsWidget::swapGridData);
}

void AxisSettingsWidget::swapGridData()
{
    SelectedAxis newChosenAxis = getSelectedAxis();

    processUserInput();
    if (newChosenAxis != currentAxis)
        loadAxisSettingsInUI();
}

void AxisSettingsWidget::saveSettingsToDisk()
{
    loadSettings(LoadDirection::TO_DISK);
}

AxisSettingsWidget::~AxisSettingsWidget()
{
    saveSettingsToDisk();
    delete ui;
}
