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
#include <QVariantList>
#include <QtQmlIntegration/qqmlintegration.h>

namespace zg {

/// @brief splits a language's manual into the blocks that DocPane draws
///
/// The viewer never gets the manual as one piece of markdown: a MarkdownText
/// TextEdit loses the top of a long page with pictures once it scrolls out of
/// view, and never draws it again. So each picture becomes its own block. Each
/// heading also becomes a block, which links jump to and the viewer puts space
/// around.
///
/// The manual is one file per language, so its links point at a heading in the
/// same file, or out of the app.
class Documentation : public QObject
{
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit Documentation(QObject* parent = nullptr): QObject(parent) {}

  /// @brief the manual of a language in order, one map per block:
  ///        {kind: 'text', text, anchor} or {kind: 'image', source, width, height}
  ///        A text block with an anchor holds one heading and nothing else.
  /// @param language  a QLocale::Language. Falls back to English when the manual
  ///                  has no translation for it
  Q_INVOKABLE QVariantList blocks(int language) const;
};

}
