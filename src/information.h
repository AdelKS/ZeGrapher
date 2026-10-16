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

#pragma once

#include <QtQmlIntegration/qqmlintegration.h>

#include "Utils/qmlsingleton.h"

#include <zecalculator/zecalculator.h>

#include "MathObjects/mathworld.h"
#include "Utils/appsettings.h"
#include "Utils/graphsettings.h"

namespace zg {
  struct PlotStyle;
}

struct IOError {
  Q_GADGET
  QML_VALUE_TYPE(ioError)

  Q_PROPERTY(QString title MEMBER title)
  Q_PROPERTY(QString file MEMBER file)
  Q_PROPERTY(QString text MEMBER text)
  Q_PROPERTY(QString details MEMBER details)

public:
  QString title;
  QString file = {};
  QString text;
  QString details = {};
};

///@note must be instanced after QGuiApplication (instanced in main.cpp)
class Information: public QObject
{
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(ZeAppSettings* appSettings READ getAppSettingsPtr CONSTANT)
  Q_PROPERTY(ZeGraphSettings* graphSettings READ getGraphSettingsPtr CONSTANT)
  Q_PROPERTY(int ioErrorCount READ getIoErrorCount NOTIFY ioErrorCountChanged)

  /// @brief the file of the last saveAs(), which save() writes to. Empty until
  ///        the first saveAs(): loading a document adds to the open one
  Q_PROPERTY(QString documentName READ getDocumentName NOTIFY documentFileChanged)

  /// @brief the ZeGrapher version that wrote the settings file, empty if no
  ///        run wrote one
  Q_PROPERTY(QString lastRunVersion MEMBER lastRunVersion CONSTANT)

public:
  /// @brief QML uses the global instance instead of creating one
  static Information* create(QQmlEngine*, QJSEngine*);

  explicit Information(QObject* parent);
  ~Information();

  ZeAppSettings* getAppSettingsPtr() { return &appSettings; }
  ZeGraphSettings* getGraphSettingsPtr() { return &graphSettings; }
  int getIoErrorCount() const { return ioErrors.size(); }
  QString getDocumentName() const;

  Q_INVOKABLE IOError popIoError();

signals:
  void ioErrorCountChanged();
  void documentFileChanged();

public slots:
  /// @brief writes the objects and graph settings to a document
  /// @returns false when it reported an error
  bool exportYaml(QUrl filename);

  /// @brief exportYaml(), and save() writes to the same file afterwards
  void saveAs(QUrl filename);

  /// @brief writes the document to the file of the last saveAs()
  void save();

  /// @brief adds a document's objects to the open ones, and takes its graph
  ///        settings
  /// @note the app settings only come from the settings file
  void importYaml(QUrl filename);

public:
  /// @brief reads the document of the last run
  /// @returns false when no run has written one yet
  bool restoreLastDocument();

  /// @brief loads the objects that a first start shows
  void loadExampleDocument();

  /// @brief reports that an export could not write its file
  ///
  /// The classes that export a file write it themselves, and their errors
  /// show in the same dialog as document errors.
  /// @param details  what went wrong
  void exportFailed(const QString& file, const QString& details = {});

  /// @brief reports that a file could not be read
  /// @param details  what went wrong, as the system words it
  void readFailed(const QString& file, const QString& details = {});

public:
  ZeAppSettings appSettings;
  ZeGraphSettings graphSettings;

  /// @brief read from the settings file when Information is built
  QString lastRunVersion;

  /// @brief a document: the objects and graph settings
  struct POD {
    /// @brief version of ZeGrapher that wrote the document, always serialized
    ///        first: content-based MIME type detection matches on the first line,
    ///        see appdata/ZeGrapher.mime.xml
    std::optional<std::string> zegrapher;
    std::optional<zg::MathWorld::POD> math_objects;
    std::optional<ZeGraphSettings::POD> graph;
  };

  /// @brief the first key of a document, read alone for the version
  struct HeaderPOD {
    std::optional<std::string> zegrapher;
  };

  /// @brief the settings file, which holds the app settings
  struct SettingsPOD {
    /// @brief version of ZeGrapher that wrote the file
    std::optional<std::string> zegrapher;
    std::optional<ZeAppSettings::POD> app;
  };

protected:
  /// @brief writes the document that the next run reopens
  void saveLastDocument();

  /// @brief writes the app settings, which the next run reads back
  void saveSettings();

  /// @brief reads the settings that the last run wrote
  /// @note the app writes the settings file on every close. If the file can't
  ///       be read, the app keeps its defaults, and the next close rewrites it
  void restoreSettings();

  /// @brief writes a POD or a SettingsPOD to a file, and reports a failure as a
  ///        document that could not be saved
  template <class Pod>
  bool writeYaml(QUrl filename, const Pod& pod);

  void appendIoErr(IOError err);
  QList<IOError> ioErrors;

  QUrl documentFile;
};

/// @brief set by Globals
inline Information* information = nullptr;

inline Information* Information::create(QQmlEngine*, QJSEngine*)
{
  return zg::qmlSingleton(information);
}
