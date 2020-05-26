#include "graphsizeadjusments.h"
#include "ui_graphsizeadjusments.h"

GraphSizeAdjusments::GraphSizeAdjusments(Information *information, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::graphSizeAdjusments)
{
    ui->setupUi(this);    

    this->information = information;

    // ui->sheetSizeSubWidget->setEnabled(false);

    makeConnects();

    updateWidgetVisibility();
    onStandardSheetSizeChange();
}

void GraphSizeAdjusments::onSizeSettingsChange()
{
    constrainFigureSizeWidgets();

    sizeSettings.sizeUnit = ui->pixelUnit->isChecked() ? ZeSizeSettings::PIXEL : ZeSizeSettings::CENTIMETER;
    sizeSettings.sizingType = ui->fitWindow->isChecked() ? ZeSizeSettings::FITWINDOW : ZeSizeSettings::CUSTOM;

    sizeSettings.cmMargins = ui->sheetMarginCm->value();
    sizeSettings.pxMargins = ui->sheetMarginPx->value();

    sizeSettings.cmFigureSize = QSizeF(ui->figureWidthCm->value(), ui->figureHeightCm->value());
    sizeSettings.pxFigureSize = QSize(ui->figureWidthPx->value(), ui->figureHeightPx->value());

    sizeSettings.scalingFactor = ui->scalingFactor->value();

    sizeSettings.cmSheetSize = QSizeF(ui->sheetWidthCm->value(), ui->sheetHeightCm->value());
    sizeSettings.pxSheetSize = QSize(ui->sheetWidthPx->value(), ui->sheetHeightPx->value());

    information->disconnect(this);
    information->setGraphSizeSettings(sizeSettings);
    connect(information, SIGNAL(sizeSettingsChanged()), this, SLOT(onExternalSizeSettingsChange()));
}

void GraphSizeAdjusments::updateWidgetVisibility()
{
    if(ui->customSizing->isChecked())
    {
        if(ui->pixelUnit->isChecked())
        {
            ui->pxSizeWidget->show();
            ui->pxFigSizeWidget->show();

            ui->cmSizeWidget->hide();
            ui->cmFigSizeWidget->hide();
        }
        else
        {
            ui->pxSizeWidget->hide();
            ui->pxFigSizeWidget->hide();

            ui->cmSizeWidget->show();
            ui->cmFigSizeWidget->show();
        }

        onStandardSheetSizeChange();
    }
    else
    {
        ui->cmSizeWidget->hide();
        ui->pxSizeWidget->hide();
    }

    // Margins and figure sizes
    if(ui->pixelUnit->isChecked())
    {
        ui->pxMarginWidget->show();
        ui->pxFigSizeWidget->show();

        ui->cmMarginWidget->hide();
        ui->cmFigSizeWidget->hide();
    }
    else
    {
        ui->pxMarginWidget->hide();
        ui->pxFigSizeWidget->hide();

        ui->cmMarginWidget->show();
        ui->cmFigSizeWidget->show();
    }


}

void GraphSizeAdjusments::onExternalSizeSettingsChange()
{
    sizeSettings = information->getGraphSizeSettings();

    if(sizeSettings.sizeUnit == ZeSizeSettings::PIXEL)
        ui->pixelUnit->setChecked(true);
    else ui->centimeterUnit->setChecked(true);

    if(sizeSettings.sizingType == ZeSizeSettings::FITWINDOW)
        ui->fitWindow->setChecked(true);
    else ui->customSize->setChecked(true);

    ui->sheetMarginCm->setValue(sizeSettings.cmMargins);
    ui->sheetMarginPx->setValue(sizeSettings.pxMargins);

    ui->figureWidthCm->setValue(sizeSettings.cmFigureSize.width());
    ui->figureHeightCm->setValue(sizeSettings.cmFigureSize.height());

    ui->figureWidthPx->setValue(sizeSettings.pxFigureSize.width());
    ui->figureHeightPx->setValue(sizeSettings.pxFigureSize.height());

    sizeSettings.scalingFactor = ui->scalingFactor->value();

    ui->sheetWidthCm->setValue(sizeSettings.cmSheetSize.width());
    ui->sheetHeightCm->setValue(sizeSettings.cmSheetSize.height());

    ui->sheetWidthPx->setValue(sizeSettings.pxSheetSize.width());
    ui->sheetHeightPx->setValue(sizeSettings.pxSheetSize.height());

    constrainFigureSizeWidgets();
}

void GraphSizeAdjusments::makeConnects()
{
    connect(ui->sheetMarginCm, SIGNAL(valueChanged(double)), this, SLOT(onSizeSettingsChange()));
    connect(ui->sheetMarginPx, SIGNAL(valueChanged(int)), this, SLOT(onSizeSettingsChange()));

    connect(ui->scalingFactor, SIGNAL(valueChanged(double)), this, SLOT(onSizeSettingsChange()));

    connect(ui->figureHeightCm, SIGNAL(valueChanged(double)), this, SLOT(onSizeSettingsChange()));
    connect(ui->figureWidthCm, SIGNAL(valueChanged(double)), this, SLOT(onSizeSettingsChange()));

    connect(ui->figureHeightPx, SIGNAL(valueChanged(int)), this, SLOT(onSizeSettingsChange()));
    connect(ui->figureWidthPx, SIGNAL(valueChanged(int)), this, SLOT(onSizeSettingsChange()));

    connect(information, SIGNAL(sizeSettingsChanged()), this, SLOT(onExternalSizeSettingsChange()));

    connect(ui->sheetHeightCm, SIGNAL(valueChanged(double)), this, SLOT(onSizeSettingsChange()));
    connect(ui->sheetWidthCm, SIGNAL(valueChanged(double)), this, SLOT(onSizeSettingsChange()));

    connect(ui->sheetHeightPx, SIGNAL(valueChanged(int)), this, SLOT(onSizeSettingsChange()));
    connect(ui->sheetWidthPx, SIGNAL(valueChanged(int)), this, SLOT(onSizeSettingsChange()));

    connect(ui->A3, SIGNAL(clicked()), this, SLOT(onStandardSheetSizeChange()));
    connect(ui->A4, SIGNAL(clicked()), this, SLOT(onStandardSheetSizeChange()));
    connect(ui->A5, SIGNAL(clicked()), this, SLOT(onStandardSheetSizeChange()));
    connect(ui->customSize, SIGNAL(clicked(bool)), this, SLOT(onStandardSheetSizeChange()));

    connect(ui->sheetSizeSwap, SIGNAL(released()), this, SLOT(swapSheetHeightAndWidth()));
    connect(ui->imageSizeSwap, SIGNAL(released()), this, SLOT(swapImageHeightAndWidth()));

    connect(ui->orientationSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(onStandardSheetSizeChange()));

    connect(ui->pixelUnit, SIGNAL(toggled(bool)), this, SLOT(updateWidgetVisibility()));
    connect(ui->fitWindow, SIGNAL(toggled(bool)), this, SLOT(updateWidgetVisibility()));
}

void GraphSizeAdjusments::constrainFigureSizeWidgets()
{
    const QSignalBlocker blocker(ui->figureHeightCm);
    const QSignalBlocker blocker1(ui->figureWidthCm);

    const QSignalBlocker blocker2(ui->figureHeightPx);
    const QSignalBlocker blocker3(ui->figureWidthPx);

    double smallest = ui->sheetHeightCm->value() < ui->sheetWidthCm->value() ? ui->sheetHeightCm->value() : ui->sheetWidthCm->value();
    double minRelSize = RELATIVE_MIN_SIZE;

    ui->sheetMarginCm->setMaximum(smallest * (1 - minRelSize) / 2);
    ui->figureHeightCm->setMaximum(ui->sheetHeightCm->value() - 2 * ui->sheetMarginCm->value());
    ui->figureHeightCm->setMinimum(minRelSize * smallest);

    ui->figureWidthCm->setMaximum(ui->sheetWidthCm->value() - 2 * ui->sheetMarginCm->value());
    ui->figureWidthCm->setMinimum(minRelSize * smallest);

    smallest = ui->sheetHeightPx->value() < ui->sheetWidthPx->value() ? ui->sheetHeightPx->value() : ui->sheetWidthPx->value();

    ui->sheetMarginPx->setMaximum(int(smallest * (1 - minRelSize) / 2));
    ui->figureHeightPx->setMaximum(ui->sheetHeightPx->value() - 2 * ui->sheetMarginPx->value());
    ui->figureHeightPx->setMinimum(int(smallest * minRelSize));

    ui->figureWidthPx->setMaximum(ui->sheetWidthPx->value() - 2 * ui->sheetMarginPx->value());
    ui->figureWidthPx->setMinimum(int(smallest * minRelSize));
}


void GraphSizeAdjusments::setSheetFigureSizeCm(QSizeF sizeCm)
{
    ui->figureHeightCm->blockSignals(true);
    ui->figureWidthCm->blockSignals(true);

    ui->figureHeightCm->setValue(sizeCm.height());
    ui->figureWidthCm->setValue(sizeCm.width());

    ui->figureHeightCm->blockSignals(false);
    ui->figureWidthCm->blockSignals(false);
}

void GraphSizeAdjusments::setImageFigureSizePx(QSize sizePx)
{
    ui->figureHeightPx->blockSignals(true);
    ui->figureWidthPx->blockSignals(true);

    ui->figureHeightPx->setValue(sizePx.height());
    ui->figureWidthPx->setValue(sizePx.width());

    ui->figureHeightPx->blockSignals(false);
    ui->figureWidthPx->blockSignals(false);
}

void GraphSizeAdjusments::onStandardSheetSizeChange()
{
    if(not ui->customSize->isChecked())
    {        
        ui->sheetSizeSubWidget->hide();
        ui->orientationSelector->show();

        QPageLayout layout;
        QSizeF sheetSize;

        if(ui->orientationSelector->currentIndex() == 0)
        {
            layout.setOrientation(QPageLayout::Landscape);
        }
        else
        {
            layout.setOrientation(QPageLayout::Portrait);
        }

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

        const QSignalBlocker blocker(ui->sheetWidthCm);
        const QSignalBlocker blocker2(ui->sheetHeightCm);

        ui->sheetWidthCm->setValue(sheetSize.width());
        ui->sheetHeightCm->setValue(sheetSize.height());

        constrainFigureSizeWidgets();
    }
    else
    {
        ui->sheetSizeSubWidget->show();
        ui->orientationSelector->hide();
    }
}

void GraphSizeAdjusments::swapSheetHeightAndWidth()
{
    const QSignalBlocker blocker(ui->sheetHeightCm);
    const QSignalBlocker blocker2(ui->sheetWidthCm);

    double tmp = ui->sheetHeightCm->value();
    ui->sheetHeightCm->setValue(ui->sheetWidthCm->value());
    ui->sheetWidthCm->setValue(tmp);

    ui->orientationSelector->setCurrentIndex((ui->orientationSelector->currentIndex() + 1) % 2);

    onSizeSettingsChange();
}

void GraphSizeAdjusments::swapImageHeightAndWidth()
{
    const QSignalBlocker blocker(ui->sheetHeightPx);
    const QSignalBlocker blocker2(ui->sheetWidthPx);

    int tmp = ui->sheetHeightPx->value();
    ui->sheetHeightPx->setValue(ui->sheetWidthPx->value());
    ui->sheetWidthPx->setValue(tmp);

    onSizeSettingsChange();
}

GraphSizeAdjusments::~GraphSizeAdjusments()
{
    delete ui;
}
