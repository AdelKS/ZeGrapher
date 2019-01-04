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
    exportPreview = new ExportPreview(QSizeF(ui->sheetWidth->value(), ui->sheetHeight->value()), info);
    ui->sheetSizeWidget->setEnabled(false);

    onOrientationChange();
    onSheetSizeChange();

    setWindowFlags(Qt::Window);

    setWindowTitle(tr("Export"));

    orthonormal = false;

    ui->scrollArea->setWidget(exportPreview);

    timer.setInterval(4000);
    timer.setSingleShot(true);


    connect(ui->scalingFactor, SIGNAL(valueChanged(double)), exportPreview, SLOT(setScale(double)));
    connect(ui->orientationSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(onOrientationChange()));
    connect(ui->canvasHeight, SIGNAL(valueChanged(double)), this, SLOT(onCanvasSizeChange()));
    connect(ui->canvasWidth, SIGNAL(valueChanged(double)), this, SLOT(onCanvasSizeChange()));
    connect(exportPreview, SIGNAL(newCanvasSizeCm(QSizeF)), this, SLOT(setCanvasSizeCm(QSizeF)));
    connect(ui->exportButton, SIGNAL(released()), this, SLOT(exportGraph()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(enableExportButton()));
    connect(ui->chooseLocation, SIGNAL(released()), this, SLOT(getFileName()));

    connect(ui->A3, SIGNAL(clicked()), this, SLOT(onSheetSizeChange()));
    connect(ui->A4, SIGNAL(clicked()), this, SLOT(onSheetSizeChange()));
    connect(ui->A5, SIGNAL(clicked()), this, SLOT(onSheetSizeChange()));
    connect(ui->customSize, SIGNAL(clicked()), this, SLOT(onSheetSizeChange()));
    connect(ui->orientationSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(onSheetSizeChange()));

    connect(ui->legendBox, SIGNAL(toggled(bool)), exportPreview, SLOT(setLegendState(bool)));
    connect(ui->legendSize, SIGNAL(valueChanged(int)), exportPreview, SLOT(setlegendFontSize(int)));
    connect(ui->xLegend, SIGNAL(textChanged(QString)), exportPreview, SLOT(setXaxisLegend(QString)));
    connect(ui->yLegend, SIGNAL(textChanged(QString)), exportPreview, SLOT(setYaxisLegend(QString)));
    connect(ui->bold, SIGNAL(toggled(bool)), exportPreview, SLOT(setBold(bool)));
    connect(ui->italic, SIGNAL(toggled(bool)), exportPreview, SLOT(setItalic(bool)));
    connect(ui->underline, SIGNAL(toggled(bool)), exportPreview, SLOT(setUnderline(bool)));
    connect(ui->numPrec, SIGNAL(valueChanged(int)), exportPreview, SLOT(setNumPrec(int)));

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


void Export::enableExportButton()
{
    ui->exportButton->setEnabled(true);
}

void Export::exportGraph()
{
    ui->exportButton->setEnabled(false);
    timer.start();

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

void Export::setCanvasSizeCm(QSizeF sizeCm)
{
    ui->canvasHeight->setValue(sizeCm.height());
    ui->canvasWidth->setValue(sizeCm.width());
}

void Export::onCanvasSizeChange()
{
    // transmit to exportPreview
}

void Export::onSheetSizeChange()
{
    QPageLayout layout;

    if(ui->orientationSelector->currentIndex() == 0)
        layout.setOrientation(QPageLayout::Landscape);
    else layout.setOrientation(QPageLayout::Portrait);

    QSizeF size;

    if(ui->A3->isChecked())
    {
        layout.setPageSize(QPageSize(QPageSize::A3));
        QRectF rect = layout.fullRect(QPageLayout::Millimeter);
        size.setWidth(rect.width() / 10);
        size.setHeight(rect.height() / 10);
    }
    else if(ui->A4->isChecked())
    {
        layout.setPageSize(QPageSize(QPageSize::A4));
        QRectF rect = layout.fullRect(QPageLayout::Millimeter);
        size.setWidth(rect.width() / 10);
        size.setHeight(rect.height() / 10);
    }
    else if(ui->A5->isChecked())
    {
        layout.setPageSize(QPageSize(QPageSize::A5));
        QRectF rect = layout.fullRect(QPageLayout::Millimeter);
        size.setWidth(rect.width() / 10);
        size.setHeight(rect.height() / 10);
    }
    else
    {
        size = QSizeF(ui->sheetWidth->value(), ui->sheetHeight->value());
    }

    ui->sheetWidth->setValue(size.width());
    ui->sheetHeight->setValue(size.height());

    qDebug() << "size update!";

    // change of maximums in cm and minimum in pixels
    exportPreview->setSheetSizeCm(size);
}

void Export::onOrientationChange()
{
    if(ui->orientationSelector->currentIndex() == 0)
    {
        exportPreview->setOrientation(QPageLayout::Landscape);
    }
    else
    {
        exportPreview->setOrientation(QPageLayout::Portrait);
    }
}



Export::~Export()
{
    delete ui;
}
