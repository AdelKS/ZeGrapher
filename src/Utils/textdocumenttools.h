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

#include <QColor>
#include <QObject>
#include <QQuickTextDocument>
#include <QTextDocument>
#include <QtQmlIntegration/qqmlintegration.h>

namespace zg {

/// @brief the QTextDocument edits that QML cannot make on its own
///
/// QQuickTextDocument only gives its QTextDocument to C++, and a Text item has
/// no document at all. So these functions only work on a TextEdit.
class TextDocumentTools : public QObject
{
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit TextDocumentTools(QObject* parent = nullptr): QObject(parent) {}

  /// @brief colors every link with `color`, and the links to `hoveredUrl`
  ///        with `hoveredColor`
  /// @note TextEdit.linkColor doesn't apply to markdown: QTextMarkdownImporter
  ///       gives each link the application palette's Link color, and
  ///       QQuickTextNodeEngine prefers the character color over the item color
  Q_INVOKABLE void colorLinks(QQuickTextDocument* doc, const QColor& color,
                              const QColor& hoveredColor, const QString& hoveredUrl);
};

}
