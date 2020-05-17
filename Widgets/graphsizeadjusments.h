#ifndef GRAPHSIZEADJUSMENTS_H
#define GRAPHSIZEADJUSMENTS_H

#include <QWidget>

#include "information.h"

namespace Ui {
class graphSizeAdjusments;
}

class graphSizeAdjusments : public QWidget
{
    Q_OBJECT

public:
    explicit graphSizeAdjusments(Information *information, QWidget *parent = nullptr);
    ~graphSizeAdjusments();

signals:
    void centimeterUnitEnabled();
    void pixelUnitEnabled();

public slots:
    void setSheetFigureSizeCm(QSizeF sizeCm);
    void setImageFigureSizePx(QSize sizePx);

    void sizeUnitChanged();

    void enableExportButton();
    void getFileName();
    void exportGraph();

protected slots:
    void onSizeSettingsChange();
    void onExternalSizeSettingsChange();

    void onStandardSheetSizeChange();

    void swapSheetHeightAndWidth();
    void swapImageHeightAndWidth();

    void constrainFigureSizeWidgets();

private:
    void makeConnects();

    ZeSizeSettings sizeSettings;
    Information *information;
    Ui::graphSizeAdjusments *ui;
};

#endif // GRAPHSIZEADJUSMENTS_H
