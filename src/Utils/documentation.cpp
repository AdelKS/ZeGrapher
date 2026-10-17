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

#include "Utils/documentation.h"

#include <QHash>
#include <QImageReader>
#include <QLocale>
#include <QRegularExpression>
#include <QUrl>

#include "Utils/resources.h"

namespace zg {

namespace {

/// @brief the manual, under ':/website/<lang>/'. website/meson.build puts the
///        website's documentation panel there, one file per language
const QString manualFile = "documentation.md";

/// @brief a paragraph that holds one picture and nothing else
const QRegularExpression picture(R"(^!\[([^\]]*)\]\(([^)]+)\)$)");

/// @brief matches the '#' characters and spaces that start a heading
const QRegularExpression hashes(R"(^#+\s*)");

/// @brief a heading's anchor, the way pandoc and GitHub write it
QString slugOf(const QString& heading)
{
  // gfm_auto_identifiers works this way: lowercase the heading, drop every
  // character that isn't a letter, number, mark, '-' or connector such as '_',
  // and turn each space into a dash. A run of spaces gives a run of dashes, so
  // 'Grid tab — ticks' gives 'grid-tab--ticks'
  QString slug;
  for (const QChar c: heading.trimmed())
  {
    if (c.isSpace())
      slug += '-';
    else if (c.isLetterOrNumber() or c.isMark() or c == '-'
             or c.category() == QChar::Punctuation_Connector)
      slug += c.toLower();
  }
  return slug;
}

/// @brief splits the markdown at every heading and picture
QVariantList cut(const QString& text, const QUrl& base)
{
  QVariantList cutBlocks;
  QStringList buffer;
  QHash<QString, int> taken;
  bool fenced = false;

  auto flush = [&]()
  {
    const QString block = buffer.join('\n').trimmed();
    buffer.clear();

    if (block.isEmpty())
      return;

    QString anchor;
    if (block.startsWith('#'))
    {
      const qsizetype end = block.indexOf('\n');
      QString heading = end < 0 ? block : block.first(end);
      anchor = slugOf(heading.remove(hashes));

      // two headings with the same text: the second gets '-1', the third '-2',
      // as on the site
      if (const int seen = taken[anchor]++; seen > 0)
        anchor += '-' + QString::number(seen);
    }

    cutBlocks.append(QVariantMap{{"kind", "text"}, {"text", block}, {"anchor", anchor}});
  };

  for (const QString& line: text.split('\n'))
  {
    if (line.startsWith("```"))
      fenced = not fenced;

    if (not fenced)
    {
      if (const auto match = picture.match(line.trimmed()); match.hasMatch())
      {
        flush();

        const QUrl source = base.resolved(QUrl(match.captured(2)));
        const QString path = source.scheme() == "qrc" ? ':' + source.path() : source.toLocalFile();
        const QSize size = QImageReader(path).size();

        cutBlocks.append(QVariantMap{{"kind", "image"},
                                     {"source", source},
                                     {"width", size.width()},
                                     {"height", size.height()}});
        continue;
      }

      // each heading becomes its own block, so the viewer can put space around
      // it: MarkdownText has no heading margin that QML can set
      if (line.startsWith('#'))
      {
        flush();
        buffer.append(line);
        flush();
        continue;
      }
    }

    buffer.append(line);
  }
  flush();

  return cutBlocks;
}

}

QVariantList Documentation::blocks(int language) const
{
  const QString dir = websiteFolder(QLocale::Language(language));

  // QFile reads ':/website/en', and the manual's pictures resolve against
  // 'qrc:/website/en/'
  return cut(readTextFile(dir + '/' + manualFile), QUrl("qrc" + dir + '/'));
}

}
