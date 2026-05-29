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
#include <glaze/yaml.hpp>

template <>
struct glz::meta<QString>
{
  using mimic = std::string;
  static constexpr auto write = [](QString& s, const std::string& input) { s = QString::fromStdString(input); };
  static constexpr auto read = [](QString& s) { return s.toStdString(); };
  static constexpr auto value = glz::custom<write, read>;
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

}
}
