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

#include <QCommandLineParser>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QLoggingCategory>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QStandardPaths>
#include <QTranslator>


int main(int argc, char *argv[])
{
  QGuiApplication a(argc, argv);

  PaletteWatcher watcher;
  paletteWatcher = &watcher;

  a.setWindowIcon(QIcon(":/icons/ZeGrapher.svg"));

  if (QFontDatabase::addApplicationFont(":/fonts/latinmodern-math.otf") == -1)
    qWarning("failed to load the bundled Latin Modern Math font");

  // QT_NO_*_OUTPUT only compiles out C++ q(Debug|Info|Warning) calls, QML's
  // console.* goes through logging categories at runtime
  QString logFilterRules;
#ifdef QT_NO_DEBUG_OUTPUT
  logFilterRules += "*.debug=false\n";
#endif
#ifdef QT_NO_INFO_OUTPUT
  logFilterRules += "*.info=false\n";
#endif
#ifdef QT_NO_WARNING_OUTPUT
  logFilterRules += "*.warning=false\n";
#endif
  QLoggingCategory::setFilterRules(logFilterRules);

  QCoreApplication::setApplicationName("ZeGrapher");
  QCoreApplication::setApplicationVersion(SOFTWARE_VERSION);

  // define after QGuiApp and QCoreApp::set* because it will use stuff from them
  Information info;
  information = &info;

  // an imported workbook can override it
  info.appSettings.language = ZeAppSettings::Language(systemLanguage());

  QCommandLineParser parser;
  parser.setApplicationDescription("2D math plotter");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument(QObject::tr("file"), QObject::tr("ZeGrapher (.zg) document(s) to open on startup"));
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

  QTranslator translator;
  QQmlApplicationEngine engine;

  QObject::connect(&engine,
                   &QQmlApplicationEngine::objectCreated,
                   [](QObject* object, const QUrl&)
                   {
                     if (object == nullptr)
                       qFatal("unable to load scene");
                   });

  auto applyLanguage = [&a, &translator, &engine, &info]()
  {
    a.removeTranslator(&translator);

    const auto lang = QLocale::Language(info.appSettings.language);
    if (lang != QLocale::English
        and translator.load(":/translations/ZeGrapher_" + langToShortString(lang) + ".qm"))
      a.installTranslator(&translator);

    engine.retranslate();
  };

  applyLanguage();
  QObject::connect(&info.appSettings, &ZeAppSettings::languageChanged, &engine, applyLanguage);

  engine.load("qrc:///qt/qml/ZeGrapher/MainWindow.qml");

  return a.exec();
}
