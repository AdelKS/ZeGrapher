#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QWidget>

#include "information.h"
#include "Widgets/qcolorbutton.h"

namespace Ui {
class AppSettings;
}

class AppSettings : public QWidget
{
    Q_OBJECT

public:
    explicit AppSettings(QWidget *parent = nullptr);
    ~AppSettings();

public slots:
    void resetToDefaults();
    void processUserInput();
    void apply();
    void saveSettingsToDisk();

protected:
    void loadSettingsFromDisk();

private:
    void fillSupportedLanguages();
    void makeConnects();

    QFont currentAppFont;
    double currentAppFontSize;
    QString currentLanguage;
    ZeAppSettings appSettings;

    Ui::AppSettings *ui;
};

#endif // APPSETTINGS_H
