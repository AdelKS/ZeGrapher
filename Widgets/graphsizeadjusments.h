#ifndef GRAPHSIZEADJUSMENTS_H
#define GRAPHSIZEADJUSMENTS_H

#include <QWidget>

#include "information.h"

namespace Ui {
class graphSizeAdjusments;
}

class GraphSizeAdjusments : public QWidget
{
    Q_OBJECT

public:
    explicit GraphSizeAdjusments(Information *information, QWidget *parent = nullptr);
    ~GraphSizeAdjusments();

signals:
    void centimeterUnitEnabled();
    void pixelUnitEnabled();

public slots:
    void setSheetFigureSizeCm(QSizeF sizeCm);
    void setImageFigureSizePx(QSize sizePx);

protected slots:
    void onSizeSettingsChange();
    void onExternalSizeSettingsChange();

    void onStandardSheetSizeChange();

    void swapSheetHeightAndWidth();
    void swapImageHeightAndWidth();

    void constrainFigureSizeWidgets();

    void updateWidgetVisibility();

private:
    void makeConnects();

    ZeSizeSettings sizeSettings;
    Information *information;
    Ui::graphSizeAdjusments *ui;
};

#endif // GRAPHSIZEADJUSMENTS_H
