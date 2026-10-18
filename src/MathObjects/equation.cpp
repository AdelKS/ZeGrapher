/****************************************************************************
**  Copyright (c) 2025, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "MathObjects/equation.h"
#include "Utils/yaml.h"
#include "information.h"

namespace zg {
namespace mathobj {

Equation::Equation(QObject *parent)
  : Base([](CoordinateSystem s, bool isDiscrete)
          { return s == CoordinateSystem::Polar
                   ? isDiscrete
                     ? StringRange{"0", "10"}
                     : StringRange{"0", "2*math::pi"}
                   : isDiscrete
                     ? StringRange{"0",    "xmax"}
                     : StringRange{"xmin", "xmax"}; }, parent), shared::ZcMathObjectBB()
{
  connect(&style, &PlotStyle::updated, this, &Equation::updated);
}

void Equation::setEquation(QString eq)
{
  if (eq == equation)
  {
    sync();
    return;
  }

  equation = eq;
  zcMathObj = equation.toStdString();

  sync();

  emit equationChanged();
  emit updated();
}

void Equation::setState(State s)
{
  if (state == s) return;

  state = s;
  emit stateChanged();
}

State Equation::sync()
{
  setState(State::from(zcMathObj.status()));

  // on a syntax error, the object keeps its kind and that kind's plot style
  if (const std::optional<bool> discrete = shared::parsedDiscrete(zcMathObj))
    Base::setDiscrete(*discrete);

  return getState();
};

Equation::POD Equation::exportPod() const {

  return POD {
    .equation = yml::not_default(getEquation()),
    .start = yml::not_default(getStartStr(), getDefaultStringRange().start),
    .end = yml::not_default(getEndStr(), getDefaultStringRange().end),
    .coordinates = yml::not_default(coordinateSystem, Cartesian),
    .style = style.exportPod()
  };
}

void Equation::importPod(Equation::POD p)
{
  if (p.equation)
    setEquation(QString::fromStdString(*p.equation));

  if (p.coordinates)
    setCoordinateSystem(*p.coordinates);

  if (p.start)
    setStart(QString::fromStdString(*p.start));

  if (p.end)
    setEnd(QString::fromStdString(*p.end));

  if (p.style)
    style.importPod(std::move(*p.style));
}

}
}
