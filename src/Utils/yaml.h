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

#include <QString>
#include <QFont>
#include <QSize>
#include <glaze/core/common.hpp>
#include <glaze/core/wrappers.hpp>
#include <glaze/yaml/opts.hpp>

#include <algorithm>
#include <cmath>

template <>
struct glz::meta<QString>
{
  using mimic = std::string;
  static constexpr auto write = [](QString& s, const std::string& input) { s = QString::fromStdString(input); };
  static constexpr auto read = [](QString& s) { return s.toStdString(); };
  static constexpr auto value = glz::custom<write, read>;
};

template <>
struct glz::meta<QFont::Weight>
{
   using enum QFont::Weight;
   static constexpr auto value = glz::enumerate(
    "thin", QFont::Thin,
    "extra_light", QFont::ExtraLight,
    "light", QFont::Light,
    "normal", QFont::Normal,
    "medium", QFont::Medium,
    "demi_bold", QFont::DemiBold,
    "bold", QFont::Bold,
    "extra_bold", QFont::ExtraBold,
    "black", QFont::Black);
};


namespace zg {
namespace yml {

template <typename T, typename U = std::conditional_t<std::is_same_v<T, QString>, std::string, T>>
std::optional<U> not_default(T s, T d = T())
{
   if constexpr (std::is_same_v<T, QString>)
      return s != d ? s.toStdString() : std::optional<U>();
   else return s != d ? s : std::optional<U>();
};

inline std::optional<double> not_nan(double v)
{
  return not std::isnan(v) ? v : std::optional<double>{};
}

/// @brief 'v' within [lo, hi], a NaN at 'lo': a document can hold any number
template <class T>
T clamped(T v, T lo, T hi)
{
  if constexpr (std::is_floating_point_v<T>)
    if (std::isnan(v))
      return lo;
  return std::clamp(v, lo, hi);
}

/// @brief read options that skip unknown keys, such as the ones a newer
///        ZeGrapher writes
inline constexpr glz::yaml::yaml_opts skip_unknown_keys{.error_on_unknown_keys = false};

struct QSizePOD {
  std::optional<int> width;
  std::optional<int> height;

  operator bool () const { return width or height; }

  /// @brief creates an instance from a size and a default (to tell which fields to leave empty)
  static std::optional<QSizePOD> from(QSize s, QSize def)
  {
    QSizePOD p {
      .width = not_default(s.width(), def.width()),
      .height = not_default(s.height(), def.height())
    };

    if (p)
      return p;
    else return {};
  }
};

struct QFontPOD {
  std::optional<std::string> name;
  std::optional<int> size;
  std::optional<QFont::Weight> weight;

  operator bool () const { return name or size or weight; }

  /// @brief sets the fields that the document holds on 'font'
  void applyTo(QFont& font) const
  {
    if (name)
      font.setFamily(QString::fromStdString(*name));
    if (size)
      font.setPointSize(*size);
    if (weight)
      font.setWeight(*weight);
  }

  /// @brief creates an instance from a font and a default (to tell which fields to leave empty)
  static std::optional<QFontPOD> from(QFont c, QFont def)
  {
    QFontPOD p {
      .name = not_default(c.family().toStdString(), def.family().toStdString()),
      .size = not_default(c.pointSize(), def.pointSize()),
      .weight = not_default(c.weight(), def.weight())
    };

    if (p)
      return p;
    else return {};
  }
};

}
}
