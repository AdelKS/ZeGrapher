#include "appsettings.h"
#include "ui_appsettings.h"

AppSettings::AppSettings(Information *information, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppSettings)
{
    ui->setupUi(this);

    this->information = information;

    makeConnects();

    loadSettingsFromDisk();
}


void AppSettings::makeConnects()
{
    connect(ui->appFontFamily, SIGNAL(currentFontChanged(QFont)), this, SLOT(apply()));
    connect(ui->appFontSize, SIGNAL(valueChanged(int)), this, SLOT(apply()));
    connect(ui->updateCheckAtStart, SIGNAL(toggled(bool)), this, SLOT(apply()));
    connect(ui->languageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(apply()));
}

void AppSettings::processUserInput()
{
    appSettings.font = ui->appFontFamily->font();
    appSettings.font.setPointSize(ui->appFontSize->value());
    appSettings.startupUpdateCheck = ui->updateCheckAtStart->isChecked();

    if(ui->languageComboBox->currentText() == "English")
    {
        appSettings.language = ZeAppSettings::Language::ENGLISH;
    }
    else if(ui->languageComboBox->currentText() == "Français")
    {
        appSettings.language = ZeAppSettings::Language::FRENCH;
    }
    else if(ui->languageComboBox->currentText() == "Deutsche")
    {
         appSettings.language = ZeAppSettings::Language::GERMAN;
    }
}

void AppSettings::apply()
{
    processUserInput();
    information->setAppSettings(appSettings);
}

void AppSettings::resetToDefaults()
{
    // TODO
}

void AppSettings::loadSettingsFromDisk()
{
    QSettings settings;

    settings.beginGroup("app");

    if(settings.contains("update_check_at_start"))
        ui->updateCheckAtStart->setChecked(settings.value("update_check_at_start").toBool());

    if(settings.contains("language"))
    {
        if(settings.value("language").toString() == "fr")
            ui->languageComboBox->setCurrentIndex(1);
        else if(settings.value("language").toString() == "de")
            ui->languageComboBox->setCurrentIndex(2);
        else ui->languageComboBox->setCurrentIndex(0);
    }

    settings.beginGroup("font");

    QFontInfo fontInfo(qApp->font());

    if(settings.contains("pixel_size"))
        ui->appFontSize->setValue(settings.value("pixel_size").toInt());
    else ui->appFontSize->setValue(fontInfo.pixelSize());

    if(settings.contains("family"))
        ui->appFontFamily->setFont(QFont(settings.value("family").toString()));
    else ui->appFontFamily->setFont(fontInfo.family());

    apply();
}

void AppSettings::saveSettingsToDisk()
{
    QSettings settings;

    settings.beginGroup("app");

    settings.setValue("update_check_at_start", ui->updateCheckAtStart->isChecked());

    if(ui->languageComboBox->currentText() == "English")
        settings.setValue("language", "en");
    else if(ui->languageComboBox->currentText() == "Français")
         settings.setValue("language", "fr");
    else if(ui->languageComboBox->currentText() == "Deutsche")
         settings.setValue("language", "de");

    settings.beginGroup("font");

    settings.setValue("pixel_size", ui->appFontSize->value());
    settings.setValue("family", ui->appFontFamily->currentFont().family());
}

AppSettings::~AppSettings()
{
    delete ui;
}
