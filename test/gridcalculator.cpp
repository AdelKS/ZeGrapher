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

#include "GraphDraw/graphlayout.h"
#include "app.h"
#include "utils.h"

int main()
{
  TestApp app;
  ZeGraphSettings& settings = information->graphSettings;
  GridCalculator calculator(settings);
  TextMetrics metrics(settings.getFont(), GraphLayoutEngine::referenceDevice());

  // the x axis ticks of a 1000 pixel view over [min, max]
  auto ticksOver = [&](double min, double max)
  {
    zg::ZeViewMapper mapper;
    mapper.setGraphRect(QRectF(0, 0, 1000, 600));
    mapper.setGraphRange({.x = {.min = {min}, .max = {max}}, .y = {.min = {-1}, .max = {1}}});
    return calculator.getLinearAxisTicks(mapper.x, metrics);
  };

  const std::pair<double, double> ranges[] = {
    {-10, 10}, {0, 1}, {1e-300, 2e-300}, {-1e300, 1e300},
    {1e6, 1e6 + 1e-6}, {1e6, 1e6 + 1e-9}, {1000, 1000 + 5e-12},
    {-1e9 - 1, -1e9}, {123456789.001, 123456789.002},
  };

  for (const char* multiplier: {"", "math::pi"})
  {
    settings.multiplier<ZeAxisName::X>().setExpression(multiplier);

    for (const auto& [min, max]: ranges)
    {
      const ZeLinAxisTicks ticks = ticksOver(min, max);
      test(2 <= ticks.ticks.size() and ticks.ticks.size() <= 100);
      // not strictly sorted: a range a few ulps wide has few positions to give
      test(std::ranges::is_sorted(ticks.ticks, {}, [](const ZeLinAxisTick& t) { return t.pos; }));

      const auto inside = std::ranges::count_if(ticks.ticks, [&](const ZeLinAxisTick& t) {
        return min <= t.pos.v and t.pos.v <= max;
      });
      test(inside >= 2);
    }
  }

  // the offset is a multiple of the multiplier, like the ticks
  test(ticksOver(1e6, 1e6 + 1e-6).offset.sumOffsetStr().endsWith("math::pi"));

  // a multiplier that doesn't leave a valid range gives plain numbers
  for (const char* multiplier: {"0", "-1", "-math::pi", "1/0", "1e-310"})
  {
    settings.multiplier<ZeAxisName::X>().setExpression(multiplier);
    const ZeLinAxisTicks ticks = ticksOver(-10, 10);
    test(ticks.offset.multiplierStr.isEmpty());
    test(2 <= ticks.ticks.size() and ticks.ticks.size() <= 100);
  }

  settings.multiplier<ZeAxisName::X>().setExpression("");
  test(ticksOver(-1e9 - 1, -1e9).offset.sumOffsetStr() == " - 1000000000");

  // a range of infinite width keeps the view on the last valid range
  zg::ZeAxisRange& x = settings.range.x;
  x.min.setExpression("-1e308");
  x.max.setExpression("1e308");
  test(x.getLatestValidSnapshot() == zg::real_range1d{.min = {-10}, .max = {10}});

  return 0;
}
