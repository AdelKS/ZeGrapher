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

#include "Utils/palettewatcher.h"
#include "information.h"
#include "structures.h"

#include <QGuiApplication>
#include <QCommandLineParser>
#include <QObject>
#include <QQmlApplicationEngine>


int main(int argc, char *argv[])
{
  QGuiApplication a(argc, argv);

  PaletteWatcher watcher;
  paletteWatcher = &watcher;

  a.setWindowIcon(QIcon(":/icons/ZeGrapher.svg"));

  QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);

  QCoreApplication::setApplicationName("ZeGrapher");
  QCoreApplication::setApplicationVersion(SOFTWARE_VERSION);

  // define after QGuiApp and QCoreApp::set* because it will use stuff from them
  Information info;
  information = &info;

  QCommandLineParser parser;
  parser.setApplicationDescription("2D math plotter");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument(QObject::tr("file"), QObject::tr("ZeGrapher (.zg) workspace file(s) to open on startup"));
  parser.process(a);

  const auto positionalArguments = parser.positionalArguments();
  const QString lastWorkspace = QStandardPaths::locate(QStandardPaths::AppConfigLocation,
                                                       "last-workbook.zg");
  if (not positionalArguments.empty())
    for (QString& document: parser.positionalArguments())
      info.importYaml(QUrl::fromLocalFile(document));
  else if (not lastWorkspace.isEmpty())
      info.importYaml(QUrl::fromLocalFile(lastWorkspace));
  else
  {
    auto* cst = zg::mathWorld.addMathObject(zg::MathObject::CONSTANT)->getConstant();
    cst->set_value(2);
    cst->setName("a");

    auto* cos = zg::mathWorld.addMathObject(zg::MathObject::EQUATION)->getEquation();
    cos->setEquation("f(x) = a * cos(x)");

    auto* fibo = zg::mathWorld.addMathObject(zg::MathObject::EQUATION)->getEquation();
    fibo->setEquation("u(n) = a ; a ; u(n-2) + u(n-1)");
  }

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
