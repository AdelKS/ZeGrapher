#include "appsettings.h"
#include "ui_appsettings.h"

AppSettings::AppSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppSettings)
{
    ui->setupUi(this);

    fillSupportedLanguages();

    makeConnects();

    loadSettingsFromDisk();
}

void AppSettings::fillSupportedLanguages()
{
    ui->languageComboBox->clear();

    for(auto language: supportedLangs)
        ui->languageComboBox->addItem(QLocale(language).nativeLanguageName(), QVariant(QLocale(language)));
}


void AppSettings::makeConnects()
{
    connect(ui->appFontFamily, SIGNAL(currentFontChanged(QFont)), this, SLOT(apply()));
    connect(ui->appFontSize, SIGNAL(valueChanged(double)), this, SLOT(apply()));
    connect(ui->updateCheckAtStart, SIGNAL(toggled(bool)), this, SLOT(apply()));
    connect(ui->languageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(apply()));
}

void AppSettings::processUserInput()
{

    if(ui->languageComboBox->currentText() != currentLanguage or
       ui->appFontSize->value() != currentAppFontSize or
       ui->appFontFamily->currentFont() != currentAppFont)
    {
        ui->restartRequiredLabel->show();
    }
    else ui->restartRequiredLabel->hide();


    appSettings.font = ui->appFontFamily->font();
    appSettings.font.setPointSize(ui->appFontSize->value());
    appSettings.startupUpdateCheck = ui->updateCheckAtStart->isChecked();
    appSettings.language = ui->languageComboBox->currentData().toLocale().language();
}

void AppSettings::apply()
{
    processUserInput();
    information.setAppSettings(appSettings);
}

void AppSettings::resetToDefaults()
{
    // TODO
}

void AppSettings::loadSettingsFromDisk()
{
    QSettings settings;

    // Clear the settings if they come from another version.
    if(not settings.contains("app/version") or settings.value("app/version").toString() != SOFTWARE_VERSION_STR)
    {
        settings.clear();
        return;
    }

    settings.beginGroup("app");

    if(settings.contains("update_check_at_start"))
        ui->updateCheckAtStart->setChecked(settings.value("update_check_at_start").toBool());

    if(settings.contains("language"))
    {
        QLocale lang = settings.value("language").toLocale();
        currentLanguage = lang.nativeLanguageName();
        ui->languageComboBox->setCurrentIndex(ui->languageComboBox->findData(lang));
    }
    else
    {
        // put index to 0, which is English
        ui->languageComboBox->setCurrentIndex(0);
        currentLanguage = ui->languageComboBox->currentData().toLocale().nativeLanguageName();
    }

    settings.beginGroup("font");

    QFontInfo fontInfo(qApp->font());

    if(settings.contains("size"))
        ui->appFontSize->setValue(settings.value("size").toDouble());
    else ui->appFontSize->setValue(fontInfo.pointSizeF());
    currentAppFontSize = ui->appFontSize->value();

    if(settings.contains("family"))
        ui->appFontFamily->setFont(QFont(settings.value("family").toString()));
    else ui->appFontFamily->setFont(fontInfo.family());
    currentAppFont = ui->appFontFamily->currentFont();

    apply();
}

void AppSettings::saveSettingsToDisk()
{
    QSettings settings;

    settings.beginGroup("app");

    settings.setValue("update_check_at_start", ui->updateCheckAtStart->isChecked());
    settings.setValue("language", ui->languageComboBox->currentData().toLocale());
    settings.setValue("version", SOFTWARE_VERSION_STR);

    settings.beginGroup("font");

    settings.setValue("size", ui->appFontSize->value());
    settings.setValue("family", ui->appFontFamily->currentFont().family());
}

AppSettings::~AppSettings()
{
    delete ui;
}
