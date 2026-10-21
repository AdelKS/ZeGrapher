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

#include "Utils/textdocumenttools.h"

#include <QTextBlock>
#include <QTextCursor>

namespace zg {

void TextDocumentTools::colorLinks(QQuickTextDocument* doc, const QColor& color,
                                   const QColor& hoveredColor, const QString& hoveredUrl)
{
  if (not doc or not doc->textDocument())
    return;

  QTextDocument* document = doc->textDocument();

  struct Recolor
  {
    int start;
    int end;
    QColor color;
  };

  QList<Recolor> recolors;
  for (QTextBlock block = document->begin(); block.isValid(); block = block.next())
    for (auto it = block.begin(); not it.atEnd(); ++it)
    {
      const QTextFragment fragment = it.fragment();
      const QTextCharFormat format = fragment.charFormat();
      if (not format.isAnchor())
        continue;

      const QColor wanted = format.anchorHref() == hoveredUrl ? hoveredColor : color;
      if (format.foreground().color() != wanted)
        recolors.append({fragment.position(), fragment.position() + fragment.length(), wanted});
    }

  if (recolors.isEmpty())
    return;

  // each format edit pushes a QTextDocument undo command, and every hover recolors a link
  const bool undo = document->isUndoRedoEnabled();
  document->setUndoRedoEnabled(false);

  QTextCursor cursor(document);
  cursor.beginEditBlock();
  for (const auto& [start, end, wanted]: recolors)
  {
    cursor.setPosition(start);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    QTextCharFormat format;
    format.setForeground(wanted);
    cursor.mergeCharFormat(format);
  }
  cursor.endEditBlock();

  document->setUndoRedoEnabled(undo);
}

void TextDocumentTools::keepOnOneLine(QQuickTextDocument* doc)
{
  if (not doc or not doc->textDocument())
    return;

  QTextDocument* document = doc->textDocument();

  connect(document, &QTextDocument::contentsChange, document, [document](int pos, int, int added) {
    // the document ends with a block separator that is not part of the text
    const int end = std::min(pos + added, document->characterCount() - 1);

    QTextCursor cursor;
    for (int i = pos; i < end; i++)
    {
      const QChar c = document->characterAt(i);
      if (c != QChar::ParagraphSeparator and c != QChar::LineSeparator)
        continue;

      if (cursor.isNull())
      {
        cursor = QTextCursor(document);
        cursor.joinPreviousEditBlock();
      }
      cursor.setPosition(i);
      cursor.setPosition(i + 1, QTextCursor::KeepAnchor);
      cursor.insertText(" ");
    }

    if (not cursor.isNull())
      cursor.endEditBlock();
  });
}

}
