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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "highlighter.h"
#include "information.h"

#include <QGuiApplication>
#include <QStyleHints>

#include <zecalculator/zecalculator.h>

Highlighter::Highlighter(QObject* parent): QSyntaxHighlighter(parent) {
  connect(&information->appSettings, &ZeAppSettings::invalidSyntaxChanged, this, &Highlighter::rehighlight);
  connect(qGuiApp->styleHints(), &QStyleHints::colorSchemeChanged, this, &Highlighter::rehighlight);
}

void Highlighter::setState(zg::State s)
{
  if (state != s)
  {
    state = s;
    rehighlight();
  }
}

void Highlighter::highlightBlock(const QString& text)
{
  const auto token = state.getErrToken();
  if (not token or token->begin < size_t(offset))
    return;

  QTextCharFormat invalidFormat;
  invalidFormat.setForeground(information->appSettings.invalidSyntax.getCurrent());
  invalidFormat.setFontUnderline(true);
  invalidFormat.setUnderlineColor(information->appSettings.invalidSyntax.getCurrent());
  invalidFormat.setUnderlineStyle(QTextCharFormat::UnderlineStyle::WaveUnderline);

  // ZeCalculator counts UTF-8 bytes, and setFormat() counts UTF-16 code units
  const QByteArray utf8 = text.toUtf8();
  const qsizetype begin = std::min(qsizetype(token->begin) - offset, utf8.size());
  const qsizetype end = std::min(begin + qsizetype(token->substr.size()), utf8.size());

  setFormat(QString::fromUtf8(utf8.first(begin)).size(),
            QString::fromUtf8(utf8.sliced(begin, end - begin)).size(),
            invalidFormat);
}

void Highlighter::setDocument(QQuickTextDocument* doc)
{
  textDocument = doc;
  QSyntaxHighlighter::setDocument(doc->textDocument());
}
