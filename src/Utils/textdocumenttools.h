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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
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

  /// @brief sets the width of one list indent level, in pixels
  /// @note QTextDocument::indentWidth is 40 pixels by default. A list item's
  ///       mark is drawn left of its text, so a width narrower than the mark
  ///       plus one space clips it
  Q_INVOKABLE void setIndentWidth(QQuickTextDocument* doc, qreal pixels)
  {
    if (doc and doc->textDocument())
      doc->textDocument()->setIndentWidth(pixels);
  }

  /// @brief colors every link with `color`, and the links to `hoveredUrl`
  ///        with `hoveredColor`
  /// @note TextEdit.linkColor doesn't apply to markdown: QTextMarkdownImporter
  ///       gives each link the application palette's Link color, and
  ///       QQuickTextNodeEngine prefers the character color over the item color
  Q_INVOKABLE void colorLinks(QQuickTextDocument* doc, const QColor& color,
                              const QColor& hoveredColor, const QString& hoveredUrl);

  /// @brief from now on, replaces each line break that enters `doc` with a space
  /// @note the replacement joins the undo step of the edit that brought the
  ///       line break, so one undo restores the text from before that edit
  Q_INVOKABLE void keepOnOneLine(QQuickTextDocument* doc);
};

}
