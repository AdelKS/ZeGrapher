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

#include "information.h"
#include "structures.h"
#include "buildconfig.h"

#include <QCommandLineParser>
#include <QEvent>
#include <QFileOpenEvent>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QLocale>
#include <QLoggingCategory>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QTranslator>

#ifdef Q_OS_MACOS
namespace {

/// @brief opens the documents that macOS sends as QFileOpenEvents instead of
///        command line arguments
class DocumentOpener: public QObject
{
public:
  explicit DocumentOpener(Information& info): info(info) {}

  QStringList startupDocuments;
  bool started = false;

protected:
  bool eventFilter(QObject* watched, QEvent* event) override
  {
    if (event->type() != QEvent::FileOpen)
      return QObject::eventFilter(watched, event);

    const QUrl url = static_cast<QFileOpenEvent*>(event)->url();

    if (started)
      info.importYaml(url);
    else
      startupDocuments.append(url.toLocalFile());

    return true;
  }

private:
  Information& info;
};

}
#endif


int main(int argc, char *argv[])
{
  QGuiApplication a(argc, argv);

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
  Information info(nullptr);
  information = &info;

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
        and translator.load(":/translations/ZeGrapher_" + QLocale::languageToCode(lang) + ".qm"))
      a.installTranslator(&translator);

    engine.retranslate();
  };

  // QCommandLineParser::process() prints the help and exits, so the translator
  // must be installed before the tr() strings below
  applyLanguage();
  QObject::connect(&info.appSettings, &ZeAppSettings::languageChanged, &engine, applyLanguage);

  QCommandLineParser parser;
  parser.setApplicationDescription(QObject::tr("Math plotting software"));
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument(QObject::tr("file"),
                               QObject::tr("ZeGrapher (.zg) document(s) to open on startup"));
  parser.process(a);

  QStringList documents = parser.positionalArguments();

#ifdef Q_OS_MACOS
  DocumentOpener opener(info);
  a.installEventFilter(&opener);

  // processEvents() runs [NSApp finishLaunching], which sends a QFileOpenEvent
  // for each document that started the app
  QCoreApplication::processEvents();
  documents += opener.startupDocuments;
  opener.started = true;
#endif

  if (not documents.isEmpty())
    for (const QString& document: documents)
      info.importYaml(QUrl::fromLocalFile(document));
  else if (not info.restoreLastDocument())
    info.loadExampleDocument();

  engine.load("qrc:///qt/qml/ZeGrapher/MainWindow.qml");

  return a.exec();
}
