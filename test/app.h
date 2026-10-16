/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "globals.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QStandardPaths>
#include <QTemporaryDir>

#include <memory>
#include <optional>

/// @brief the app's shared objects, without a window. A test's settings files
///        and documents go to a temporary folder
class TestApp
{
public:
  /// @brief where the windows of a test go
  enum Platform { Offscreen, Native };

  explicit TestApp(Platform platform = Offscreen)
  {
    if (platform == Offscreen)
      qputenv("QT_QPA_PLATFORM", "offscreen");
    // Information writes its files under AppConfigLocation when it is destroyed
    qputenv("XDG_CONFIG_HOME", folder.path().toUtf8());
    app = std::make_unique<QGuiApplication>(argc, argv);
    // macOS and Windows ignore XDG_CONFIG_HOME, and meson runs the tests in parallel
    QCoreApplication::setApplicationName(QFileInfo(folder.path()).fileName());
    globals.emplace();
  }

  ~TestApp()
  {
    globals.reset();
    QDir config(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
    if (config.dirName() == QCoreApplication::applicationName())
      config.removeRecursively();
  }

  /// @brief new shared objects, like a fresh app start
  void restart()
  {
    globals.reset();
    globals.emplace();
  }

  /// @brief a path in the temporary folder
  QString path(const QString& name) const { return folder.filePath(name); }

  /// @brief loads the document 'yaml' in a fresh app start
  /// @returns false when the app reported an error
  bool load(const QByteArray& yaml)
  {
    QFile file(path("loaded.zg"));
    if (not file.open(QIODevice::WriteOnly) or file.write(yaml) != yaml.size())
      return false;
    file.close();

    restart();
    information->importYaml(QUrl::fromLocalFile(file.fileName()));
    return information->getIoErrorCount() == 0;
  }

protected:
  QTemporaryDir folder;

  int argc = 1;
  char name[5] = "test";
  char* argv[1] = {name};
  std::unique_ptr<QGuiApplication> app;

  std::optional<Globals> globals;
};
