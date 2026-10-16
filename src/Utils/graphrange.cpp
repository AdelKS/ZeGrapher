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
#include "information.h"

#include <QLocale>

#include <cmath>

namespace zg {

ZeAxisRange::ZeAxisRange(QObject *parent)
  : QObject(parent), min(this), max(this)
{}

real_range1d ZeAxisRange::getLatestValidSnapshot()
{
  const real_range1d newSnapshot = {.min = {min.getValue()}, .max = {max.getValue()}};
  if (newSnapshot.is_valid())
    latestValidSnapshot = newSnapshot;

  return latestValidSnapshot;
}

void ZeAxisRange::update(const real_range1d& range, double pixels)
{
  const double power = std::round(std::log10(range.amplitude().v)) - std::round(std::log10(pixels));

  // powers of ten are exact up to 1e22, and dividing by one rounds the bound to
  // the double closest to its decimal, which then prints with few digits
  auto rounded = [power](double v) {
    if (not std::isfinite(power))
      return v;
    return power < 0 ? std::round(v * std::pow(10., -power)) / std::pow(10., -power)
                     : std::round(v / std::pow(10., power)) * std::pow(10., power);
  };

  auto text = [&](double v) {
    return QString::number(rounded(v), 'g', QLocale::FloatingPointShortest);
  };

  min.setExpression(text(range.min.v));
  max.setExpression(text(range.max.v));
}

GraphRange::GraphRange(QObject* parent)
  : QObject(parent), x(this), y(this)
{
  x.min.setImplicitName("xmin");
  x.min.setExpression("-10");

  x.max.setImplicitName("xmax");
  x.max.setExpression("10");

  y.min.setImplicitName("ymin");
  y.min.setExpression("-10");

  y.max.setImplicitName("ymax");
  y.max.setExpression("10");
};

void GraphRange::interactiveUpdate(const real_range2d& range, QSizeF pixels)
{
  x.update(range.x, pixels.width());
  y.update(range.y, pixels.height());
}

real_range2d GraphRange::getLatestValidSnapshot()
{
  return real_range2d{.x = x.getLatestValidSnapshot(), .y = y.getLatestValidSnapshot()};
}

std::optional<ZeAxisRange::POD> ZeAxisRange::exportPod() const
{
  return POD {
    .min = min.getExpression().toStdString(),
    .max = max.getExpression().toStdString()
  };
}

void ZeAxisRange::importPod(POD p)
{
  if (p.min)
    min.setExpression(QString::fromStdString(*p.min));

  if (p.max)
    max.setExpression(QString::fromStdString(*p.max));
}

std::optional<GraphRange::POD> GraphRange::exportPod() const
{
  return POD {
    .x = x.exportPod(),
    .y = y.exportPod()
  };
}

void GraphRange::importPod(POD p)
{
  if (p.x)
    x.importPod(std::move(*p.x));

  if (p.y)
    y.importPod(std::move(*p.y));
}


} // namespace zg
