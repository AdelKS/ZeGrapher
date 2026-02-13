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

#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "graph.h"
#include <QPageLayout>
#include <QPdfWriter>
#include <QSvgGenerator>

enum MouseActionType
{
  NOTHING,
  MOVE_VIEW
};

class InteractiveGraph : public Graph
{
  Q_OBJECT
  QML_ELEMENT

public:
  explicit InteractiveGraph(QQuickItem *parent = nullptr);

public slots:
  virtual void mousePressEvent(QMouseEvent *event) override;
  virtual void mouseMoveEvent(QMouseEvent *event) override;
  virtual void mouseReleaseEvent(QMouseEvent *event) override;
  virtual void wheelEvent(QWheelEvent *event) override;

protected:

  QPoint lastMousePos;
  MouseActionType moveType;
};

#endif // MAINVIEW_H
