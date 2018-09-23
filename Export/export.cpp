/****************************************************************************
**  Copyright (c) 2016, Adel Kara Slimane <adel.ks@zegrapher.com>
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


#include "Export/export.h"
#include "ui_export.h"

Export::Export(Information *info, QWidget *parent) : QWidget(parent), ui(new Ui::Export)
{
    ui->setupUi(this);
    exportPreview = new ExportPreview(info);

    orientationChanged(ui->orientationSelector->currentIndex());

    setWindowFlags(Qt::Window);

    setWindowTitle(tr("Print"));

    orthonormal = false;

    ui->scrollArea->setWidget(exportPreview);

    timer.setInterval(4000);
    timer.setSingleShot(true);

    printersList = QPrinterInfo::availablePrinters();

    for(short i = 0; i < printersList.size(); i++)
        ui->printerSelector->insertItem(i, printersList[i].printerName());

    connect(ui->exportGroup, SIGNAL(toggled(bool)), this, SLOT(exportGroupStateChanged(bool)));
    connect(ui->scalingFactor, SIGNAL(valueChanged(double)), exportPreview, SLOT(setScale(double)));
    connect(ui->orientationSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(orientationChanged(int)));
    connect(ui->height, SIGNAL(valueChanged(double)), exportPreview, SLOT(setGraphHeight(double)));
    connect(ui->width, SIGNAL(valueChanged(double)), exportPreview, SLOT(setGraphWidth(double)));
    connect(exportPreview, SIGNAL(newGraphSize(double,double)), this, SLOT(setGraphSize(double,double)));
    connect(ui->exportButton, SIGNAL(released()), this, SLOT(exportGraph()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(enablePrintButton()));
    connect(ui->chooseLocation, SIGNAL(released()), this, SLOT(getFileName()));
    connect(ui->printGroup, SIGNAL(toggled(bool)), this, SLOT(setPrintButtonText(bool)));

    connect(ui->legendBox, SIGNAL(toggled(bool)), exportPreview, SLOT(setLegendState(bool)));
    connect(ui->legendSize, SIGNAL(valueChanged(int)), exportPreview, SLOT(setlegendFontSize(int)));
    connect(ui->xLegend, SIGNAL(textChanged(QString)), exportPreview, SLOT(setXaxisLegend(QString)));
    connect(ui->yLegend, SIGNAL(textChanged(QString)), exportPreview, SLOT(setYaxisLegend(QString)));
    connect(ui->bold, SIGNAL(toggled(bool)), exportPreview, SLOT(setBold(bool)));
    connect(ui->italic, SIGNAL(toggled(bool)), exportPreview, SLOT(setItalic(bool)));
    connect(ui->underline, SIGNAL(toggled(bool)), exportPreview, SLOT(setUnderline(bool)));
    connect(ui->numPrec, SIGNAL(valueChanged(int)), exportPreview, SLOT(setNumPrec(int)));

}

void Export::exportGroupStateChanged(bool state)
{
    ui->printGroup->setChecked(!state);
}

void Export::getFileName()
{
    fileName = QFileDialog::getSaveFileName(this, tr("Enter file name in the selected folder"));
    if(!fileName.isEmpty())
    {
        if(ui->scalableFormat->isChecked())
            fileName.append("." + ui->scalableFormatSelection->currentText().toLower());
        else fileName.append("." + ui->imageFormatSelection->currentText().toLower());

        ui->fileName->setText(fileName);
    }
}

void Export::setPrintButtonText(bool state)
{
    if(state)
        ui->exportButton->setText(tr("Print"));
    else ui->exportButton->setText(tr("Save"));

    ui->exportGroup->setChecked(!state);
}

void Export::enablePrintButton()
{
    ui->exportButton->setEnabled(true);
}

void Export::exportGraph()
{
    bool useColor;
    if(ui->colorType->currentIndex() == 0)
        useColor = true;
    else useColor = false;

    ui->exportButton->setEnabled(false);
    timer.start();

    if(ui->printGroup->isChecked())
    {
        int reponse = QMessageBox::question(this, tr("Please confrim your printing request"), tr("You are about to print, do you want to continue?\n Please check settings before printing."), QMessageBox::Yes | QMessageBox::No);

        if (reponse == QMessageBox::No)
            return;

        if(!printersList.isEmpty())
            exportPreview->setPrinter(printersList[ui->printerSelector->currentIndex()]);
        else return;

         exportPreview->print(ui->numPages->value(), useColor);
    }
    else
    {
         if(ui->scalableFormat->isChecked())
         {
             if(ui->scalableFormatSelection->currentText() == "PDF")
             {
                 if(!fileName.isEmpty())
                    exportPreview->exportPDF(fileName);
                 else QMessageBox::critical(this, tr("Unspecified export file"), tr("A file name needs to be "
                                                                                    "specified along with a destination folder.\n\n"
                                                                                    "Please specify them then try again."));
             }
             else if(ui->scalableFormatSelection->currentText() == "SVG")
             {

             }
         }
         else // image format
         {

         }
    }
}

void Export::setGraphSize(double H, double W)
{
    ui->height->setValue(H);
    ui->width->setValue(W);
}

void Export::orientationChanged(int pos)
{
    if(pos == 1)
    {
        ui->width->setMaximum(20);
        ui->height->setMaximum(28.7);
        exportPreview->setOrientation(QPrinter::Portrait);
        exportPreview->setMinimumSize(210,297);
    }
    else
    {
        ui->width->setMaximum(28.7);
        ui->height->setMaximum(20);
        exportPreview->setOrientation(QPrinter::Landscape);
        exportPreview->setMinimumSize(297,210);
    }

}



Export::~Export()
{
    delete ui;
}
