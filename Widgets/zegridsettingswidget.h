#ifndef ZEGRIDSETTINGSWIDGET_H
#define ZEGRIDSETTINGSWIDGET_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>

#include "structures.h"
#include "qcolorbutton.h"

namespace Ui {
class ZeGridSettingsWidget;
}

class ZeGridSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ZeGridSettingsWidget(QWidget *parent = nullptr);
    ZeGridSettings getSettings();
    ~ZeGridSettingsWidget();

signals:
    void settingsUpdated();

public slots:
    void apply();

protected slots:
    void swapGridData();

protected:
    void loadGridSettingsInUi(ZeAxisName name);
    void loadGridSettingsInUi(const ZeUnidimGridSettings &unidimGridSettings);
    void makeConnects();
    void loadDefaults();

    QColorButton *gridColorButton, *subgridColorButton;
    Ui::ZeGridSettingsWidget *ui;
    ZeGridSettings gridSettings;
    ZeAxisName currentAxis;
};

#endif // ZEGRIDSETTINGSWIDGET_H
