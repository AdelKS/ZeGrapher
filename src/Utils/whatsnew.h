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

#include <QVariantList>
#include <QtQmlIntegration/qqmlintegration.h>

#include "Utils/versionparser.h"

#include <glaze/yaml.hpp>

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

/// @brief one heading of appdata/release-notes.md, the way appdata/releases.py
///        writes it to the YAML file that the app embeds
struct NoteYaml
{
  /// @brief the tag the heading starts after, 'v3.1.1'. A heading that names
  ///        one release repeats its tag here
  std::string after;

  /// @brief the newest tag the heading names, 'v4.0.0'
  std::string newest;

  /// @brief the markdown written under the heading
  std::string summary;
};

/// @brief every heading of the notes file, newest first
/// @param yaml  the file that appdata/releases.py writes, which checked every
///              heading already
inline QList<Note> readNotes(const QString& yaml)
{
  std::vector<NoteYaml> parsed;
  if (glz::read_yaml(parsed, yaml.toStdString()))
    return {};

  QList<Note> found;

  for (const NoteYaml& note: parsed)
  {
    const QString afterTag = QString::fromStdString(note.after);
    const QString newestTag = QString::fromStdString(note.newest);

    const auto after = parse_version_string(afterTag);
    const auto newest = parse_version_string(newestTag);
    if (not after or not newest)
      continue;

    found.append({.after = *after, .newest = *newest, .newestTag = newestTag,
                  .heading = afterTag == newestTag ? newestTag
                                                   : afterTag + " - " + newestTag,
                  .summary = QString::fromStdString(note.summary)});
  }

  return found;
}

/// @brief the cards to show, newest first
/// @param notes  the YAML file that appdata/releases.py writes
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
/// @param notes  the YAML file that appdata/releases.py writes
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

/// @brief the releases that the app shows the first time it runs after an update
///
/// The releases come from appdata/release-notes.md. appdata/releases.py writes
/// its headings to a YAML file, which meson embeds in the binary.
class WhatsNew : public QObject
{
  Q_OBJECT
  QML_ELEMENT

  /// @brief the version of ZeGrapher the reader ran last, empty on a first start
  Q_PROPERTY(QString since READ getSince WRITE setSince NOTIFY entriesChanged)

  /// @brief true to list every release, instead of the ones after 'since'
  Q_PROPERTY(bool everyRelease READ getEveryRelease WRITE setEveryRelease NOTIFY entriesChanged)

  /// @brief the releases to show, newest first, each one a
  ///        {tag, span, summary, released} map for QML. Empty when the reader
  ///        has seen them all
  Q_PROPERTY(QVariantList entries READ getEntries NOTIFY entriesChanged)

public:
  explicit WhatsNew(QObject* parent = nullptr);

  QString getSince() const { return since; }
  bool getEveryRelease() const { return everyRelease; }
  QVariantList getEntries() const { return entries; }

  void setSince(QString version);
  void setEveryRelease(bool every);

signals:
  void entriesChanged();

private:
  void load();

  QString since;
  bool everyRelease = false;
  QVariantList entries;
};

}
