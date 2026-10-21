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


#include "GraphDraw/graphpainter.h"

#include <QPainterPath>

#include <cmath>

GraphPainter::GraphPainter(QPainter& painter, bool textAsPaths)
  : painter(painter), textAsPaths(textAsPaths)
{
  painter.setRenderHint(QPainter::Antialiasing);
}

void GraphPainter::fill(const QRectF& rect, const QColor& color)
{
  painter.setPen(Qt::NoPen);
  painter.setBrush(color);
  painter.drawRect(rect);
}

void GraphPainter::draw(const LineBatch& batch)
{
  painter.setPen(QPen(batch.color, batch.width, Qt::SolidLine, batch.cap, Qt::RoundJoin));
  painter.drawLines(batch.lines.data(), int(batch.lines.size()));
}

void GraphPainter::draw(const zg::SampledCurve& curve)
{
  const zg::CurveStyle& style = curve.style;
  if (not style.visible)
    return;

  if (style.drawLine)
  {
    QPen pen(style.color, style.lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    if (not style.dashPattern.isEmpty())
      pen.setDashPattern(style.dashPattern);

    painter.setPen(pen);
    for (const QPolygonF& polyline : curve.polylines())
      painter.drawPolyline(polyline);
  }

  if (not curve.discrete or style.pointStyle == zg::PlotStyle::None)
    return;

  painter.setPen(QPen(style.color, style.lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter.setBrush(style.color);

  for (const QPointF& center : curve.px_curve)
    if (not std::isnan(center.x()) and not std::isnan(center.y()))
      drawPoint(center, style);
}

void GraphPainter::drawPoint(QPointF pt, const zg::CurveStyle& style)
{
  const double w = style.pointWidth;

  switch (style.pointStyle)
  {
  case zg::PlotStyle::Cross:
    painter.drawLine(pt + QPointF(0, w), pt + QPointF(0, -w));
    painter.drawLine(pt + QPointF(-w, 0), pt + QPointF(w, 0));
    break;
  case zg::PlotStyle::Disc:
    painter.drawEllipse(pt, w, w);
    break;
  case zg::PlotStyle::Rhombus:
    painter.drawPolygon(QPolygonF(
      {pt + QPointF(-w, 0), pt + QPointF(0, w), pt + QPointF(w, 0), pt + QPointF(0, -w)}));
    break;
  case zg::PlotStyle::Square:
    painter.drawRect(QRectF(pt + QPointF(-w, -w), pt + QPointF(w, w)));
    break;
  case zg::PlotStyle::Triangle:
  {
    const double d = w * std::sqrt(3.) / 2.;
    painter.drawPolygon(QPolygonF({pt + QPointF(0, -w), pt + QPointF(d, w / 2.), pt + QPointF(-d, w / 2.)}));
    break;
  }
  case zg::PlotStyle::None:
    break;
  }
}

void GraphPainter::drawLabels(const GraphLayout& layout)
{
  painter.setPen(layout.textColor);

  for (const Label& label : layout.labels)
  {
    // QPainter converts points to pixels with its device's logicalDpiY, and the
    // layout sizes its fonts for 72 dpi
    QFont font = layout.font(label.role);
    font.setPointSizeF(font.pointSizeF() * 72. / painter.device()->logicalDpiY());
    painter.setFont(font);

    painter.save();
    painter.translate(label.origin);
    painter.rotate(label.angle);
    if (textAsPaths)
    {
      QPainterPath path;
      path.addText(QPointF(), painter.font(), label.text);
      painter.fillPath(path, layout.textColor);
    }
    else painter.drawText(QPointF(), label.text);
    painter.restore();
  }
}
