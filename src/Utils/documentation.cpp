#include "Utils/documentation.h"

#include <QFile>
#include <QHash>
#include <QImageReader>
#include <QLocale>
#include <QRegularExpression>

#include "structures.h"

namespace zg {

namespace {

/// @brief the manual, under ':/website/<lang>/'. website/meson.build puts the
///        documentation panel of the website there, one file per language
const QString manualFile = "documentation.md";

/// @brief a paragraph that holds one picture and nothing else
const QRegularExpression picture(R"(^!\[([^\]]*)\]\(([^)]+)\)$)");

/// @brief the '#' and the spaces a heading starts with
const QRegularExpression hashes(R"(^#+\s*)");

/// @brief the anchor of a heading, the way pandoc and GitHub write it
QString slugOf(const QString& heading)
{
  // the rule of gfm_auto_identifiers: lower case, drop every character that is
  // neither a letter, a number, a mark, a '-' nor a connector such as '_', then
  // turn each space into one dash. A run of spaces gives a run of dashes, so
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

}

Documentation::Documentation(QObject* parent): QObject(parent)
{
  load();
}

void Documentation::setLanguage(int lang)
{
  if (language == lang)
    return;

  language = lang;
  load();

  emit languageChanged();
}

void Documentation::load()
{
  const QString dir = websiteFolder(QLocale::Language(language));

  // ':/website/en' is what QFile reads, 'qrc:/website/en/' what the pictures
  // of the manual resolve against
  blocks = cut(readTextFile(dir + '/' + manualFile), QUrl("qrc" + dir + '/'));

  emit blocksChanged();
}

QVariantList Documentation::cut(const QString& text, const QUrl& base) const
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

      // the second heading of the same text takes '-1', the third '-2', as on the site
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

      // a heading takes a block of its own, so the viewer can put space around
      // it. MarkdownText gives it no margin that QML can set
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

int Documentation::blockOfAnchor(QString anchor) const
{
  if (anchor.startsWith('#'))
    anchor = anchor.sliced(1);

  for (qsizetype i = 0; i != blocks.size(); i++)
    if (blocks[i].toMap().value("anchor").toString() == anchor)
      return int(i);

  return -1;
}

}
