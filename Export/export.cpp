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

    setWindowFlags(Qt::Window);

    setWindowTitle(tr("Export"));

    orthonormal = false;
    disableOnSheetSizeChangeSlot = false;
    disableOnFigureSizeChange = false;

    ui->scrollArea->setWidget(exportPreview);

    timer.setInterval(4000);
    timer.setSingleShot(true);


    connect(ui->scalingFactor, SIGNAL(valueChanged(double)), exportPreview, SLOT(setScale(double)));
    connect(ui->figureHeight, SIGNAL(valueChanged(double)), this, SLOT(onFigureSizeChange()));
    connect(ui->figureWidth, SIGNAL(valueChanged(double)), this, SLOT(onFigureSizeChange()));
    connect(exportPreview, SIGNAL(newFigureSizeCm(QSizeF)), this, SLOT(setFigureSizeCm(QSizeF)));
    connect(ui->exportButton, SIGNAL(released()), this, SLOT(exportGraph()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(enableExportButton()));
    connect(ui->chooseLocation, SIGNAL(released()), this, SLOT(getFileName()));

    connect(ui->A3, SIGNAL(clicked()), this, SLOT(onSheetSizeChange()));
    connect(ui->A4, SIGNAL(clicked()), this, SLOT(onSheetSizeChange()));
    connect(ui->A5, SIGNAL(clicked()), this, SLOT(onSheetSizeChange()));
    connect(ui->sheetHeight, SIGNAL(valueChanged(double)), this, SLOT(onSheetSizeChange()));
    connect(ui->sheetWidth, SIGNAL(valueChanged(double)), this, SLOT(onSheetSizeChange()));
    connect(ui->swapButton, SIGNAL(released()), this, SLOT(swapSheetHeightAndWidth()));
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

void Export::constrainFigureSizeWidgets()
{
    ui->figureHeight->setMaximum(ui->sheetHeight->value() * (1 - exportPreview->getMinMargin()));
    ui->figureHeight->setMinimum(ui->sheetHeight->value() * exportPreview->getMinFigureSize());

    ui->figureWidth->setMaximum(ui->sheetWidth->value() * (1 - exportPreview->getMinMargin()));
    ui->figureWidth->setMinimum(ui->sheetWidth->value() * exportPreview->getMinFigureSize());
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

void Export::setFigureSizeCm(QSizeF sizeCm)
{
    disableOnFigureSizeChange = true;
    ui->figureHeight->setValue(sizeCm.height());
    ui->figureWidth->setValue(sizeCm.width());
    disableOnFigureSizeChange = false;
}

void Export::onFigureSizeChange()
{
    if(disableOnFigureSizeChange)
        return;

    exportPreview->setFigureSizeCm(QSizeF(ui->figureWidth->value(), ui->figureHeight->value()));
    exportPreview->update();
}

void Export::onSheetSizeChange()
{
    if(disableOnSheetSizeChangeSlot)
        return;

    disableOnSheetSizeChangeSlot = true;

    QPageLayout layout;

    if(ui->orientationSelector->currentIndex() == 0)
    {
        layout.setOrientation(QPageLayout::Landscape);
        exportPreview->setOrientation(QPageLayout::Landscape);
    }
    else
    {
        layout.setOrientation(QPageLayout::Portrait);
        exportPreview->setOrientation(QPageLayout::Portrait);
    }

    QSizeF sheetSize;

    if(ui->A3->isChecked())
    {
        layout.setPageSize(QPageSize(QPageSize::A3));
        QRectF rect = layout.fullRect(QPageLayout::Millimeter);
        sheetSize.setWidth(rect.width() / 10);
        sheetSize.setHeight(rect.height() / 10);
    }
    else if(ui->A4->isChecked())
    {
        layout.setPageSize(QPageSize(QPageSize::A4));
        QRectF rect = layout.fullRect(QPageLayout::Millimeter);
        sheetSize.setWidth(rect.width() / 10);
        sheetSize.setHeight(rect.height() / 10);
    }
    else if(ui->A5->isChecked())
    {
        layout.setPageSize(QPageSize(QPageSize::A5));
        QRectF rect = layout.fullRect(QPageLayout::Millimeter);
        sheetSize.setWidth(rect.width() / 10);
        sheetSize.setHeight(rect.height() / 10);
    }
    else
    {
        sheetSize = QSizeF(ui->sheetWidth->value(), ui->sheetHeight->value());
    }

    ui->sheetWidth->setValue(sheetSize.width());
    ui->sheetHeight->setValue(sheetSize.height());

    exportPreview->setSheetSizeCm(sheetSize);

    exportPreview->update();

    constrainFigureSizeWidgets(); // should come after updating

    disableOnSheetSizeChangeSlot = false;
}

void Export::swapSheetHeightAndWidth()
{
    disableOnSheetSizeChangeSlot = true;

    double tmp = ui->sheetHeight->value();
    ui->sheetHeight->setValue(ui->sheetWidth->value());

    disableOnSheetSizeChangeSlot = false;

    ui->sheetWidth->setValue(tmp);
}



Export::~Export()
{
    delete ui;
}
