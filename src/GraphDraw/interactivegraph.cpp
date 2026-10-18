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

#include "GraphDraw/interactivegraph.h"

#include <cmath>
#include <utility>

InteractiveGraph::InteractiveGraph(QQuickItem *parent) : Graph(parent)
{
  setKeepMouseGrab(true);
  setAcceptedMouseButtons(Qt::LeftButton);
}


void InteractiveGraph::mousePressEvent(QMouseEvent *event)
{
  if (event->buttons() == Qt::LeftButton and boundingRect().contains(event->position()))
  {
    dragging = true;
    lastMousePos = event->position();
    event->accept();
  }
  else
    dragging = false;
}

void InteractiveGraph::mouseMoveEvent(QMouseEvent *event)
{
  const QPointF dr = event->position() - lastMousePos;

  if (dragging)
  {
    // the view goes in the opposite direction
    viewMapper.translateView({
      .x = zg::pixel_unit{-dr.x() / totalScaleFactor},
      .y = zg::pixel_unit{-dr.y() / totalScaleFactor},
    });
    settings.range.interactiveUpdate(viewMapper.getRange<zg::real>(), frame.source.layout.graph.size());
  }

  lastMousePos = event->position();
  event->accept();
}

void InteractiveGraph::mouseReleaseEvent(QMouseEvent *event)
{
  Q_UNUSED(event);
  dragging = false;
}

void InteractiveGraph::wheelEvent(QWheelEvent *event)
{
  if (event->angleDelta().isNull())
    return;

  const double inverted = event->inverted() ? -1.0 : 1.0;

  int x_delta = event->angleDelta().x();
  int y_delta = event->angleDelta().y();

  // if SHIFT is pressed, swap x angleDelta with y
  // this is so users without horizontal scroll can still do it
  const bool xy_swap = event->modifiers() & Qt::ShiftModifier;

  if (xy_swap) std::swap(x_delta, y_delta);

  const double y_factor = std::pow(1. + 1./32., inverted * double(y_delta) / 120.);
  const double x_factor = std::pow(1. + 1./32., inverted * double(x_delta) / 120.);

  // if CTRL is pressed, handle x and y separately
  const bool xy_separate = event->modifiers() & Qt::ControlModifier;

  // the view mapper counts pixels from the graph's top left corner, inside the
  // label margins
  const zg::pixel_pt center = zg::pixel_pt::from(event->position() / totalScaleFactor
                                                 - frame.source.layout.graph.topLeft());

  if (xy_separate)
  {
    if (x_delta != 0)
      viewMapper.x.zoomView(center.x, x_factor);

    if (y_delta != 0)
      viewMapper.y.zoomView(center.y, y_factor);
  }
  else
    viewMapper.zoomView(center, y_delta != 0 ? y_factor : x_factor);

  settings.range.interactiveUpdate(viewMapper.getRange<zg::real>(), frame.source.layout.graph.size());
  event->accept();
}
