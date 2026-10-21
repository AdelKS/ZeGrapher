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

#include "base.h"

namespace zg {

Base::Base(DefaultStringRangeProvider p, QObject* parent)
  : QObject(parent), start(this), end(this), defaultStringRangeProvider(p)
{
  start.setImplicitName("zg_internal_start");
  end.setImplicitName("zg_internal_end");
  resetStart();
  resetEnd();

  connect(&style, &PlotStyle::updated, this, &Base::updated);
}

StringRange Base::getDefaultStringRange() const
{
  return defaultStringRangeProvider(coordinateSystem, discrete);
}

bool Base::isDefaultStart() const
{
  return start.getExpression() == getDefaultStringRange().start;
}

bool Base::isDefaultEnd() const
{
  return end.getExpression() == getDefaultStringRange().end;
}

void Base::resetStart()
{
  start.setExpression(getDefaultStringRange().start);
}

void Base::resetEnd()
{
  end.setExpression(getDefaultStringRange().end);
}

void Base::setCoordinateSystem(CoordinateSystem s)
{
  if (s == coordinateSystem) return;

  const bool defStart = isDefaultStart();
  const bool defEnd = isDefaultEnd();

  coordinateSystem = s;

  if (defStart) resetStart();
  if (defEnd) resetEnd();

  emit coordinateSystemChanged();
  emit updated();
}

void Base::setDiscrete(bool b)
{
  if (discrete == b) return;

  const bool defStart = isDefaultStart();
  const bool defEnd = isDefaultEnd();

  discrete = b;
  style.setDiscrete(b);

  if (defStart) resetStart();
  if (defEnd) resetEnd();

  emit discreteChanged();
}

void Base::setSchrodinger(bool s)
{
  if (schrodinger == s) return;

  schrodinger = s;

  emit schrodingerChanged();
}

zg::real_range1d Base::getSnapshot() const
{
  return zg::real_range1d{.min = {start.evaluate()}, .max = {end.evaluate()}};
}

std::optional<zg::real_range1d> Base::getValidSnapshot() const
{
  const zg::real_range1d range = getSnapshot();

  if (not range.is_valid())
    return {};

  return range;
}

std::optional<SamplingSettings> Base::getSamplingSettings(Revision revision) const
{
  const auto range = getValidSnapshot();
  if (not range)
    return {};

  return SamplingSettings{.range = *range, .coordinateSystem = coordinateSystem, .revision = revision};
}

} // namespace zg
