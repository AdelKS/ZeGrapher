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

  parEqs = information.getParEqsList();
  funcs = information.getFuncsList();
  seqs = information.getSeqsList();

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

void MathObjectDraw::drawDataSet(int id, int width)
{
  auto userData = information.getDataPoints(id);

  pen.setColor(userData->style.color);
  painter->setPen(pen);

  static std::vector<std::function<void(MathObjectDraw*, QPointF, double)>> draw_functions;
    // = {&MathObjectDraw::drawRhombus,
    //    &MathObjectDraw::drawDisc,
    //    &MathObjectDraw::drawSquare,
    //    &MathObjectDraw::drawTriangle,
    //    &MathObjectDraw::drawCross};

  const std::vector<Point>& dataPoints = userData->dataPoints;
  QPolygonF polygon;
  QPointF prev_qpt;
  bool first_pt = true;

  const double target_sq_dist = information.getGraphSettings().distanceBetweenPoints
                                * information.getGraphSettings().distanceBetweenPoints;

  brush.setColor(userData->style.color);
  painter->setBrush(brush);

  for (const Point& pt: dataPoints)
  {
    QPointF qpt = userData->cartesian
                    ? pt * pxPerUnit
                    : Point{.x = pt.x * cos(pt.y), .y = pt.x * sin(pt.y)} * pxPerUnit;

    double&& sq_dist = QPointF::dotProduct(qpt - prev_qpt, qpt - prev_qpt);

    if (!first_pt && sq_dist <= target_sq_dist)
      continue;

    if (userData->style.drawLines)
      polygon.push_back(qpt);

    if (userData->style.drawPoints)
      draw_functions[uint(userData->style.pointStyle)](this, qpt, width);

    prev_qpt = std::move(qpt);
    first_pt = false;
  }

  if (userData->style.drawLines)
  {
    pen.setStyle(userData->style.lineStyle);
    painter->setPen(pen);
    painter->drawPolyline(polygon);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
  }
}

void MathObjectDraw::drawData()
{
  for (int i = 0; i < information.getDataListsCount(); i++)
  {
    if (information.getDataPoints(i)->style.draw)
      drawDataSet(i, information.getGraphSettings().curvesThickness + 2);
  }
}

void MathObjectDraw::drawCurve(int width, QColor color, const QPolygonF& curve)
{
  pen.setWidth(width);
  pen.setColor(color);
  painter->setPen(pen);

  painter->drawPolyline(curve);
}

void MathObjectDraw::drawCurve(int width, QColor color, const QList<QPolygonF>& curves)
{
  for (const QPolygonF& curve: curves) drawCurve(width, color, curve);
}

void MathObjectDraw::drawRegressions()
{
  painter->setRenderHint(QPainter::Antialiasing,
                         information.getGraphSettings().smoothing && !moving);

  for (int reg = 0; reg < regValuesSavers.size(); reg++)
  {
    if (information.getRegression(reg)->getDrawState())
    {
      for (int curve = 0; curve < regValuesSavers[reg].getCurves().size(); curve++)
      {
        drawCurve(information.getGraphSettings().curvesThickness,
                  information.getRegression(reg)->getColor(),
                  regValuesSavers[reg].getCurves().at(curve));
      }
    }
  }
}

void MathObjectDraw::recalculateRegVals()
{
  for (auto& regValSaver: regValuesSavers)
  {
    regValSaver.recalculate(pxPerUnit, viewMapper);
  }
}

void MathObjectDraw::drawFunctions()
{
  for (const auto& [_, f_curve]: sampler.getContinuousCurves())
    drawSampledCurve(f_curve);
}

void MathObjectDraw::drawSequences()
{
  for (const auto& [_, f_curve]: sampler.getDiscreteCurves())
    drawSampledCurve(f_curve);
}

void MathObjectDraw::drawStaticParEq()
{
  QList<QList<Point>>* list;
  QPolygonF polygon;
  Point point;
  ColorSaver* colorSaver;

  pen.setWidth(information.getGraphSettings().curvesThickness);
  painter->setRenderHint(QPainter::Antialiasing,
                         information.getGraphSettings().smoothing && !moving);
  painter->setPen(pen);

  for (int i = 0; i < parEqs->size(); i++)
  {
    if (!parEqs->at(i)->getDrawState() || parEqs->at(i)->isAnimated())
      continue;

    list = parEqs->at(i)->getPointsList();
    colorSaver = parEqs->at(i)->getColorSaver();

    for (int curve = 0; curve < list->size(); curve++)
    {
      pen.setColor(colorSaver->getColor(curve));
      painter->setPen(pen);

      polygon.clear();

      for (int pos = 0; pos < list->at(curve).size(); pos++)
      {
        point = list->at(curve).at(pos);
        polygon << QPointF(viewMapper.to<zg::plane::pixel>(zg::real_pt::from(point)));
      }

      painter->drawPolyline(polygon);
    }
  }
}
