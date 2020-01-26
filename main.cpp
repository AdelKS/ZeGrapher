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

    if(settings.contains("language"))
    {
        QString language = settings.value("language").toString();
        if(language == "fr")
            translator.load(":/ZeGrapher_fr.qm");
        else if(language == "de")
            translator.load(":/ZeGrapher_de.qm");
    }
    else
    {
        QLocale locale;
        if(locale.language() == QLocale::French)
            translator.load(":/ZeGrapher_fr.qm");
        else if(locale.language() == QLocale::German)
            translator.load(":/ZeGrapher_de.qm");
    }

    settings.beginGroup("font");

    if(settings.contains("family") && settings.contains("pixel_size"))
    {
        QFont font;
        font.setPixelSize(settings.value("pixel_size").toInt());
        font.setFamily(settings.value("family").toString());
        font.setStyleStrategy(QFont::PreferAntialias);
        a.setFont(font);
    }

    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
