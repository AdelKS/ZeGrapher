#pragma once

/****************************************************************************
**  Copyright (c) 2019, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
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

#include <vector>
#include <unordered_set>

namespace zg {
namespace utils {

/// @brief move all values of 'vec' by 'distance' indices/slots
/// @note the slots on the left whose elements are moved from will have std::move() applied on them
/// @note if distance > vec.size(), the extra elements are default constructed
template <typename T>
void move_elements_right(std::vector<T>& vec, const int distance)
{
  vec.reserve(vec.size() + distance);
  const int orig_size = vec.size();

  int copy_index = int(vec.size()) - int(distance);
  for (int i = 0 ; i != distance ; i++)
  {
    if (copy_index >= 0)
      vec.push_back(std::move(vec[copy_index]));
    else vec.push_back(T{});
    copy_index++;
  }

  if (orig_size > distance)
  {
    for (int i = orig_size - 1 ; i >= distance ; i--)
      vec[i] = std::move(vec[i - distance]);
  }
}

/// @brief erase elements from a set according the predicate
template <typename T, typename Pred>
void erase(std::unordered_set<T>& s, Pred&& pred)
{
  for (auto it = s.begin(); it != s.end();)
  {
    if (pred(*it))
      it = s.erase(it);
    else
      ++it;
  }
}

}
}
