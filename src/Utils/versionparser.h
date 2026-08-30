#pragma once

#include <QRegularExpression>
#include <QString>
#include <QStringList>

#include <array>
#include <optional>
#include <utility>

/// @brief the parts of a version, ordered so that comparing two arrays
///        compares the versions: {major, minor, patch, stage, stage number,
///        revision}
using Version = std::array<ushort, 6>;

/// @brief the stage of a version that carries no '_alpha', '_beta' or '_rc'.
///        It is the highest stage, so a final release beats its pre-releases
constexpr ushort FINAL_STAGE = 3;

/// @brief reads 'v4.0.0', '4.0.0_beta2', 'v3.1', 'v3.1.1-r111' and
///        '4.0.0_rc0-dev' alike
/// @returns the parts of the version, or nothing when the string is no version
inline std::optional<Version> parse_version_string(const QString& version)
{
  // the groups are: major, minor, patch, the word of a pre-release and its
  // number, then the revision that a continuous build carries. A version
  // without a patch number, such as 'v3.1', gets a patch of zero. A '-dev'
  // suffix marks a build between two releases, and it reads as the release it
  // comes before.
  //
  // The pattern is anchored, so a string that only holds a version somewhere
  // in it reads as no version at all
  static const QRegularExpression ver_regex(QRegularExpression::anchoredPattern(
    R"(v?(\d+)\.(\d+)(?:\.(\d+))?(?:_(alpha|beta|rc)(\d+))?(?:-r(\d+))?(?:-dev)?)"));

  const QRegularExpressionMatch match = ver_regex.match(version);

  if (not match.hasMatch())
    return {};

  Version parts = {0, 0, 0, FINAL_STAGE, 0, 0};

  // {part to fill, group to read from}. The stage is missing here because it is
  // a word, and the loop below reads numbers
  constexpr std::array<std::pair<size_t, int>, 5> numbers =
    {{{0, 1}, {1, 2}, {2, 3}, {4, 5}, {5, 6}}};

  for (const auto& [part, group]: numbers)
  {
    const QString number = match.captured(group);
    if (number.isEmpty())
      continue;

    bool ok = false;
    parts[part] = number.toUShort(&ok);
    if (not ok) [[unlikely]]
      return {};
  }

  static const QStringList stages = {"alpha", "beta", "rc"};

  if (const QString stage = match.captured(4); not stage.isEmpty())
    parts[3] = ushort(stages.indexOf(stage));

  return parts;
}
