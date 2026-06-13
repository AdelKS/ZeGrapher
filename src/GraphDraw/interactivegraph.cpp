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
#include "information.h"

InteractiveGraph::InteractiveGraph(QQuickItem *parent) : Graph(parent)
{
  setKeepMouseGrab(true);
  orientation = QPageLayout::Landscape;
  moveType = NOTHING;
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::LeftButton);
}


void InteractiveGraph::mousePressEvent(QMouseEvent *event)
{
  if (event->buttons() == Qt::LeftButton and boundingRect().contains(event->pos()))
  {
    moveType = MOVE_VIEW;
    lastMousePos = event->pos();
    event->accept();
  }
  else
    moveType = NOTHING;
}

void InteractiveGraph::mouseMoveEvent(QMouseEvent *event)
{
  qDebug() << "################################";
  qDebug() << "Mouse move event " << event->pos();

  QPoint dr = event->pos() - lastMousePos;

  switch (moveType)
  {
  case MOVE_VIEW:
    // the view goes in the opposite direction
    viewMapper.translateView({
      .x = zg::pixel_unit{double(-dr.x() / totalScaleFactor)},
      .y = zg::pixel_unit{double(-dr.y() / totalScaleFactor)},
    });
    settings.range.interactiveUpdate(viewMapper.getRange<zg::real>());
    break;
  case NOTHING:
    break;
  }

  lastMousePos = event->pos();
  event->accept();
}

void InteractiveGraph::mouseReleaseEvent(QMouseEvent *event)
{
  Q_UNUSED(event);
  setCursor(Qt::ArrowCursor);
  moveType = NOTHING;
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

  if (xy_separate)
  {
    if (x_delta != 0)
      viewMapper.x.zoomView(zg::pixel_unit{event->position().x() / totalScaleFactor}, x_factor);

    if (y_delta != 0)
      viewMapper.y.zoomView(zg::pixel_unit{event->position().y() / totalScaleFactor}, y_factor);
  }
  else
    viewMapper.zoomView(zg::pixel_pt::from(event->position() / totalScaleFactor),
                        y_delta != 0 ? y_factor : x_factor);

  settings.range.interactiveUpdate(viewMapper.getRange<zg::real>());
  event->accept();
}
