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

/// @brief one card of the "What's new" list, built from one heading of
///        appdata/release-notes.md
struct Release
{
  /// @brief the newest tag the heading names, 'v4.0.0'. The card shows it as
  ///        its title, and its button opens that release on GitHub
  QString tag;

  /// @brief the heading without its date, 'v3.1.1 - v4.0.0'. The card shows it
  ///        under the title, and only when the heading covers a span
  QString span;

  /// @brief the markdown written under the heading
  QString summary;

  /// @brief true when the release is out, so its page on GitHub is up. The
  ///        card shows its button only then
  bool released = false;
};

/// @brief one heading of appdata/release-notes.md, read into its parts
struct Note
{
  /// @brief the release the heading starts after, which stays out of the span.
  ///        A heading that names one tag repeats that tag here
  Version after;

  /// @brief the newest release the heading covers
  Version newest;

  /// @brief that newest release as the heading writes it, 'v4.0.0'
  QString newestTag;

  /// @brief the whole heading without its date, 'v3.1.1 - v4.0.0'
  QString heading;

  /// @brief the markdown written under the heading
  QString summary;
};

/// @brief the release this build was made from, or nothing when
///        SOFTWARE_VERSION names no version
inline const std::optional<Version>& buildVersion()
{
  static const std::optional<Version> version = parse_version_string(SOFTWARE_VERSION);
  return version;
}

/// @brief true when this build sits between two releases
///
/// parse_version_string() reads a '-dev' version as the release it comes
/// before, so buildVersion() alone does not tell the two apart.
inline bool buildIsDev()
{
  static const bool dev = QString(SOFTWARE_VERSION).endsWith("-dev");
  return dev;
}

/// @brief true when the release of that tag is out
///
/// A release newer than this build has no page on GitHub yet, and a heading of
/// the notes file can name one: a pre-release sits under the release that its
/// span leads to. A '-dev' build comes before the release it names, so that
/// release is not out either.
inline bool isReleased(const Version& tag)
{
  return buildVersion() and (buildIsDev() ? tag < *buildVersion()
                                          : tag <= *buildVersion());
}

/// @brief splits a markdown file at its '## ' headings
/// @returns one pair per section, in the order of the file: the text of the
///          heading, and the markdown under it
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

/// @brief matches the date that closes a heading, ' (2026-09-01)'
///
/// A date holds dashes, and a span heading is split on a dash, so the date is
/// removed first. The metainfo file keeps the date for software centres, and
/// the app shows the tags only.
inline const QRegularExpression headingDay(R"(\s*\([0-9]{4}-[0-9]{2}-[0-9]{2}\)$)");

/// @brief every heading of the notes file, newest first
///
/// A heading names one tag, or two tags around a dash, then the date:
/// '## v3.1.1 - v4.0.0 (2026-09-01)'. A heading is skipped when it names more
/// than two tags, when a tag is not a version, or when the two are in the wrong
/// order.
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

/// @brief the cards to show, newest first
/// @param notes  the content of appdata/release-notes.md
/// @param floor  releases up to and including this version are left out
///
/// One heading gives one card, a span of releases included.
inline QList<Release> buildReleases(const QString& notes,
                                    const std::optional<Version>& floor)
{
  QList<Release> found;

  for (const Note& note: readNotes(notes))
  {
    // the headings run newest first, so every heading left is older than floor
    if (floor and note.newest <= *floor)
      break;

    found.append({.tag = note.newestTag, .span = note.heading,
                  .summary = note.summary, .released = isReleased(note.newest)});
  }

  return found;
}

/// @brief every release of the notes file, newest first
inline QList<Release> allReleases(const QString& notes)
{
  return buildReleases(notes, {});
}

/// @brief the releases that came out after the reader last ran the app
/// @param notes  the content of appdata/release-notes.md
/// @param since  the version the reader ran last, empty on a first start
///
/// A first start gives the newest release alone. The list is empty when no
/// release came after 'since', and when 'since' is the version of this build.
/// A 'since' that is not a version counts as a first start.
inline QList<Release> releasesSince(const QString& notes, const QString& since)
{
  const auto seen = parse_version_string(since);

  // the reader already ran this build, so nothing came out since. A
  // pre-release sits under the release that its span leads to, and without
  // this test it would show that span again at every start
  if (seen and seen == buildVersion())
    return {};

  // on a first start, show the newest release and nothing older
  if (not seen)
  {
    const auto every = allReleases(notes);
    return every.isEmpty() ? every : QList<Release>{every.first()};
  }

  return buildReleases(notes, seen);
}

/// @brief what the app shows the first time it runs after an update
///
/// The releases come from appdata/release-notes.md, which meson embeds in the
/// binary. The words that ask for a donation come from the 'donation' of
/// website/content/<lang>/strings.yaml, the same string the download panel of
/// the site shows.
class WhatsNew : public QObject
{
  Q_OBJECT
  QML_ELEMENT

  /// @brief the version of ZeGrapher the reader ran last, empty on a first start
  Q_PROPERTY(QString since READ getSince WRITE setSince NOTIFY entriesChanged)

  /// @brief true to list every release, instead of the ones after 'since'
  Q_PROPERTY(bool everyRelease READ getEveryRelease WRITE setEveryRelease NOTIFY entriesChanged)

  /// @brief QLocale::Language of the interface. It picks the language of the
  ///        donation words, and English stands in for a language that has no
  ///        translation of them
  Q_PROPERTY(int language READ getLanguage WRITE setLanguage NOTIFY donationChanged)

  /// @brief the releases to show, newest first, each one a {tag, span, summary}
  ///        map for QML. Empty when the reader has seen them all
  Q_PROPERTY(QVariantList entries READ getEntries NOTIFY entriesChanged)

  /// @brief the markdown that asks for a donation
  Q_PROPERTY(QString donation READ getDonation NOTIFY donationChanged)

  /// @brief the donation page that the button opens
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
