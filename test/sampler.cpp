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
/// @returns the inputs of each curve
std::vector<std::vector<zg::real_unit>> sample(const QString& xmin, const QString& xmax, size_t maxPoints)
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

  std::vector<std::vector<zg::real_unit>> inputs;
  for (const zg::SampledCurve& curve: sampler.getCurves())
  {
    // the inputs grow strictly, which the refinement and the drawing rely on
    test(std::ranges::adjacent_find(curve.input, std::greater_equal{}) == curve.input.end());
    inputs.push_back(curve.input);
  }
  return inputs;
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

      const auto inputs = sample(xmin, xmax, maxPoints);
      test(inputs.size() == 1);
      test(inputs[0].size() >= 2);

      // the refinement stops once a pass ends above max_points, and each pass
      // at most doubles the points
      test(inputs[0].size() < 2 * maxPoints);
    }

  // a range of infinite width is invalid, and gives no curve
  app.restart();
  zg::mathWorld->addMathObject()->getEquation()->setEquation("f(x) = x");
  test(sample("-1e308", "1e308", maxPoints).empty());

  // a sequence has no values past its end
  app.restart();
  auto* sequence = zg::mathWorld->addMathObject()->getEquation();
  sequence->setEquation("u(n) = 0 ; n");
  sequence->setEnd("3");
  const auto inputs = sample("-10", "10", maxPoints);
  test(inputs.size() == 1);
  test(inputs[0].back().v == 3);

  // a pan keeps the samples of Schrödinger curves, and an edit drops them
  {
    app.restart();
    auto* a = zg::mathWorld->addMathObject(zg::MathObject::CONSTANT)->getConstant();
    a->setName("a");
    a->set_value(1);
    auto* f = zg::mathWorld->addMathObject()->getEquation();
    f->setEquation("f(x) = a * sin(x)");
    a->setDeadAndAlive(true);

    zg::GraphRange& range = information->graphSettings.range;
    zg::ZeViewMapper mapper;
    mapper.setGraphRect(QRectF(0, 0, 1000, 600));

    auto pan = [&](const QString& xmin, const QString& xmax) {
      range.x.min.setExpression(xmin);
      range.x.max.setExpression(xmax);
      mapper.setGraphRange(range.getLatestValidSnapshot());
    };

    pan("-10", "10");
    Sampler sampler(mapper);
    sampler.update();
    test(sampler.getCurves().size() == 6);

    auto inputs = [&] {
      std::vector<std::vector<zg::real_unit>> all;
      for (const zg::SampledCurve& curve: sampler.getCurves())
        all.push_back(curve.input);
      return all;
    };

    // the share of the previous inputs, within the new range, that each curve kept
    auto kept = [&](const std::vector<std::vector<zg::real_unit>>& before) {
      size_t shared = 0, total = 0;
      for (const auto& [curve, old]: std::views::zip(sampler.getCurves(), before))
        for (zg::real_unit x: old)
          if (curve.get().input.front() <= x and x <= curve.get().input.back())
          {
            total++;
            shared += std::ranges::binary_search(curve.get().input, x);
          }
      return double(shared) / double(total);
    };

    auto before = inputs();
    pan("-9.7", "10.3");
    sampler.update();
    test(kept(before) > 0.9);

    // sampling from scratch over the new range lands on other inputs
    before = inputs();
    f->setEquation("f(x) = a * sin(x) + 0.1");
    sampler.update();
    test(kept(before) < 0.1);
  }

  return 0;
}
