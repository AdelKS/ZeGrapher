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

#include <QLocale>
#include <QRegularExpression>
#include <QStringList>
#include <QUrl>
#include <QVariantList>
#include <QtQmlIntegration/qqmlintegration.h>

#include "Utils/versionparser.h"

namespace zg {

/// @brief one entry to draw: one release, or the span that one note covers
struct Release
{
  /// @brief 'v4.0.0', the newest tag of the entry
  QString tag;

  /// @brief the tags that appdata/release-notes.md writes over the entry,
  ///        such as 'v3.1.1 - v4.0.0', the day of the heading taken off
  QString span;

  /// @brief the markdown under that heading
  QString summary;
};

/// @brief one '## <from> - <to>' heading of appdata/release-notes.md
struct Note
{
  /// @brief '<from>', the release the span counts its changes from
  Version after;

  /// @brief '<to>', the newest release the span holds
  Version newest;

  /// @brief '<to>' the way the heading writes it, such as 'v4.0.0'
  QString newestTag;

  /// @brief the heading the way the file writes it, such as 'v3.1.1 - v4.0.0'
  QString heading;

  /// @brief the markdown under the heading
  QString summary;
};

/// @brief the '## <tag>' sections of one file, in the order of the file
/// @returns one pair per section: the tag, and the text under it
inline QList<std::pair<QString, QString>> releaseSections(const QString& markdown)
{
  QList<std::pair<QString, QString>> sections;
  QStringList body;

  auto flush = [&]()
  {
    if (not sections.isEmpty())
      sections.back().second = body.join('\n').trimmed();
    body.clear();
  };

  for (const QString& line: markdown.split('\n'))
  {
    if (line.startsWith("## "))
    {
      flush();
      sections.append({line.sliced(3).trimmed(), QString()});
    }
    else if (not sections.isEmpty())
      body.append(line);
  }
  flush();

  return sections;
}

/// @brief the day that closes a heading, as ' (2026-09-01)'
///
/// The day holds dashes of its own, so it comes off before the tags are split.
/// A software centre shows the day, and the app shows the tags alone.
inline const QRegularExpression headingDay(R"(\s*\([0-9]{4}-[0-9]{2}-[0-9]{2}\)$)");

/// @brief the headings of the notes file, newest first
///
/// A heading names one tag, or the two tags of a span, as 'v3.1.1 - v4.0.0',
/// and then the day the release went out. A heading that names no tag drops
/// out, and so does one that names more than two.
inline QList<Note> readNotes(const QString& notes)
{
  QList<Note> found;

  for (const auto& [heading, summary]: releaseSections(notes))
  {
    const QString span = QString(heading).remove(headingDay).trimmed();
    const QStringList tags = span.split('-');
    if (tags.size() > 2)
      continue;

    const auto after = parse_version_string(tags.first().trimmed());
    const auto newest = parse_version_string(tags.last().trimmed());

    if (after and newest and *after <= *newest)
      found.append({.after = *after, .newest = *newest,
                    .newestTag = tags.last().trimmed(),
                    .heading = span, .summary = summary});
  }

  return found;
}

/// @brief the entries to draw, newest first
/// @param notes  appdata/release-notes.md
/// @param floor  the entries at or under this version drop out
///
/// One heading of the notes file makes one entry. A heading can name a span of
/// releases, and that span comes out as a single entry.
inline QList<Release> buildReleases(const QString& notes,
                                    const std::optional<Version>& floor)
{
  QList<Release> found;

  for (const Note& note: readNotes(notes))
  {
    // the file writes the headings newest first, so nothing under this is left
    if (floor and note.newest <= *floor)
      break;

    found.append({.tag = note.newestTag, .span = note.heading,
                  .summary = note.summary});
  }

  return found;
}

/// @brief every entry of the notes file, newest first
inline QList<Release> allReleases(const QString& notes)
{
  return buildReleases(notes, {});
}

/// @brief the releases that came after the version the reader saw last
/// @param notes  appdata/release-notes.md
/// @param since  the version the reader saw last, empty for a first start
///
/// A first start shows the newest release alone, and a version that no release
/// comes after gives nothing. A version that parse_version_string() cannot read
/// counts as a first start.
inline QList<Release> releasesSince(const QString& notes, const QString& since)
{
  const auto seen = parse_version_string(since);

  // a first start shows the newest entry, and nothing of the ones before it
  if (not seen)
  {
    const auto every = allReleases(notes);
    return every.isEmpty() ? every : QList<Release>{every.first()};
  }

  return buildReleases(notes, seen);
}

/// @brief what the app shows at the first start after an update
///
/// The releases come from appdata/release-notes.md, and the words that ask for
/// a donation from the download panel of the site, which
/// website/content/<lang>/donation.md holds in every language.
class WhatsNew : public QObject
{
  Q_OBJECT
  QML_ELEMENT

  /// @brief the version of ZeGrapher that the reader saw last, empty for a first start
  Q_PROPERTY(QString since READ getSince WRITE setSince NOTIFY entriesChanged)

  /// @brief every release, in place of the ones that came after 'since'
  Q_PROPERTY(bool everyRelease READ getEveryRelease WRITE setEveryRelease NOTIFY entriesChanged)

  /// @brief QLocale::Language of the interface. A language that has no
  ///        translation of the donation words falls back to English
  Q_PROPERTY(int language READ getLanguage WRITE setLanguage NOTIFY donationChanged)

  /// @brief the entries to show, newest first: {tag, span, summary}. Empty
  ///        when the reader saw them all
  Q_PROPERTY(QVariantList entries READ getEntries NOTIFY entriesChanged)

  /// @brief the markdown that asks for a donation
  Q_PROPERTY(QString donation READ getDonation NOTIFY donationChanged)

  /// @brief the page that the donation button opens
  Q_PROPERTY(QUrl donationUrl READ getDonationUrl CONSTANT)

public:
  explicit WhatsNew(QObject* parent = nullptr);

  QString getSince() const { return since; }
  bool getEveryRelease() const { return everyRelease; }
  int getLanguage() const { return language; }
  QVariantList getEntries() const { return entries; }
  QString getDonation() const;
  QUrl getDonationUrl() const;

  void setSince(QString version);
  void setEveryRelease(bool every);
  void setLanguage(int lang);

signals:
  void entriesChanged();
  void donationChanged();

private:
  void load();

  QString since;
  bool everyRelease = false;
  int language = QLocale::English;
  QVariantList entries;
};

}
