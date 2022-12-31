#ifndef GRAPHSIZESETTINGS_H
#define GRAPHSIZESETTINGS_H

#include <QWidget>

#include "information.h"

namespace Ui {
class GraphSizeSettings;
}

class GraphSizeSettings : public QWidget
{
    Q_OBJECT

public:
    explicit GraphSizeSettings(QWidget *parent = nullptr);
    const ZeSizeSettings &getSettings();
    ~GraphSizeSettings();

signals:
    void centimeterUnitEnabled();
    void pixelUnitEnabled();

public slots:
    void setSheetFigureSizeCm(QSizeF sizeCm);
    void setImageFigureSizePx(QSize sizePx);

    void processUserInput();
    void apply();

protected slots:
    void onExternalSizeSettingsChange();

    void onStandardSheetSizeChange();

    void swapSheetHeightAndWidth();
    void swapImageHeightAndWidth();

    void constrainFigureSizeWidgets();

    void updateWidgetVisibility();

private:
    void makeConnects();

    double screenDPI;
    ZeSizeSettings sizeSettings;
    Ui::GraphSizeSettings *ui;
};

#endif // GRAPHSIZESETTINGS_H
