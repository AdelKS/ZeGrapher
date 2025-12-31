/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
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

#include "structures.h"
#include "globalvars.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>


int main(int argc, char *argv[])
{
  QGuiApplication a(argc, argv);

  // the information global var can't initialize the correct font until QGuiApplication is initialized.
  // so we set it here
  information.setAppFont(a.font());

  QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);

  QCoreApplication::setOrganizationName("ZeGrapher Project");
  QCoreApplication::setOrganizationDomain("zegrapher.com");
  QCoreApplication::setApplicationName("ZeGrapher");

  QSettings settings;
  QTranslator translator;

  settings.beginGroup("app");

  QLocale::Language language;

  if (settings.contains("language"))
    language = settings.value("language").toLocale().language();
  else
    language = QLocale::system().language();

  QString langString = langToShortString(language);
  if (supportedLangs.contains(language)) [[maybe_unused]]
    bool loaded = translator.load(":/Translations/ZeGrapher_" + langToShortString(language) + ".qm");

  settings.beginGroup("font");

  if (settings.contains("family") && settings.contains("size"))
  {
    QFont font;
    font.setPointSizeF(settings.value("size").toDouble());
    font.setFamily(settings.value("family").toString());
    font.setStyleStrategy(QFont::PreferAntialias);
    a.setFont(font);
  }

  a.installTranslator(&translator);

  QQmlApplicationEngine engine;

  QObject::connect(&engine,
                   &QQmlApplicationEngine::objectCreated,
                   [](QObject* object, const QUrl&)
                   {
                     if (object == nullptr)
                       qFatal("unable to load scene");
                   });

  engine.load("qrc:///qt/qml/ZeGrapher/MainWindow.qml");

  return a.exec();
}
