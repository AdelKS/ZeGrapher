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

#include "app.h"
#include "utils.h"
#include "GraphDraw/viewmapper.h"

#include <cmath>

int main()
{
  TestApp app;

  zg::GraphRange& range = information->graphSettings.range;

  auto write = [&](double min, double max) {
    range.interactiveUpdate({.x = {.min = {min}, .max = {max}}, .y = {.min = {-1.}, .max = {1.}}},
                            QSizeF(1000, 600));
    return range.getLatestValidSnapshot().x;
  };

  // a bound keeps about a pixel's worth of digits, and no more
  write(-10.0123456, 9.98765);
  test(range.x.min.getExpression() == "-10.01");
  test(range.x.max.getExpression() == "9.99");

  // a deep zoom far from zero keeps its width
  const zg::real_range1d deep = write(1000., 1000. + 1e-9);
  test(deep.min.v == 1000.);
  test(std::abs(deep.amplitude().v - 1e-9) < 1e-11);

  // the view mapper gets a range before its rect
  zg::ZeViewMapper mapper;
  mapper.setGraphRange({.x = {.min = {-10.}, .max = {10.}}, .y = {.min = {-5.}, .max = {5.}}});
  test(mapper.getRange<zg::real>().x == zg::real_range1d{.min = {-10.}, .max = {10.}});

  return 0;
}
