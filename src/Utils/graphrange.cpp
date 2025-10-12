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

real_range1d ZeAxisRange::getSnapshot()
{
  real_range1d newSnapshot = {.min = {min->getValue()}, .max = {max->getValue()}};
  bool old_state = state;

  state = newSnapshot.is_valid();
  if (state)
    latestValidSnapshot = newSnapshot;

  if (state != old_state)
    emit stateChanged();

  return newSnapshot;
}

real_range1d ZeAxisRange::getLatestValidSnapshot()
{
  real_range1d newSnapshot = {.min = {min->getValue()}, .max = {max->getValue()}};
  bool old_state = state;

  state = newSnapshot.is_valid();
  if (state)
    latestValidSnapshot = newSnapshot;

  if (state != old_state)
    emit stateChanged();

  return latestValidSnapshot;
}

void ZeAxisRange::update(const real_range1d& range)
{
  if (not min or not max) [[unlikely]]
    return;

  min->setExpression(QString::number(range.min.v, 'g', 12));
  max->setExpression(QString::number(range.max.v, 'g', 12));
}

GraphRange::GraphRange(QObject* parent)
  : QObject(parent), x(new ZeAxisRange(this)), y(new ZeAxisRange(this))
{
  x->min->setImplicitName("xmin");
  x->min->setExpression("-10");

  x->max->setImplicitName("xmax");
  x->max->setExpression("10");

  y->min->setImplicitName("ymin");
  y->min->setExpression("-10");

  y->max->setImplicitName("ymax");
  y->max->setExpression("10");
};

QRectF GraphRange::getLatestValidRect() const
{
  real_range1d xSnapshot = x->getLatestValidSnapshot();
  real_range1d ySnapshot = y->getLatestValidSnapshot();

  QRectF graphWin;
  graphWin.setBottom(ySnapshot.min.v);
  graphWin.setTop(ySnapshot.max.v);
  graphWin.setLeft(xSnapshot.min.v);
  graphWin.setRight(xSnapshot.max.v);
  return graphWin;
}

void GraphRange::update(const real_range2d& range)
{
  x->update(range.x);
  y->update(range.y);
}

real_range2d GraphRange::getLatestValidSnapshot() const
{
  return real_range2d{.x = x->getLatestValidSnapshot(), .y = y->getLatestValidSnapshot()};
}


} // namespace zg
