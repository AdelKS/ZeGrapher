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

#include <cmath>

#include "Utils/graphrange.h"
#include "globalvars.h"

namespace zg {

ZeAxisRange::ZeAxisRange(QObject *parent)
  : QObject(parent),
    min(mathWorld.addAltExprObject()),
    max(mathWorld.addAltExprObject())
{}

ZeAxisRange::~ZeAxisRange()
{
  mathWorld.removeAltExprObject(min);
  mathWorld.removeAltExprObject(max);
}

double ZeAxisRange::amplitude() const
{
  if (not min or not max) [[unlikely]]
    return std::nan("");

  return fabs(max->getValue() - min->getValue());
}

double ZeAxisRange::getMin() const
{
  if (not min) [[unlikely]]
    return std::nan("");

  return min->getValue();
}

double ZeAxisRange::getMax() const
{
  if (not max) [[unlikely]]
    return std::nan("");

  return max->getValue();
}

real_range1d ZeAxisRange::snapshot() const
{
  return real_range1d{.min = {getMin()}, .max = {getMax()}};
}

void ZeAxisRange::update(const real_range1d& range)
{
  if (not min or not max) [[unlikely]]
    return;

  min->setExpression(QString::number(range.min.v, 'g', 12));
  max->setExpression(QString::number(range.max.v, 'g', 12));
}

QRectF GraphRange::getRect() const
{
  QRectF graphWin;
  graphWin.setBottom(y->getMin());
  graphWin.setTop(y->getMax());
  graphWin.setLeft(x->getMin());
  graphWin.setRight(x->getMax());
  return graphWin;
}

void GraphRange::update(const real_range2d& range)
{
  x->update(range.x);
  y->update(range.y);
}

real_range2d GraphRange::snapshot() const
{
  return real_range2d{.x = x->snapshot(), .y = y->snapshot()};
}


} // namespace zg
