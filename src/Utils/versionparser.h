#pragma once

#include <QRegularExpression>
#include <QString>
#include <QStringList>

#include <array>
#include <optional>
#include <utility>

/// @brief the parts of a version, in the order that compares them:
///        {major, minor, patch, stage, stage number, revision}
using Version = std::array<ushort, 6>;

/// @brief the stage of a version that no '_alpha', '_beta' or '_rc' marks
constexpr ushort FINAL_STAGE = 3;

/// @brief reads 'v4.0.0', '4.0.0_beta2', 'v3.1' and 'v3.1.1-r111' the same way
/// @returns the parts of the version, which lexicographical_compare orders
inline std::optional<Version> parse_version_string(const QString& version)
{
  // the groups: major, minor, patch, the word of a pre-release and its number,
  // then the revision that a continuous build carries. A version that leaves
  // its patch number out, such as 'v3.1', reads it as zero
  static const QRegularExpression ver_regex(
    R"(v?(\d+)\.(\d+)(?:\.(\d+))?(?:_(alpha|beta|rc)(\d+))?(?:-r(\d+))?)");

  const QRegularExpressionMatch match = ver_regex.match(version);

  if (not match.hasMatch())
    return {};

  Version parts = {0, 0, 0, FINAL_STAGE, 0, 0};

  // {the part it fills, the group it reads}: the stage is a word, not a number
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
