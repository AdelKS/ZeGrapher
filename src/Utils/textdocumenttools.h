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
#include <QQuickTextDocument>
#include <QTextDocument>
#include <QtQmlIntegration/qqmlintegration.h>

namespace zg {

/// @brief the parts of the QTextDocument of a TextEdit that QML reaches none of
///
/// QQuickTextDocument gives its QTextDocument to C++ alone, and Text carries no
/// document at all: a markdown list of a TextEdit is the only one that can
/// take another indent step.
class TextDocumentTools : public QObject
{
  Q_OBJECT
  QML_NAMED_ELEMENT(TextDocumentTools)
  QML_SINGLETON

public:
  explicit TextDocumentTools(QObject* parent = nullptr): QObject(parent) {}

  /// @brief sets the step that one level of list nesting indents by, in pixels
  /// @note QTextDocument::indentWidth is 40 pixels by default. The mark of a
  ///       list item is drawn to the left of its text, and a step under the
  ///       width of that mark and a space clips it
  Q_INVOKABLE void setIndentWidth(QQuickTextDocument* doc, qreal pixels)
  {
    if (doc and doc->textDocument())
      doc->textDocument()->setIndentWidth(pixels);
  }
};

}
