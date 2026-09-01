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

#include <algorithm>

namespace zg {

/// @brief one card of the "What's new" list, built from one heading of
///        appdata/release-notes.md
struct Release
{
  /// @brief the newest release the heading covers
  Version newest;

  /// @brief that newest release as the heading writes it, 'v4.0.0'. The card
  ///        shows it as its title, and its button opens that release on GitHub
  QString tag;

  /// @brief the heading without its date, 'v3.1.1 - v4.0.0'. Empty when the
  ///        heading names one release. The card shows it under the title
  QString span;

  /// @brief the markdown written under the heading
  QString summary;

  /// @brief true when the release is out, so its page on GitHub is up. The
  ///        card shows its button only then
  bool released = false;
};

/// @brief a build of ZeGrapher, read from its version string
struct Build
{
  explicit Build(const QString& version = SOFTWARE_VERSION)
    : release(parse_version_string(version)), dev(version.endsWith("-dev")) {}

  /// @brief the release the build was made from, or nothing when the version
  ///        names no release
  std::optional<Version> release;

  /// @brief true when the build sits between two releases.
  ///        parse_version_string() reads a '-dev' version as the release it
  ///        comes before, so 'release' alone does not tell the two apart
  bool dev;
};

/// @brief true when the release of that tag is out
///
/// A release newer than the build has no page on GitHub yet, and a heading of
/// the notes file can name one: a pre-release sits under the release that its
/// span leads to. A '-dev' build comes before the release it names, so that
/// release is not out either.
inline bool isReleased(const Version& tag, const Build& build = Build())
{
  return build.release and (build.dev ? tag < *build.release
                                      : tag <= *build.release);
}

/// @brief one heading of appdata/release-notes.md, the way appdata/releases.py
///        writes it to the YAML file that the app embeds
struct NoteYaml
{
  /// @brief the newest tag the heading names, 'v4.0.0'
  std::string newest;

  /// @brief the two tags of a span, 'v3.1.1 - v4.0.0'. Empty when the heading
  ///        names one release
  std::string span;

  /// @brief the markdown written under the heading
  std::string summary;
};

/// @brief every release of the notes file, newest first
/// @param notes  the YAML file that appdata/releases.py writes, which checked
///               every heading already
///
/// One heading gives one card, a span of releases included.
inline QList<Release> allReleases(const QString& notes, const Build& build = Build())
{
  std::vector<NoteYaml> parsed;
  if (glz::read_yaml(parsed, notes.toStdString()))
    return {};

  QList<Release> found;

  for (const NoteYaml& note: parsed)
  {
    const QString tag = QString::fromStdString(note.newest);

    const auto newest = parse_version_string(tag);
    if (not newest)
      continue;

    found.append({.newest = *newest, .tag = tag,
                  .span = QString::fromStdString(note.span),
                  .summary = QString::fromStdString(note.summary),
                  .released = isReleased(*newest, build)});
  }

  return found;
}

/// @brief the releases that came out after the reader last ran the app
/// @param notes  the YAML file that appdata/releases.py writes
/// @param since  the version the reader ran last, empty on a first start
///
/// A first start gives the newest release alone. The list is empty when no
/// release came after 'since', and when 'since' is the version of the build.
/// A 'since' that is not a version counts as a first start.
inline QList<Release> releasesSince(const QString& notes, const QString& since,
                                    const Build& build = Build())
{
  const auto seen = parse_version_string(since);

  // the reader already ran this build, so nothing came out since. A
  // pre-release sits under the release that its span leads to, and without
  // this test it would show that span again at every start
  if (seen and seen == build.release)
    return {};

  QList<Release> found = allReleases(notes, build);

  // on a first start, show the newest release and nothing older
  if (not seen)
    return found.mid(0, 1);

  // the headings run newest first, so every heading from this one on is older
  // than 'seen'
  const auto older = std::ranges::find_if(found, [&](const Release& release) {
    return release.newest <= *seen;
  });
  found.erase(older, found.end());

  return found;
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
  explicit WhatsNew(QObject* parent = nullptr): QObject(parent) {}

  QString getSince() const { return since; }
  bool getEveryRelease() const { return everyRelease; }
  QVariantList getEntries() const;

  void setSince(QString version);
  void setEveryRelease(bool every);

signals:
  void entriesChanged();

private:
  QString since;
  bool everyRelease = false;
};

}
