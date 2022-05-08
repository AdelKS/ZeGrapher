/****************************************************************************
**  Copyright (c) 2019, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "Windows/mainwindow.h"

const QList<QLocale::Language> supportedLangs = { QLocale::English, QLocale::French, QLocale::German, QLocale::Chinese};

 QString langToShortString(QLocale::Language lang)
{
    if(lang == QLocale::French)
        return "fr";
    else if(lang == QLocale::German)
        return "de";
    else if(lang == QLocale::Chinese)
        return "zh";
    else return "en";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);

    QCoreApplication::setOrganizationName("ZeGrapher Project");
    QCoreApplication::setOrganizationDomain("zegrapher.com");
    QCoreApplication::setApplicationName("ZeGrapher");

    QSettings settings;
    QTranslator translator;

    settings.beginGroup("app");

    QLocale::Language language;

    if(settings.contains("language"))
        language = settings.value("language").toLocale().language();
    else language = QLocale::system().language();

    QString langString = langToShortString(language);
    if(supportedLangs.contains(language))
        translator.load(":/Translations/ZeGrapher_" + langToShortString(language) + ".qm");

    settings.beginGroup("font");

    if(settings.contains("family") && settings.contains("size"))
    {
        QFont font;
        font.setPointSizeF(settings.value("size").toDouble());
        font.setFamily(settings.value("family").toString());
        font.setStyleStrategy(QFont::PreferAntialias);
        a.setFont(font);
    }

    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
