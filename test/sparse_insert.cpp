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

#include "Utils/sampledcurve.h"
#include "utils.h"

int main()
{
  zg::SampledCurve curve {
    .input = {{1.}, {1.}, {1.}},
    .curve = {zg::real_pt{{1.}, {1.}}, {{1.}, {1.}}, {{1.}, {1.}}},
    .px_curve = {{1., 1.}, {1., 1.}, {1., 1.}},
  };

  curve.sparse_insert(
    {0, 1, 2, 3},
    {{2.}, {3.}, {4.}, {5.}},
    {zg::real_pt{{2.}, {2.}}, {{3.}, {3.}}, {{4.}, {4.}}, {{5.}, {5.}}},
    {{2., 2.}, {3., 3.}, {4., 4.}, {5., 5.}}
  );

  test(bool(curve.input == std::vector<zg::real_unit>{{2.}, {1.}, {3.}, {1.}, {4.}, {1.}, {5.}}));

  return 0;
}
