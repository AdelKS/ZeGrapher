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
#include <QEvent>
#include <QFileOpenEvent>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QLoggingCategory>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QTranslator>

#ifdef Q_OS_MACOS
namespace {

/// @brief opens the documents that macOS hands to the app
///
/// Every other desktop passes a document on the command line. macOS sends a
/// QFileOpenEvent to the application instead, both when a document starts the
/// app and when one is opened while it runs. The event carries one document, so
/// opening several sends several events.
class DocumentOpener: public QObject
{
public:
  explicit DocumentOpener(Information& info): info(info) {}

  /// @brief the documents that arrived before main() picked what to open at
  ///        startup. They take the place of the command line arguments
  QStringList startupDocuments;

  /// @brief false until main() read startupDocuments
  bool started = false;

protected:
  bool eventFilter(QObject* watched, QEvent* event) override
  {
    if (event->type() != QEvent::FileOpen)
      return QObject::eventFilter(watched, event);

    const QUrl url = static_cast<QFileOpenEvent*>(event)->url();

    // once the app runs, a document opens with the same call as the 'Load a
    // ZeGrapher document' button
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

#ifdef Q_OS_MACOS
  // macOS sends the document of a double click as an event. The filter goes in
  // before the first call that processes events, so that none is missed
  DocumentOpener opener(info);
  a.installEventFilter(&opener);
#endif

  // an imported document can override it
  info.appSettings.language = systemLanguage();

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

  // QCommandLineParser::process() prints the help and exits, so the tr()
  // strings under it need the translator installed already
  applyLanguage();

  QCommandLineParser parser;
  parser.setApplicationDescription(QObject::tr("2D math plotter"));
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument(QObject::tr("file"),
                               QObject::tr("ZeGrapher (.zg) document(s) to open on startup"));
  parser.process(a);

  QStringList documents = parser.positionalArguments();

#ifdef Q_OS_MACOS
  // processEvents() makes QCocoaEventDispatcher run [NSApp finishLaunching].
  // AppKit hands over the documents that started the app during that call, and
  // Qt turns them into QFileOpenEvents
  QCoreApplication::processEvents();
  documents += opener.startupDocuments;
  opener.started = true;
#endif

  if (not documents.isEmpty())
    info.openStartupDocuments(documents);
  else if (not info.restoreLastDocument())
    info.loadExampleDocument();

  // a document carries the language it was written with, so this reads the
  // language again once the documents are in
  applyLanguage();
  QObject::connect(&info.appSettings, &ZeAppSettings::languageChanged, &engine, applyLanguage);

  engine.load("qrc:///qt/qml/ZeGrapher/MainWindow.qml");

  return a.exec();
}
