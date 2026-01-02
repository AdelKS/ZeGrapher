#include <QRegularExpression>
#include <QString>

#include <optional>
#include <array>

/// @brief tries to parse the input string as vXX.YY.ZZ-rWWWWW (where X, Y, Z and W are digits and '-rWWWWW' is optional)
/// @returns if successful, returns an array of small integers [XX, YY, ZZ, WWWW] (WWWW = 0 if it wasn't there)
/// @note this helps in lexicographical comparison
std::optional<std::array<ushort, 4>> parse_version_string(QString version)
{
  static const QRegularExpression ver_regex = QRegularExpression(R"(v?(\d+)\.(\d+)\.(\d+)(-r\d+)?)");

  QRegularExpressionMatch match = ver_regex.match(version);

  if (not match.hasCaptured(0)) // check if the entire pattern didn't match, return early
    return {};

  std::array<ushort, 4> res = {0, 0, 0, 0};

  for (size_t i = 0; i != 4 ; i++)
  {
    if (not match.hasCaptured(i+1)) // i+1 because 0 captures the entire pattern
      return res;

    bool ok = false;
    QString substr = match.captured(i+1);
    if (i == 3)
      substr = substr.slice(2); // remove the "-r"
    ushort val = substr.toUShort(&ok);
    if (not ok) [[unlikely]]
      return {};
    else res[i] = val;
  }

  return res;
}
