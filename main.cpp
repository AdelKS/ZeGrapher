/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include <QApplication>
#include "Windows/zegrapher.h"
#include <QLocale>
#include <QTranslator>
#include <QTextCodec>
#include <QLibraryInfo>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("ZeGrapher Project");
    QCoreApplication::setOrganizationDomain("zegrapher.com");
    QCoreApplication::setApplicationName("ZeGrapher");

    QString locale = QLocale::system().name();
    QTranslator translator;
    translator.load(QString("ZeGrapher_") + locale);

    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
