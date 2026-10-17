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

#include "Utils/highlighter.h"
#include "app.h"
#include "utils.h"

#include <QTextBlock>
#include <QTextDocument>
#include <QTextLayout>

int main()
{
  TestApp app;

  // the error token comes after a two-byte character
  const QString text = "f(x) = é + )";

  zg::mathobj::Equation equation;
  equation.setEquation(text);
  test(equation.getState().getErrToken().has_value());

  QTextDocument document(text);
  Highlighter highlighter;
  static_cast<QSyntaxHighlighter&>(highlighter).setDocument(&document);
  highlighter.setState(equation.getState());

  const QList<QTextLayout::FormatRange> formats = document.firstBlock().layout()->formats();
  test(formats.size() == 1);
  test(formats[0].start == text.lastIndexOf(')') and formats[0].length == 1);

  return 0;
}
