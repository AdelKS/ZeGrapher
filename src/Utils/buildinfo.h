#pragma once

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

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "structures.h"

namespace zg {

/// @brief the facts about the build that the About panel shows
///
/// QML reads the version of the app off Qt.application, and Qt exposes its own
/// version to C++ alone. The licence of the bundled font is a file of the
/// resource, which QML has no reader for either.
class BuildInfo : public QObject
{
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  /// @brief the version of Qt that runs the app, which is the one that a
  ///        distribution updates under it
  Q_PROPERTY(QString qtVersion READ getQtVersion CONSTANT)

  /// @brief the licence of the Latin Modern Math font, which the app embeds
  Q_PROPERTY(QString fontLicense READ getFontLicense CONSTANT)

public:
  explicit BuildInfo(QObject* parent = nullptr): QObject(parent) {}

  QString getQtVersion() const { return QString::fromLatin1(qVersion()); }

  QString getFontLicense() const
  {
    return readTextFile(":/fonts/latinmodern-math-license.txt").trimmed();
  }
};

}
