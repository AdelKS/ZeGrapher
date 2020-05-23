#ifndef ZEGRIDSETTINGSWIDGET_H
#define ZEGRIDSETTINGSWIDGET_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>

#include "structures.h"
#include "qcolorbutton.h"
#include "information.h"

namespace Ui {
class ZeGridSettingsWidget;
}

class ZeGridSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ZeGridSettingsWidget(Information *information, QWidget *parent = nullptr);
    ZeGridSettings getSettings();
    ~ZeGridSettingsWidget();

signals:
    void settingsUpdated();

public slots:
    void processUserInput();
    void apply();

protected slots:
    void swapGridData();

protected:
    void loadGridSettingsInUi(ZeAxisName name);
    void loadGridSettingsInUi(const Ze1DGridSettings &unidimGridSettings);
    void makeConnects();
    void loadDefaults();

    QColorButton *gridColorButton, *subgridColorButton;
    Ui::ZeGridSettingsWidget *ui;
    ZeGridSettings gridSettings;
    ZeAxisName currentAxis;
    Information *information;
};

#endif // ZEGRIDSETTINGSWIDGET_H
