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


#include "Windows/export.h"
#include "ui_export.h"

Export::Export(Information *info, QWidget *parent) : QWidget(parent), ui(new Ui::Export)
{
    information = info;
    ui->setupUi(this);
    QSizeF sheetSize(ui->sheetWidth->value(), ui->sheetHeight->value());
    QSize imageSize(ui->imageWidth->value(), ui->imageHeight->value());
    ExportType exportType = ui->vectorFormat->isChecked() ? ExportType::VECTORIAL : ExportType::IMAGE;

    exportPreview = new ExportPreview(sheetSize, imageSize, exportType, info);
    exportPreview->setImageMarginPx(ui->imageMargin->value());
    exportPreview->setSheetMarginCm(ui->sheetMargin->value());

    setWindowFlags(Qt::Window);

    setWindowTitle(tr("Export"));

    ui->sheetSizeSubWidget->setEnabled(false);

    orthonormal = false;

    ui->scrollArea->setWidget(exportPreview);

    timer.setInterval(2000);
    timer.setSingleShot(true);

    makeUiConnects();

    exportFormatChanged();
}

void Export::makeUiConnects()
{
    connect(rangeWidget, SIGNAL(graphRangeChanged(GraphRange)), exportPreview, SLOT(setGraphRange(GraphRange)));
    connect(rangeWidget, SIGNAL(graphTickIntervalsChanged(GraphTickIntervals)), exportPreview, SLOT(setGraphTickIntervals(GraphTickIntervals)));

    connect(information, SIGNAL(graphRangeChanged(GraphRange)), this, SLOT(onMainGraphRangeChange(GraphRange)));

    connect(ui->zoomIn, SIGNAL(released()), this, SLOT(zoomIn()));
    connect(ui->zoomOut, SIGNAL(released()), this, SLOT(zoomOut()));

    connect(ui->zoomPercentage, SIGNAL(valueChanged(double)), this, SLOT(onZoomPercentageUserChange()));
    connect(exportPreview, SIGNAL(newZoomValue(double)), this, SLOT(newZoomValue(double)));
    connect(ui->fitSheet, SIGNAL(toggled(bool)), this, SLOT(resizeExportPreview()));
    connect(ui->hundredPercent, SIGNAL(released()), this, SLOT(activateRealSizePreview()));

    connect(ui->vectorFormat, SIGNAL(toggled(bool)), this, SLOT(exportFormatChanged()));

    connect(ui->sheetMargin, SIGNAL(valueChanged(double)), this, SLOT(onSheetMarginChange()));
    connect(ui->imageMargin, SIGNAL(valueChanged(int)), this, SLOT(onImageMarginChange()));

    connect(ui->scalingFactor, SIGNAL(valueChanged(double)), exportPreview, SLOT(setScale(double)));

    connect(ui->sheetFigureHeight, SIGNAL(valueChanged(double)), this, SLOT(onSheetFigureSizeChange()));
    connect(ui->sheetFigureWidth, SIGNAL(valueChanged(double)), this, SLOT(onSheetFigureSizeChange()));

    connect(ui->imageFigureHeight, SIGNAL(valueChanged(int)), this, SLOT(onImageFigureSizeChange()));
    connect(ui->imageFigureWidth, SIGNAL(valueChanged(int)), this, SLOT(onImageFigureSizeChange()));

    connect(exportPreview, SIGNAL(newFigureSizeCm(QSizeF)), this, SLOT(setSheetFigureSizeCm(QSizeF)));
    connect(exportPreview, SIGNAL(newFigureSizePx(QSize)), this, SLOT(setImageFigureSizePx(QSize)));

    connect(ui->sheetHeight, SIGNAL(valueChanged(double)), this, SLOT(onSheetSizeChange()));
    connect(ui->sheetWidth, SIGNAL(valueChanged(double)), this, SLOT(onSheetSizeChange()));

    connect(ui->imageHeight, SIGNAL(valueChanged(int)), this, SLOT(onImageSizeChange()));
    connect(ui->imageWidth, SIGNAL(valueChanged(int)), this, SLOT(onImageSizeChange()));

    connect(ui->exportButton, SIGNAL(released()), this, SLOT(exportGraph()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(enableExportButton()));
    connect(ui->chooseLocation, SIGNAL(released()), this, SLOT(getFileName()));

    connect(ui->A3, SIGNAL(clicked()), this, SLOT(onSheetSizeChange()));
    connect(ui->A4, SIGNAL(clicked()), this, SLOT(onSheetSizeChange()));
    connect(ui->A5, SIGNAL(clicked()), this, SLOT(onSheetSizeChange()));

    connect(ui->sheetSizeSwap, SIGNAL(released()), this, SLOT(swapSheetHeightAndWidth()));
    connect(ui->imageSizeSwap, SIGNAL(released()), this, SLOT(swapImageHeightAndWidth()));

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

void Export::exportFormatChanged()
{
    // showing and hiding the necessary widgets

    if(ui->vectorFormat->isChecked())
    {
        exportPreview->setExportType(ExportType::VECTORIAL);

        ui->imageSizeLabel->hide();
        ui->imageSizeWidget->hide();
        ui->imageFigureSizeWidget->hide();

        ui->sheetSizeLabel->show();
        ui->sheetSizeWidget->show();
        ui->sheetFigureSizeWidget->show();
    }
    else
    {
        exportPreview->setExportType(ExportType::IMAGE);

        ui->imageSizeLabel->show();
        ui->imageSizeWidget->show();
        ui->imageFigureSizeWidget->show();

        ui->sheetSizeLabel->hide();
        ui->sheetSizeWidget->hide();
        ui->sheetFigureSizeWidget->hide();
    }

    resizeExportPreview();
}

void Export::onMainGraphRangeChange(GraphRange range)
{
    if(ui->linkToMainGraph->isChecked())
    {
        rangeWidget->setGraphRange(range);
        exportPreview->setGraphRange(range);
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
        if(ui->vectorFormat->isChecked())
        {
            if(exportPreview->size() != ui->scrollArea->contentsRect().size())
                exportPreview->resize(ui->scrollArea->contentsRect().size());
        }
        else
        {
            if(ui->scrollArea->contentsRect().size().width() > ui->imageWidth->value() &&
                    ui->scrollArea->contentsRect().size().height() > ui->imageHeight->value())
                exportPreview->resize(ui->imageWidth->value(), ui->imageHeight->value());
            else if(exportPreview->size() != ui->scrollArea->contentsRect().size())
                exportPreview->resize(ui->scrollArea->contentsRect().size());
        }

    }
    else
    {
        QSize targetSize = exportPreview->getTargetSupportSizePixels();
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
    const QSignalBlocker blocker(ui->sheetFigureHeight);
    const QSignalBlocker blocker1(ui->sheetFigureWidth);

    const QSignalBlocker blocker2(ui->imageFigureHeight);
    const QSignalBlocker blocker3(ui->imageFigureWidth);

    double smallest = ui->sheetHeight->value() < ui->sheetWidth->value() ? ui->sheetHeight->value() : ui->sheetWidth->value();
    double minRelSize = exportPreview->getMinFigureRelativeSize();

    ui->sheetMargin->setMaximum(smallest * (1 - minRelSize) / 2);
    ui->sheetFigureHeight->setMaximum(ui->sheetHeight->value() - 2 * ui->sheetMargin->value());
    ui->sheetFigureHeight->setMinimum(minRelSize * smallest);

    ui->sheetFigureWidth->setMaximum(ui->sheetWidth->value() - 2 * ui->sheetMargin->value());
    ui->sheetFigureWidth->setMinimum(minRelSize * smallest);

    smallest = ui->imageHeight->value() < ui->imageWidth->value() ? ui->imageHeight->value() : ui->imageWidth->value();

    ui->imageMargin->setMaximum(int(smallest * (1 - minRelSize) / 2));
    ui->imageFigureHeight->setMaximum(ui->imageHeight->value() - 2 * ui->imageMargin->value());
    ui->imageFigureHeight->setMinimum(int(smallest * minRelSize));

    ui->imageFigureWidth->setMaximum(ui->imageWidth->value() - 2 * ui->imageMargin->value());
    ui->imageFigureWidth->setMinimum(int(smallest * minRelSize));
}

void Export::getFileName()
{
    fileName = QFileDialog::getSaveFileName(this, tr("Enter file name in the selected folder"));
    if(!fileName.isEmpty())
    {
        QString extension;
        if(ui->vectorFormat->isChecked())
            extension = "." + ui->scalableFormatSelection->currentText().toLower();
        else extension = "." + ui->scalableFormatSelection->currentText().toLower();

        if(!fileName.endsWith(extension, Qt::CaseInsensitive))
            fileName.append(extension);

        ui->fileName->setText(fileName);
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
    ui->exportButton->setEnabled(false);
    timer.start();

    if(ui->vectorFormat->isChecked())
    {
        if(ui->scalableFormatSelection->currentText() == "PDF")
        {
            exportPreview->exportPDF(fileName, ui->customSize->isChecked() ? SheetSizeType::CUSTOM : SheetSizeType::NORMALISED);
        }
        else if(ui->scalableFormatSelection->currentText() == "SVG")
        {
            exportPreview->exportSVG(fileName);
        }
    }
    else // image format
    {

    }

}

void Export::setSheetFigureSizeCm(QSizeF sizeCm)
{
    ui->sheetFigureHeight->blockSignals(true);
    ui->sheetFigureWidth->blockSignals(true);

    ui->sheetFigureHeight->setValue(sizeCm.height());
    ui->sheetFigureWidth->setValue(sizeCm.width());

    ui->sheetFigureHeight->blockSignals(false);
    ui->sheetFigureWidth->blockSignals(false);
}

void Export::setImageFigureSizePx(QSize sizePx)
{
    ui->imageFigureHeight->blockSignals(true);
    ui->imageFigureWidth->blockSignals(true);

    ui->imageFigureHeight->setValue(sizePx.height());
    ui->imageFigureWidth->setValue(sizePx.width());

    ui->imageFigureHeight->blockSignals(false);
    ui->imageFigureWidth->blockSignals(false);
}

void Export::onSheetFigureSizeChange()
{
    exportPreview->setSheetFigureSizeCm(QSizeF(ui->sheetFigureWidth->value(), ui->sheetFigureHeight->value()));
    exportPreview->update();
}

void Export::onImageFigureSizeChange()
{
    exportPreview->setImageFigureSizePx(QSize(ui->imageFigureWidth->value(), ui->imageFigureHeight->value()));
    exportPreview->update();
}

void Export::onSheetMarginChange()
{
   constrainFigureSizeWidgets();
   exportPreview->setSheetMarginCm(ui->sheetMargin->value());
}

void Export::onImageMarginChange()
{
   constrainFigureSizeWidgets();
   exportPreview->setImageMarginPx(ui->imageMargin->value());
}

void Export::onSheetSizeChange()
{   
    QPageLayout layout;

    if(ui->orientationSelector->currentIndex() == 0)
    {
        layout.setOrientation(QPageLayout::Landscape);
        exportPreview->setSheetOrientation(QPageLayout::Landscape);
    }
    else
    {
        layout.setOrientation(QPageLayout::Portrait);
        exportPreview->setSheetOrientation(QPageLayout::Portrait);
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

    const QSignalBlocker blocker(ui->sheetWidth);
    const QSignalBlocker blocker2(ui->sheetHeight);

    ui->sheetWidth->setValue(sheetSize.width());
    ui->sheetHeight->setValue(sheetSize.height());

    constrainFigureSizeWidgets();

    exportPreview->setSheetSizeCm(sheetSize);

    exportPreview->update();

}

void Export::onImageSizeChange()
{
    constrainFigureSizeWidgets();

    exportPreview->setImageSizePx(QSize(ui->imageWidth->value(), ui->imageHeight->value()));
    resizeExportPreview();

    exportPreview->update();
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

void Export::swapImageHeightAndWidth()
{
    ui->imageHeight->blockSignals(true);
    ui->imageWidth->blockSignals(true);

    int tmp = ui->imageHeight->value();
    ui->imageHeight->setValue(ui->imageWidth->value());
    ui->imageWidth->setValue(tmp);

    ui->imageHeight->blockSignals(false);
    ui->imageWidth->blockSignals(false);

    onImageSizeChange();
}



Export::~Export()
{
    delete ui;
}
