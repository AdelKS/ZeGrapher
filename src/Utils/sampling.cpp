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

#include "Utils/sampling.h"
#include "DataPlot/datatablemodel.h"
#include "information.h"

#include <cmath>
#include <format>

namespace zg {
namespace mathobj {

Sampling::Sampling(QObject *parent)
  : QObject(parent), source(this), start(this), end(this), step(this)
{
  start.setImplicitName("sampling_start");
  end.setImplicitName("sampling_end");
  step.setImplicitName("sampling_step");

  start.setExpression("0");
  end.setExpression("10");
  step.setExpression("1");
}

void Sampling::setColumn(Data* c)
{
  column = c;
}

QString Sampling::apply()
{
  if (not column)
    return tr("No column to fill.");

  const zc::DynMathObject<zc_t>* zcObj = source.getZcObject();

  if (not source.isValid() or not zcObj
      or not (start.isValid() and end.isValid() and step.isValid()))
    return tr("Some of the fields are invalid.");

  const double from = start.getValue(), to = end.getValue(), by = step.getValue();

  if (std::isnan(from) or std::isnan(to) or std::isnan(by))
    return tr("The range has no value.");

  const double span = to - from;

  if (by == 0)
    return tr("The step cannot be zero.");
  else if (span / by < 0)
    return tr("The step must point towards the end.");
  else if (std::abs(by) > std::abs(span))
    return tr("The step is wider than the range.");
  else if (span / by > maxPoints)
    return tr("Too many points: the limit is %1.").arg(maxPoints);

  const size_t pointCount = size_t(span / by) + 1;

  std::vector<std::string> values;
  values.reserve(pointCount);

  for (size_t i = 0 ; i != pointCount ; i++)
  {
    const std::expected<double, zc::Error> res
      = (*zcObj)({from + double(i) * by}, &information->mathObjectCache);

    // ZeCalculator cannot read 'inf' or 'nan' back, such points stay empty
    values.push_back(res and std::isfinite(*res) ? std::format("{}", *res) : std::string());
  }

  dataTableModel.replaceColumnData(column, std::move(values));

  return {};
}

}
}
