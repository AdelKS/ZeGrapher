#include "zegridsettingswidget.h"
#include "ui_zegridsettingswidget.h"

ZeGridSettingsWidget::ZeGridSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ZeGridSettingsWidget)
{
    ui->setupUi(this);

    gridColorButton = new QColorButton(Qt::gray);
    ui->gridFormLayout->addRow(tr("Color"), gridColorButton);

    subgridColorButton = new QColorButton(Qt::gray);
    ui->subgridFormLayout->addRow(tr("Color"), subgridColorButton);

    makeConnects();

    loadDefaults();
}

ZeGridSettings ZeGridSettingsWidget::getSettings()
{
    return gridSettings;
}

void ZeGridSettingsWidget::loadDefaults()
{
    ui->xGrid->setChecked(true);

    Ze1DGridSettings defaultSettings;

    defaultSettings.showGrid = true;
    defaultSettings.showSubGrid = false;
    defaultSettings.showSubgridRelativeCoordinates = false;
    defaultSettings.subgridSubDivs = 1;
    defaultSettings.gridColor = Qt::gray;
    // TODO: update grid default colors
    defaultSettings.subgridColor = Qt::gray;

    // TODO: fine tune grid and subgrid widths, check what antialiasing does.
    defaultSettings.gridLineWidth = 1;
    defaultSettings.subgridLineWidth = 0.5;

    gridSettings.alongX = defaultSettings;
    gridSettings.alongY = defaultSettings;

    loadGridSettingsInUi(ZeAxisName::X);
}

void ZeGridSettingsWidget::makeConnects()
{
    connect(gridColorButton, &QColorButton::colorChanged, this, &ZeGridSettingsWidget::apply);
    connect(subgridColorButton, &QColorButton::colorChanged, this, &ZeGridSettingsWidget::apply);

    connect(ui->xGrid, &QRadioButton::toggled, this, &ZeGridSettingsWidget::swapGridData);
    connect(ui->yGrid, &QRadioButton::toggled, this, &ZeGridSettingsWidget::swapGridData);

    connect(ui->showGrid, &QCheckBox::toggled, this, &ZeGridSettingsWidget::apply);

    connect(ui->showSubgrid, &QCheckBox::toggled, this, &ZeGridSettingsWidget::apply);
    connect(ui->showGrid, &QCheckBox::toggled, this, &ZeGridSettingsWidget::apply);
    connect(ui->showGrid, &QCheckBox::toggled, this, &ZeGridSettingsWidget::apply);
    connect(ui->showGrid, &QCheckBox::toggled, this, &ZeGridSettingsWidget::apply);

    connect(ui->gridLineWidth, SIGNAL(valueChanged()), this, SLOT(apply()));
    connect(ui->subgridDivs, SIGNAL(valueChanged()), this, SLOT(apply()));
}

void ZeGridSettingsWidget::swapGridData()
{
    ZeAxisName newChosenAxis = ui->xGrid->isChecked() ? ZeAxisName::X : ZeAxisName::Y;
    apply();
    if(newChosenAxis != currentAxis)
        loadGridSettingsInUi(newChosenAxis);
}

void ZeGridSettingsWidget::apply()
{
    Ze1DGridSettings &settingsToUpdate = currentAxis == ZeAxisName::X ? gridSettings.alongX : gridSettings.alongY;

    settingsToUpdate.showGrid = ui->showGrid->isChecked();
    settingsToUpdate.showSubGrid = ui->showSubgrid->isChecked();
    settingsToUpdate.showSubgridRelativeCoordinates = ui->showSubGridRelativePos->isChecked();
    settingsToUpdate.subgridSubDivs = ui->subgridDivs->value();
    settingsToUpdate.gridColor = gridColorButton->getCurrentColor();
    settingsToUpdate.subgridColor = subgridColorButton->getCurrentColor();
    settingsToUpdate.gridLineWidth = ui->gridLineWidth->value();
    settingsToUpdate.subgridLineWidth = ui->subGridLineWidth->value();

    emit settingsUpdated();
}

void ZeGridSettingsWidget::loadGridSettingsInUi(ZeAxisName name)
{
    currentAxis = name;

    if(name == ZeAxisName::X)
        ui->xGrid->setChecked(true);
    else ui->yGrid->setChecked(true);

    const Ze1DGridSettings &unidimSettings = name == ZeAxisName::X ? gridSettings.alongX : gridSettings.alongY;
    loadGridSettingsInUi(unidimSettings);
}

void ZeGridSettingsWidget::loadGridSettingsInUi(const Ze1DGridSettings &unidimGridSettings)
{
    ui->showGrid->setChecked(unidimGridSettings.showGrid);
    ui->showSubgrid->setChecked(unidimGridSettings.showSubGrid);
    ui->showSubGridRelativePos->setChecked(unidimGridSettings.showSubgridRelativeCoordinates);
    ui->subgridDivs->setValue(unidimGridSettings.subgridSubDivs);
    gridColorButton->setColor(unidimGridSettings.gridColor);
    subgridColorButton->setColor(unidimGridSettings.subgridColor);
    ui->gridLineWidth->setValue(unidimGridSettings.gridLineWidth);
    ui->subGridLineWidth->setValue(unidimGridSettings.subgridLineWidth);
}

ZeGridSettingsWidget::~ZeGridSettingsWidget()
{
    delete ui;
}
