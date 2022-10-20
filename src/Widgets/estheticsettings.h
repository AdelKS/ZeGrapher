#ifndef ESTHETICSETTINGS_H
#define ESTHETICSETTINGS_H

#include <QWidget>

#include "information.h"
#include "Widgets/qcolorbutton.h"

namespace Ui {
class EstheticSettings;
}

class EstheticSettings : public QWidget
{
    Q_OBJECT

public:
    explicit EstheticSettings(QWidget *parent = nullptr);
    ~EstheticSettings();
    const ZeEstheticSettings &getSettings();

public slots:
    void resetToDefaults();
    void apply();
    void processUserInput();
    void saveSettingsToDisk();

private:
    void loadSettingsFromDisk();
    void makeConnects();

    QColorButton *backgroundColorButton, *defaultColorButton;
    ZeEstheticSettings estheticSettings;
    Ui::EstheticSettings *ui;
};

#endif // ESTHETICSETTINGS_H
