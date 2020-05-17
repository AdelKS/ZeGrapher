#include "export.h"
#include "ui_export.h"

Export::Export(Information *information, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Export)
{
    ui->setupUi(this);

    this->information = information;

    exportFormatChanged();

    makeConnects();
}

void Export::exportFormatChanged()
{
    const ZeSizeSettings &sizeSettings = information->getGraphSettings().sizeSettings;

    if(sizeSettings.sizeUnit == ZeSizeSettings::CENTIMETER)
    {
        ui->imageWidget->setEnabled(false);
        ui->imageFormat->setEnabled(false);

        ui->vectorFormat->setChecked(true);
        ui->vectorFormat->setEnabled(true);
        ui->vectorWidget->setEnabled(true);
    }
    else
    {
        ui->imageWidget->setEnabled(true);
        ui->imageFormat->setEnabled(true);
        ui->imageFormat->setChecked(true);

        ui->vectorFormat->setEnabled(false);
        ui->vectorWidget->setEnabled(false);
    }

    updateFileNameExtension();
}

void Export::makeConnects()
{
    connect(ui->exportButton, SIGNAL(released()), this, SLOT(exportGraph()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(enableExportButton()));
    connect(ui->chooseLocation, SIGNAL(released()), this, SLOT(getFileName()));
}

void Export::getFileName()
{
    fileName = QFileDialog::getSaveFileName(this, tr("Enter file name in the selected folder"));

    updateFileNameExtension();
}

void Export::updateFileNameExtension()
{
    if(ui->vectorFormat->isChecked())
        extension = "." + ui->scalableFormatSelection->currentText().toLower();
    else extension = "." + ui->scalableFormatSelection->currentText().toLower();

    if(!fileName.isEmpty())
    {
        ui->fileName->setText(fileName + extension);
    }
}

void Export::enableExportButton()
{
    ui->exportButton->setEnabled(true);
}

void Export::exportGraph()
{
    if(fileName.isEmpty())
        QMessageBox::critical(this, tr("Unspecified export file"), tr("A file name needs to be "
                                                                       "specified along with a destination folder.\n\n"
                                                                       "Please specify them then try again."));
    information->setExportFileName(fileName + extension);

    ui->exportButton->setEnabled(false);
    timer.start();

    emit exportGraphSignal();
}


Export::~Export()
{
    delete ui;
}
