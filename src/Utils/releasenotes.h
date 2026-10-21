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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <QVariantList>
#include <QVariantMap>

#include "Utils/versionparser.h"

#include <glaze/yaml.hpp>

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

namespace zg {

/// @brief one release notes card, built from one announcement or changes entry
///        of appdata/releases.yaml
struct Release
{
  /// @brief the newest release the entry covers
  Version newest;

  /// @brief that newest release as the entry writes it, 'v4.0.0'. The card
  ///        uses it as its title, and its button opens that GitHub release
  QString tag;

  /// @brief the span of the entry, 'v3.1.1 - v4.0.0'. Empty for an entry about
  ///        one release. The card shows it under the title
  QString span;

  /// @brief the markdown of the entry
  QString summary;

  /// @brief true when the release is out, so its GitHub page exists. The card
  ///        only shows its button then
  bool released = false;
};

/// @brief true when the release of that tag is out
///
/// A release newer than the build has no GitHub page yet, and an entry can name
/// one: a pre-release sits under the announcement of the release it leads to.
inline bool isReleased(const Version& tag,
                       const std::optional<Version>& build = build_version())
{
  return build and tag <= *build;
}

/// @brief one announcement or changes entry of appdata/releases.yaml
struct NoteYaml
{
  /// @brief the release the span counts from, which stays out of it. Empty for
  ///        an entry about 'to' alone
  std::string from;

  /// @brief the newest release of the span, 'v4.0.0'
  std::string to;

  /// @brief the markdown of the entry
  std::string text;
};

/// @brief one item of the 'releases' list, which only appdata/releases.py reads
struct ReleaseDateYaml
{
  std::string tag;
  std::string date;
};

/// @brief appdata/releases.yaml
struct ReleasesYaml
{
  std::vector<ReleaseDateYaml> releases;
  std::vector<NoteYaml> announcements;
  std::vector<NoteYaml> changes;
};

/// @brief every entry that holds a release up to the build, newest first
/// @param notes  appdata/releases.yaml, which appdata/releases.py checks before
///               the build embeds it
///
/// One entry gives one card, a span of releases included. An announcement comes
/// above a changes entry that ends on the same release.
inline QList<Release> allReleases(const QString& notes,
                                  const std::optional<Version>& build = build_version())
{
  ReleasesYaml parsed;
  if (glz::read_yaml(parsed, notes.toStdString()))
    return {};

  QList<Release> found;

  for (const auto* entries: {&parsed.announcements, &parsed.changes})
    for (const NoteYaml& note: *entries)
    {
      const QString tag = QString::fromStdString(note.to);
      const QString from = QString::fromStdString(note.from);

      const auto newest = parse_version_string(tag);
      const auto after = from.isEmpty() ? newest : parse_version_string(from);
      if (not newest or not after)
        continue;

      // an entry holds the releases after 'from' up to 'to', or 'to' alone.
      // Skip an entry that holds no release up to the build
      if (build and (from.isEmpty() ? *newest > *build : *after >= *build))
        continue;

      found.append({.newest = *newest, .tag = tag,
                    .span = from.isEmpty() ? QString() : from + " - " + tag,
                    .summary = QString::fromStdString(note.text).trimmed(),
                    .released = isReleased(*newest, build)});
    }

  // the announcements come first, and a stable sort keeps them first on a tie
  std::ranges::stable_sort(found, std::ranges::greater{}, &Release::newest);

  return found;
}

/// @brief the entries about releases since the user last ran the app
/// @param notes  appdata/releases.yaml
/// @param since  the last version the user ran, empty on a first start
///
/// A first start only gives the newest entry. The list is empty when no
/// release came after 'since', or when 'since' is this build or newer. A
/// 'since' that isn't a version counts as a first start.
inline QList<Release> releasesSince(const QString& notes, const QString& since,
                                    const std::optional<Version>& build = build_version())
{
  const auto seen = parse_version_string(since);

  // the user already ran this build or a newer one, so nothing is new. A
  // pre-release sits under the announcement of the release it leads to, and
  // without this check that announcement would come back at every start
  if (seen and build and *seen >= *build)
    return {};

  QList<Release> found = allReleases(notes, build);

  // on a first start, show the newest entry and nothing older
  if (not seen)
    return found.mid(0, 1);

  // the entries go newest first, so every entry from here on is older than
  // 'seen'
  const auto older = std::ranges::find_if(found, [&](const Release& release) {
    return release.newest <= *seen;
  });
  found.erase(older, found.end());

  return found;
}

/// @brief the releases as the {tag, span, summary, released} maps that QML reads
inline QVariantList toVariantList(const QList<Release>& releases)
{
  QVariantList maps;
  for (const Release& release: releases)
    maps.append(QVariantMap{{"tag", release.tag},
                            {"span", release.span},
                            {"summary", release.summary},
                            {"released", release.released}});

  return maps;
}

}
