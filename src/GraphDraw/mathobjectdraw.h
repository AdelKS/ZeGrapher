/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
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

#ifndef GRAPHDRAW_H
#define GRAPHDRAW_H

#include <QWidget>

#include "Calculus/function.h"
#include "Calculus/funcvaluessaver.h"
#include "Calculus/regressionvaluessaver.h"
#include "Calculus/seqcalculator.h"
#include "GraphDraw/viewmapper.h"
#include "Utils/plotstyle.h"
#include "Widgets/pareqwidget.h"

class MathObjectDraw
{
public:
  explicit MathObjectDraw();

protected:
  void drawDataPoint(const QPointF& pt, const zg::PlotStyle& style);

  void drawOneSequence(const zc::Sequence<zc_t> &, const zg::PlotStyle&);
  void drawDataSet(int id, int width);
  void drawCurve(int width, QColor color, const QPolygonF& curve);
  void drawCurve(int width, QColor color, const QList<QPolygonF>& curves);

  void drawFunctions();
  void drawRegressions();
  void drawData();
  void drawSequences();
  void drawStaticParEq();

  void recalculateRegVals();

  QList<RegressionValuesSaver> regValuesSavers;
  QPainter* painter = nullptr;

  QPolygonF polygon;
  QPen pen;
  QBrush brush;
  Point centre;
  zg::ZeViewMapper viewMapper;
  FuncValuesSaver funcValuesSaver;

  double coef;

  Point pxPerUnit;
  bool moving, recalculate, recalculateRegs;

  QList<Function*> funcs;
  QList<SeqCalculator*> seqs;
  QList<ParEqWidget*>* parEqs;
  QList<QList<double>>* regVals;
};

#endif // GRAPHDRAW_H
