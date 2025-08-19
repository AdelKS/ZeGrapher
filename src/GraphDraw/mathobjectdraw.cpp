/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "GraphDraw/mathobjectdraw.h"
#include "GraphDraw/mathobjectdraw.impl.h"
#include "information.h"

using namespace std;

MathObjectDraw::MathObjectDraw()
  : sampler(viewMapper, information.getGraphSettings().distanceBetweenPoints)
{
  coef = sqrt(3) / 2;

  pen.setCapStyle(Qt::RoundCap);
  brush.setStyle(Qt::SolidPattern);

  moving = false;
}

void MathObjectDraw::drawDataPoint(const QPointF& pt, const zg::PlotStyle& style)
{
  double w = style.pointWidth;

  pen.setColor(style.color);
  pen.setWidth(w);
  brush.setColor(style.color);
  painter->setBrush(brush);
  painter->setPen(pen);

  painter->setRenderHint(QPainter::Antialiasing, true);

  switch (style.pointStyle)
  {
  case zg::PlotStyle::Cross:
  {
    painter->drawLine(pt + QPointF(0, 2 * w), pt + QPointF(0, -2 * w));
    painter->drawLine(pt + QPointF(-2 * w, 0), pt + QPointF(2 * w, 0));
    break;
  }
  case zg::PlotStyle::Disc:
    painter->drawEllipse(pt, w, w);
    break;
  case zg::PlotStyle::Rhombus:
  {
    QPolygonF polygon(
      {pt + QPointF(-w, 0), pt + QPointF(0, w), pt + QPointF(w, 0), pt + QPointF(0, -w)});

    painter->drawPolygon(polygon);
    break;
  }
  case zg::PlotStyle::Square:
  {
    QRectF rect;
    rect.setTopLeft(pt + QPointF(-w, -w));
    rect.setBottomRight(pt + QPointF(w, w));

    painter->drawRect(rect);
    break;
  }
  case zg::PlotStyle::Triangle:
  {
    w *= 2;
    QPolygonF polygon;
    double d = w * coef;
    double b = w / 2;

    polygon << pt + QPointF(0, -w) << pt + QPointF(d, b) << pt + QPointF(-d, b);

    painter->drawPolygon(polygon);
    break;
  }
  case zg::PlotStyle::None:
    break;
  }
}

void MathObjectDraw::drawObjects()
{
  for (const auto& [_, f_curve]: sampler.getContinuousCurves())
    drawSampledCurve(f_curve);

  for (const auto& [_, f_curve]: sampler.getDiscreteCurves())
    drawSampledCurve(f_curve);
}
