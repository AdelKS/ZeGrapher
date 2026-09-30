#pragma once

#include "buildconfig.h"

#include <QRegularExpression>
#include <QString>
#include <QStringList>

#include <array>
#include <optional>

/// @brief a version's parts, ordered so that comparing two arrays compares the
///        versions: {major, minor, patch, stage, stage number, revision}
using Version = std::array<int, 6>;

/// @brief the stage of a version without '_alpha', '_beta' or '_rc'. It is the
///        highest stage, so a final release beats its pre-releases
constexpr int FINAL_STAGE = 3;

/// @brief the revision of a '-dev' version. A tree between two releases comes
///        just before the release it names, and a '-r<n>' build comes just
///        after it
constexpr int DEV_REVISION = -1;

/// @brief reads 'v4.0.0', '4.0.0_beta2', 'v3.1', 'v3.1.1-r111' and
///        '4.0.0_rc0-dev' alike
/// @returns the version's parts, or nothing when the string isn't a version
inline std::optional<Version> parse_version_string(const QString& version)
{
  // the groups are: major, minor, patch, the pre-release word and its number,
  // a continuous build's revision, and the '-dev' of a build between two
  // releases. A '-dev' version gets DEV_REVISION as its revision. A version
  // without a patch number, such as 'v3.1', gets a patch of zero.
  //
  // The pattern is anchored, so a version inside other text doesn't match
  static const QRegularExpression ver_regex(QRegularExpression::anchoredPattern(
    R"(v?(\d+)\.(\d+)(?:\.(\d+))?(?:_(alpha|beta|rc)(\d+))?(?:-r(\d+))?(-dev)?)"));

  const QRegularExpressionMatch match = ver_regex.match(version);

  if (not match.hasMatch())
    return {};

  Version parts = {0, 0, 0, FINAL_STAGE, 0, 0};

  // group n fills part n - 1. Group 4 is missing here because the stage is a
  // word, and the loop below reads numbers
  for (const int group: {1, 2, 3, 5, 6})
  {
    const QString number = match.captured(group);
    if (number.isEmpty())
      continue;

    bool ok = false;
    parts[group - 1] = number.toUShort(&ok);
    if (not ok) [[unlikely]]
      return {};
  }

  static const QStringList stages = {"alpha", "beta", "rc"};

  if (const QString stage = match.captured(4); not stage.isEmpty())
    parts[3] = int(stages.indexOf(stage));

  if (match.hasCaptured(7))
    parts[5] = DEV_REVISION;

  return parts;
}

/// @brief this build's version, nothing when SOFTWARE_VERSION isn't a version
inline std::optional<Version> build_version()
{
  static const auto version = parse_version_string(SOFTWARE_VERSION);
  return version;
}
