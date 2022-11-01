#include "information.h"
#include "appsettings.h"
#include "ui_appsettings.h"

AppSettings::AppSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppSettings)
{
    ui->setupUi(this);

    fillSupportedLanguages();
    loadSettingsFromDisk();

    makeConnects();

    apply();
}

void AppSettings::fillSupportedLanguages()
{
    ui->languageComboBox->clear();

    for(const auto& language: supportedLangs)
        ui->languageComboBox->addItem(QLocale(language).nativeLanguageName(), QVariant(QLocale(language)));
}


void AppSettings::makeConnects()
{
    connect(ui->appFontFamily, SIGNAL(currentFontChanged(QFont)), this, SLOT(apply()));
    connect(ui->appFontSize, SIGNAL(valueChanged(double)), this, SLOT(apply()));
    connect(ui->updateCheckAtStart, SIGNAL(toggled(bool)), this, SLOT(apply()));
    connect(ui->languageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(apply()));
    connect(ui->validColor, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));
    connect(ui->invalidColor, SIGNAL(colorChanged(QColor)), this, SLOT(apply()));
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

    appSettings.invalidSyntax = ui->invalidColor->getCurrentColor();
    appSettings.validSyntax = ui->validColor->getCurrentColor();
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

    settings.endGroup();

    settings.beginGroup("color");

    if(settings.contains("valid_syntax"))
        ui->validColor->setColor(QColor(settings.value("valid_syntax").toString()));
    else ui->validColor->setColor(QColor(VALID_COLOR));

    if(settings.contains("invalid_syntax"))
        ui->invalidColor->setColor(QColor(settings.value("invalid_syntax").toString()));
    else ui->invalidColor->setColor(QColor(INVALID_COLOR));

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

    settings.endGroup();

    settings.beginGroup("color");

    settings.setValue("valid_syntax", ui->validColor->getCurrentColor().name());
    settings.setValue("invalid_syntax", ui->invalidColor->getCurrentColor().name());
}

AppSettings::~AppSettings()
{
    saveSettingsToDisk();
    delete ui;
}
