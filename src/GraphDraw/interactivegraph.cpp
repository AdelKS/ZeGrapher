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
#include "globalvars.h"

InteractiveGraph::InteractiveGraph(QQuickItem *parent) : Graph(parent)
{
  setKeepMouseGrab(true);
  orientation = QPageLayout::Landscape;
  moveType = NOTHING;
  sizeSettings.scalingFactor = 1;
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::LeftButton);

  connect(this, &QQuickItem::heightChanged, this, &InteractiveGraph::assignMouseRects);
  connect(this, &QQuickItem::widthChanged, this, &InteractiveGraph::assignMouseRects);
}

void InteractiveGraph::assignMouseRects()
{
  QPoint topLeftTranslation;
  topLeftTranslation.setX(-8);
  topLeftTranslation.setY(-8);

  QPoint bottomRightTranslation;
  bottomRightTranslation.setX(8);
  bottomRightTranslation.setY(8);

  topLeft.setTopLeft(figureRect.topLeft() + topLeftTranslation);
  topLeft.setBottomRight(figureRect.topLeft() + bottomRightTranslation);

  topRight.setTopLeft(figureRect.topRight() + topLeftTranslation);
  topRight.setBottomRight(figureRect.topRight() + bottomRightTranslation);

  bottomLeft.setTopLeft(figureRect.bottomLeft() + topLeftTranslation);
  bottomLeft.setBottomRight(figureRect.bottomLeft() + bottomRightTranslation);

  bottomRight.setTopLeft(figureRect.bottomRight() + topLeftTranslation);
  bottomRight.setBottomRight(figureRect.bottomRight() + bottomRightTranslation);

  top.setTopLeft(topLeft.topRight());
  top.setBottomRight(topRight.bottomLeft());

  bottom.setTopLeft(bottomLeft.topRight());
  bottom.setBottomRight(bottomRight.bottomLeft());

  left.setTopLeft(topLeft.bottomLeft());
  left.setBottomRight(bottomLeft.topRight());

  right.setTopLeft(topRight.bottomLeft());
  right.setBottomRight(bottomRight.topRight());
}

void InteractiveGraph::mousePressEvent(QMouseEvent *event)
{
  if (event->buttons() == Qt::LeftButton and boundingRect().contains(event->pos()))
    moveType = MOVE_VIEW;
  else if (not sizeSettings.figureFillsSheet)
  {
    if (topLeft.contains(event->pos()))
      moveType = RESIZE_GRAPH_TOPLEFT_CORNER;
    else if (topRight.contains(event->pos()))
      moveType = RESIZE_GRAPH_TOPRIGHT_CORNER;
    else if (top.contains(event->pos()))
      moveType = RESIZE_GRAPH_TOP_SIDE;
    else if (bottomLeft.contains(event->pos()))
      moveType = RESIZE_GRAPH_BOTTOMLEFT_CORNER;
    else if (bottomRight.contains(event->pos()))
      moveType = RESIZE_GRAPH_BOTTOMRIGHT_CORNER;
    else if (bottom.contains(event->pos()))
      moveType = RESIZE_GRAPH_BOTTOM_SIDE;
    else if (left.contains(event->pos()))
      moveType = RESIZE_GRAPH_LEFT_SIDE;
    else if (right.contains(event->pos()))
      moveType = RESIZE_GRAPH_RIGHT_SIDE;
    else if (figureRect.contains(event->pos()))
      moveType = MOVE_GRAPH;
    else
      moveType = NOTHING;
  }

  if (moveType != NOTHING)
  {
    lastMousePos = event->pos();
    event->accept();
  }
}

void InteractiveGraph::mouseMoveEvent(QMouseEvent *event)
{
  qDebug() << "Mouse move event";
  if (moveType == NOTHING and not sizeSettings.figureFillsSheet)
  {
    if (topLeft.contains(event->pos()) || bottomRight.contains(event->pos()))
      setCursor(Qt::SizeFDiagCursor);
    else if (topRight.contains(event->pos()) || bottomLeft.contains(event->pos()))
      setCursor(Qt::SizeBDiagCursor);
    else if (top.contains(event->pos()) || bottom.contains(event->pos()))
      setCursor(Qt::SizeVerCursor);
    else if (left.contains(event->pos()) || right.contains(event->pos()))
      setCursor(Qt::SizeHorCursor);
    else if (figureRect.contains(event->pos()))
      setCursor(Qt::SizeAllCursor);
    else
      setCursor(Qt::ArrowCursor);

    lastMousePos = event->pos();
  }
  else
  {
    qDebug() << "################################";
    qDebug() << rand();
    qDebug() << "Mouse move event";
    qDebug() << "figure Rect before change: " << figureRect;

    auto update_relfigrect = [this]{
      qDebug() << "figure Rect after change: " << figureRect;

      int marginVal = sizeSettings.pxMargins;
      QMargins margins(marginVal, marginVal, marginVal, marginVal);
      QRect marginlessSupport = supportRect.marginsRemoved(margins);

      QPointF topLeft = figureRect.topLeft() - marginlessSupport.topLeft();
      topLeft.setX(topLeft.x() / double(marginlessSupport.width()));
      topLeft.setY(topLeft.y() / double(marginlessSupport.height()));

      relFigRect.setWidth(double(figureRect.width()) / double(marginlessSupport.width()));
      relFigRect.setHeight(double(figureRect.height()) / double(marginlessSupport.height()));
      relFigRect.moveTopLeft(topLeft);

      qDebug() << "marginless support is " << marginlessSupport;
      qDebug() << "relative figure rect is " << relFigRect;

      constrainFigureRectRel();
      updateFigureSize();
      update();
    };

    QPoint pos = event->pos();

    qDebug() << "Graph size: " << size();
    qDebug() << "Mouse position: " << pos;

    QPoint dr = pos - lastMousePos;
    lastMousePos = pos;

    switch (moveType)
    {
    case MOVE_VIEW:
      // the view goes in the opposite direction
      viewMapper.translateView({
        .x = zg::pixel_unit{double(-dr.x() / totalScaleFactor)},
        .y = zg::pixel_unit{double(-dr.y() / totalScaleFactor)},
      });
      information.setGraphRangeMouseEdit(viewMapper.getRange<zg::real>());
      break;
    case RESIZE_GRAPH_TOPLEFT_CORNER:
      figureRect.setTopLeft(figureRect.topLeft() + dr);
      update_relfigrect();
      break;
    case RESIZE_GRAPH_TOPRIGHT_CORNER:
      figureRect.setTopRight(figureRect.topRight() + dr);
      update_relfigrect();
      break;
    case RESIZE_GRAPH_BOTTOMLEFT_CORNER:
      figureRect.setBottomLeft(figureRect.bottomLeft() + dr);
      update_relfigrect();
      break;
    case RESIZE_GRAPH_BOTTOMRIGHT_CORNER:
      figureRect.setBottomRight(figureRect.bottomRight() + dr);
      update_relfigrect();
      break;
    case RESIZE_GRAPH_LEFT_SIDE:
      figureRect.setLeft(figureRect.left() + dr.x());
      update_relfigrect();
      break;
    case RESIZE_GRAPH_TOP_SIDE:
      figureRect.setTop(figureRect.top() + dr.y());
      update_relfigrect();
      break;
    case RESIZE_GRAPH_RIGHT_SIDE:
      figureRect.setRight(figureRect.right() + dr.x());
      update_relfigrect();
      break;
    case RESIZE_GRAPH_BOTTOM_SIDE:
      figureRect.setBottom(figureRect.bottom() + dr.y());
      update_relfigrect();
      break;
    case MOVE_GRAPH:
      figureRect.translate(dr);
      update_relfigrect();
      break;
    case NOTHING:
      break;
    }
  }

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
  Q_UNUSED(event);
}
