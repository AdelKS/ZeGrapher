#include "graphsettings.h"
#include "ui_graphsettings.h"
#include "information.h"

GraphSettings::GraphSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphSettings)
{
    ui->setupUi(this);

    backgroundColorButton = new QColorButton(Qt::white);
    ui->formLayout->addRow(tr("Background color:"), backgroundColorButton);

    defaultColorButton = new QColorButton(Qt::black);
    ui->formLayout->addRow(tr("Curve default color:"), defaultColorButton);

    graphSettings.backgroundColor = backgroundColorButton->getCurrentColor();
    graphSettings.defaultColor = defaultColorButton->getCurrentColor();

    loadSettingsFromDisk();

    makeConnects();
}

void GraphSettings::makeConnects()
{
    connect(ui->distanceWidget, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->thicknessWidget, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->graphFontSize, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->graphFont, SIGNAL(currentFontChanged(QFont)), this, SLOT(apply()));
    connect(ui->smoothing, SIGNAL(toggled(bool)), this, SLOT(apply()));
    connect(backgroundColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));

    connect(defaultColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));
}


void GraphSettings::resetToDefaults()
{
    backgroundColorButton->setColor(Qt::white);
    defaultColorButton->setColor(Qt::black);
    ui->distanceWidget->setValue(4);
    ui->graphFontSize->setValue(11);
    ui->thicknessWidget->setValue(1);
    ui->smoothing->setChecked(true);

    apply();
}

void GraphSettings::apply()
{
    processUserInput();
    information.setGraphSettings(graphSettings);
}

const ZeGraphSettings& GraphSettings::getSettings() const
{
    return graphSettings;
}

void GraphSettings::processUserInput()
{
    graphSettings.backgroundColor = backgroundColorButton->getCurrentColor();

    double dist = ui->distanceWidget->value();

    graphSettings.smoothing = ui->smoothing->isChecked();
    graphSettings.distanceBetweenPoints = pow(2, 2 - dist/2);
    graphSettings.curvesThickness = ui->thicknessWidget->value();

    graphSettings.defaultColor = defaultColorButton->getCurrentColor();

    QFont font(ui->graphFont->currentFont());
    font.setPixelSize(ui->graphFontSize->value());

    graphSettings.graphFont = font;
}

void GraphSettings::loadSettingsFromDisk()
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

    apply();
}

void GraphSettings::saveSettingsToDisk()
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
}

GraphSettings::~GraphSettings()
{
    saveSettingsToDisk();
    delete ui;
}
