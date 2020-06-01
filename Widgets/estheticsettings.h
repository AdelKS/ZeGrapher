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
    explicit EstheticSettings(Information *information, QWidget *parent = nullptr);
    const ZeEstheticSettings &getSettings();
    ~EstheticSettings();

public slots:
    void resetToDefaults();
    void apply();
    void processUserInput();
    void saveSettingsToDisk();

private:    
    void loadSettingsFromDisk();
    void makeConnects();

    Information *information;
    QColorButton *backgroundColorButton, *defaultColorButton;
    ZeEstheticSettings estheticSettings;
    Ui::EstheticSettings *ui;
};

#endif // ESTHETICSETTINGS_H
