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

#include "Calculus/sampler.h"
#include "app.h"
#include "utils.h"

namespace {

/// @brief samples the app's objects in a 1000x600 pixel view, with the y axis
///        over [-1.5, 1.5]
/// @returns the size of each curve
std::vector<size_t> sample(const QString& xmin, const QString& xmax, size_t maxPoints)
{
  zg::GraphRange& range = information->graphSettings.range;
  range.x.min.setExpression(xmin);
  range.x.max.setExpression(xmax);
  range.y.min.setExpression("-1.5");
  range.y.max.setExpression("1.5");

  zg::ZeViewMapper mapper;
  mapper.setGraphRect(QRectF(0, 0, 1000, 600));
  mapper.setGraphRange(range.getLatestValidSnapshot());

  Sampler sampler(mapper);
  sampler.max_points = maxPoints;
  sampler.update();

  std::vector<size_t> sizes;
  for (const zg::SampledCurve& curve: sampler.getCurves())
  {
    // the inputs grow strictly, which the refinement and the drawing rely on
    test(std::ranges::adjacent_find(curve.input, std::greater_equal{}) == curve.input.end());
    sizes.push_back(curve.size());
  }
  return sizes;
}

}

int main()
{
  TestApp app;

  constexpr size_t maxPoints = 1 << 12;

  const auto equations = {"f(x) = x", "f(x) = sin(1/x)", "f(x) = sin(100*x)", "f(x) = tan(x)",
                          "u(n) = 0 ; sin(n)", "u(n) = 0 ; n"};

  // the last range is narrower than its values' precision
  const auto ranges = {std::pair{"-10", "10"}, {"0", "30000"}, {"-1e300", "1e300"},
                       {"1000", "1000.000000000005"}};

  for (const char* equation: equations)
    for (const auto& [xmin, xmax]: ranges)
    {
      app.restart();
      zg::mathWorld->addMathObject()->getEquation()->setEquation(equation);

      const std::vector<size_t> sizes = sample(xmin, xmax, maxPoints);
      test(sizes.size() == 1);
      test(sizes[0] >= 2);

      // the refinement stops once a pass ends above max_points, and each pass
      // at most doubles the points
      test(sizes[0] < 2 * maxPoints);
    }

  // a range of infinite width is invalid, and gives no curve
  app.restart();
  zg::mathWorld->addMathObject()->getEquation()->setEquation("f(x) = x");
  test(sample("-1e308", "1e308", maxPoints).empty());

  return 0;
}
