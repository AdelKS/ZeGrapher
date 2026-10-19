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


#pragma once

#include <QPainter>

#include "graphlayout.h"
#include "Utils/sampledcurve.h"

/// @brief draws a graph with QPainter, for the exports
///
/// The screen draws the same layout and curves with scene graph nodes.
class GraphPainter
{
public:
  /// @param textAsPaths: draw text as glyph outlines, because an SVG names its
  ///        font without embedding it
  GraphPainter(QPainter& painter, bool textAsPaths);

  void fill(const QRectF& rect, const QColor& color);
  void draw(const LineBatch& batch);
  void draw(const zg::SampledCurve& curve);
  void drawLabels(const GraphLayout& layout);

protected:
  void drawPoint(QPointF center, const zg::CurveStyle& style);

  QPainter& painter;
  bool textAsPaths;
};
