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

#include "Utils/algos.h"

#include "utils.h"

int main()
{
  std::vector<int> vals = {0, 1, 2, 3, 4};

  zg::utils::move_element(vals, 1, 3);
  test(bool(vals == std::vector<int>({0, 2, 3, 1, 4})));

  zg::utils::move_element(vals, 4, 0);
  test(bool(vals == std::vector<int>({4, 0, 2, 3, 1})));

  zg::utils::move_element(vals, 2, 2);
  test(bool(vals == std::vector<int>({4, 0, 2, 3, 1})));

  return 0;
}
