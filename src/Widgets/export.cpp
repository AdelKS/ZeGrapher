#include "export.h"
#include "ui_export.h"

Export::Export(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Export)
{
    ui->setupUi(this);

    timer.setSingleShot(true);
    timer.setInterval(2000);

    makeConnects();
}

void Export::makeConnects()
{
    connect(ui->exportButton, SIGNAL(released()), this, SLOT(exportGraph()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(enableExportButton()));
    connect(ui->chooseLocation, SIGNAL(released()), this, SLOT(getFileName()));
    connect(ui->imageFormatSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFileNameExtension()));
    connect(ui->scalableFormatSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFileNameExtension()));
    connect(ui->imageFormat, SIGNAL(toggled(bool)), this, SLOT(updateFileNameExtension()));
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
    else extension = "." + ui->imageFormatSelection->currentText().toLower();

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
    {
        QMessageBox::critical(this, tr("Unspecified export file"), tr("A file name needs to be "
                                                                       "specified along with a destination folder. "
                                                                       "Please specify them then try again."));
        return;
    }

    information.setExportFileName(fileName + extension);

    ui->exportButton->setEnabled(false);
    timer.start();

    emit exportGraphSignal();
}


Export::~Export()
{
    delete ui;
}
