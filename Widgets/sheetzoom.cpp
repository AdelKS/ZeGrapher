#include "sheetzoom.h"
#include "ui_sheetzoom.h"

SheetZoom::SheetZoom(Information *information, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SheetZoom)
{
    ui->setupUi(this);
    this->information = information;

    connect(ui->zoomIn, &QPushButton::released, this, &SheetZoom::zoomIn);
    connect(ui->zoomOut, &QPushButton::released, this, &SheetZoom::zoomOut);
    connect(ui->zoomPercentage, SIGNAL(valueChanged(double)), this, SLOT(onZoomValueChange()));

    updateZoomSettings();
}

void SheetZoom::updateZoomSettings()
{
    zoomSettings.zoom = ui->zoomPercentage->value();
    zoomSettings.zoomingType = ui->fitSheet->isChecked() ? ZeZoomSettings::FITSHEET : ZeZoomSettings::CUSTOM;

    information->setGraphZoomSettings(zoomSettings);
}

void SheetZoom::activateRealSizePreview()
{
    ui->fitSheet->setChecked(false);
    ui->zoomPercentage->setValue(100);

    updateZoomSettings();
}

void SheetZoom::zoomIn()
{
    ui->zoomPercentage->setValue(ui->zoomPercentage->value() + 5);
}

void SheetZoom::zoomOut()
{
    ui->zoomPercentage->setValue(ui->zoomPercentage->value() - 5);
    // Method onZoomValueChange will be automatically called with signal slot mechanism
}

void SheetZoom::onExternalZoomSettingsChange()
{
    zoomSettings = information->getGraphZoomSettings();

    const QSignalBlocker blocker(ui->fitSheet);
    const QSignalBlocker blocker2(ui->zoomPercentage);


    ui->fitSheet->setChecked(zoomSettings.zoomingType == ZeZoomSettings::FITSHEET);
    ui->zoomPercentage->setValue(zoomSettings.zoom);
}

void SheetZoom::onZoomValueChange()
{
    ui->fitSheet->setChecked(false);

    updateZoomSettings();
}


SheetZoom::~SheetZoom()
{
    delete ui;
}
