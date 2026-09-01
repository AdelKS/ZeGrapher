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

namespace zg {

/// @brief the donation appeal
///
/// It is the 'donation' key of website/content/<lang>/strings.yaml, the same
/// string as the site's download panel. BuildInfo gives the donation page.
class Donation : public QObject
{
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit Donation(QObject* parent = nullptr): QObject(parent) {}

  /// @brief the donation appeal, in markdown
  /// @param language  a QLocale::Language. Falls back to English when the appeal
  ///                  has no translation for it
  Q_INVOKABLE QString words(int language) const;
};

}
