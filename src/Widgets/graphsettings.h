#ifndef GRAPHSETTINGS_H
#define GRAPHSETTINGS_H

#include <QWidget>

#include "Widgets/qcolorbutton.h"
#include "structures.h"

namespace Ui {
class GraphSettings;
}

class GraphSettings : public QWidget
{
    Q_OBJECT

public:
    explicit GraphSettings(QWidget *parent = nullptr);
    ~GraphSettings();
    const ZeGraphSettings &getSettings() const;

public slots:
    void resetToDefaults();
    void apply();
    void processUserInput();
    void saveSettingsToDisk();

private:
    void loadSettingsFromDisk();
    void makeConnects();

    QColorButton *backgroundColorButton, *defaultColorButton;
    ZeGraphSettings graphSettings;
    Ui::GraphSettings *ui;
};

#endif // GRAPHSETTINGS_H
