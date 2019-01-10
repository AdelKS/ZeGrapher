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

    ui->scrollArea->setWidget(exportPreview);

    timer.setInterval(4000);
    timer.setSingleShot(true);

    connect(ui->zoomIn, SIGNAL(released()), this, SLOT(zoomIn()));
    connect(ui->zoomOut, SIGNAL(released()), this, SLOT(zoomOut()));

    connect(ui->zoomPercentage, SIGNAL(valueChanged(double)), this, SLOT(onZoomPercentageUserChange()));
    connect(exportPreview, SIGNAL(newZoomValue(double)), this, SLOT(newZoomValue(double)));
    connect(ui->fitSheet, SIGNAL(toggled(bool)), this, SLOT(resizeExportPreview()));
    connect(ui->hundredPercent, SIGNAL(released()), this, SLOT(activateRealSizePreview()));

    connect(ui->vectorFormat, SIGNAL(toggled(bool)), this, SLOT(updateWidgetsVisibility()));

    connect(ui->sheetMargin, SIGNAL(valueChanged(double)), exportPreview, SLOT(setSheetMarginCm(double)));
    connect(ui->imageMargin, SIGNAL(valueChanged(int)), exportPreview, SLOT(setImageMarginPx(int)));

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

    resizeExportPreview();

}

void Export::updateWidgetsVisibility()
{
    if(ui->vectorFormat->isChecked())
    {
        ui->imageSizeLabel->hide();
        ui->imageSizeWiwget->hide();
        ui->imageFigureSizeWidget->hide();

        ui->sheetSizeLabel->show();
        ui->sheetSizeWidget->show();
        ui->sheetFigureSizeWidget->show();
    }
    else
    {
        ui->imageSizeLabel->show();
        ui->imageSizeWiwget->show();
        ui->imageFigureSizeWidget->show();

        ui->sheetSizeLabel->hide();
        ui->sheetSizeWidget->hide();
        ui->sheetFigureSizeWidget->hide();
    }
}

void Export::activateRealSizePreview()
{
    ui->zoomPercentage->setValue(100);
}

void Export::zoomIn()
{
    ui->zoomPercentage->setValue(ui->zoomPercentage->value() + 5);
}

void Export::zoomOut()
{
    ui->zoomPercentage->setValue(ui->zoomPercentage->value() - 5);
}

void Export::newZoomValue(double value)
{
    if(!ui->fitSheet->isChecked())
        resizeExportPreview();
    else
    {
        ui->zoomPercentage->blockSignals(true);
        ui->zoomPercentage->setValue(value * 100);
        ui->zoomPercentage->blockSignals(false);
    }
}

void Export::onZoomPercentageUserChange()
{
    ui->fitSheet->setChecked(false);
    resizeExportPreview();
}


void Export::resizeExportPreview()
{
    if(ui->fitSheet->isChecked())
    {
        if(exportPreview->size() != ui->scrollArea->contentsRect().size())
            exportPreview->resize(ui->scrollArea->contentsRect().size());
    }
    else
    {
        QSize targetSize = exportPreview->getTargetSheetSizePixels();
        targetSize.setHeight(int(double(targetSize.height()) * ui->zoomPercentage->value() / 100));
        targetSize.setWidth(int(double(targetSize.width()) * ui->zoomPercentage->value() / 100));
        exportPreview->resize(targetSize);
    }
}

void Export::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    resizeExportPreview();
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
             exportPreview->exportPDF(fileName, ui->customSize->isChecked() ? SheetSizeType::CUSTOM : SheetSizeType::NORMALISED);

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
    ui->figureHeight->blockSignals(true);
    ui->figureWidth->blockSignals(true);

    ui->figureHeight->setValue(sizeCm.height());
    ui->figureWidth->setValue(sizeCm.width());

    ui->figureHeight->blockSignals(false);
    ui->figureWidth->blockSignals(false);
}

void Export::onFigureSizeChange()
{
    exportPreview->setFigureSizeCm(QSizeF(ui->figureWidth->value(), ui->figureHeight->value()));
    exportPreview->update();
}

void Export::onSheetSizeChange()
{
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

    ui->sheetHeight->blockSignals(true);
    ui->sheetWidth->blockSignals(true);

    ui->sheetWidth->setValue(sheetSize.width());
    ui->sheetHeight->setValue(sheetSize.height());

    ui->sheetHeight->blockSignals(false);
    ui->sheetWidth->blockSignals(false);



    exportPreview->setSheetSizeCm(sheetSize);

    exportPreview->update();

    constrainFigureSizeWidgets(); // should come after updating
}

void Export::swapSheetHeightAndWidth()
{
    ui->sheetHeight->blockSignals(true);
    ui->sheetWidth->blockSignals(true);

    double tmp = ui->sheetHeight->value();
    ui->sheetHeight->setValue(ui->sheetWidth->value());
    ui->sheetWidth->setValue(tmp);

    ui->orientationSelector->setCurrentIndex((ui->orientationSelector->currentIndex() + 1) % 2);

    ui->sheetHeight->blockSignals(false);
    ui->sheetWidth->blockSignals(false);

    onSheetSizeChange();

}



Export::~Export()
{
    delete ui;
}
