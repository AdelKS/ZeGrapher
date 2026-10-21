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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#pragma once

#include "graph.h"

class InteractiveGraph : public Graph
{
  Q_OBJECT
  QML_ELEMENT

public:
  explicit InteractiveGraph(QQuickItem *parent = nullptr);

  /// @brief zooms in by 'factor' around 'position', in item coordinates. A
  ///        factor below 1 zooms out
  Q_INVOKABLE void zoomAt(QPointF position, double factor);

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseUngrabEvent() override;
  void wheelEvent(QWheelEvent *event) override;

  /// @brief 'position', in item coordinates, as the view mapper counts pixels
  zg::pixel_pt toGraphPixels(QPointF position) const;

  /// @brief writes the view mapper's range to the graph settings
  void viewChanged();

  QPointF lastMousePos;
  bool dragging = false;
};
