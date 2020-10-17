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
        QLocale::Language language = settings.value("language").toLocale().language();
        if(supportedLangs.contains(language))
            translator.load(":/ZeGrapher_" + langToShortString(language) + ".qm");
    }
    else
    {
        QLocale::Language language = QLocale::system().language();
        if(supportedLangs.contains(language))
            translator.load(":/ZeGrapher_" + langToShortString(language) + ".qm");
    }

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
