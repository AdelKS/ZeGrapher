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

#pragma once

#include <QQuickItem>
#include <QImage>

#include "Calculus/sampler.h"
#include "graphlayout.h"
#include "linenode.h"
#include "markernode.h"
#include "viewmapper.h"
#include "Utils/graphsettings.h"

/// @brief the graph on screen, drawn with scene graph nodes, and its exports,
///        drawn with QPainter, both from the same GraphLayout
class Graph : public QQuickItem
{
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(ZeGraphSettings* settings READ getGraphSettings CONSTANT)

public:
  explicit Graph(QQuickItem* parent = nullptr);

  ZeGraphSettings* getGraphSettings() { return &settings; }

  /// @brief what updatePolish() prepares for updatePaintNode(), in drawing units
  struct Frame
  {
    /// @brief the inputs of the layout layers, which are only rebuilt when
    ///        these change
    struct Source
    {
      float scale = 1;
      float pxPerUnit = 1;
      QColor background;
      GraphLayout layout;

      bool operator==(const Source&) const = default;
    };

    Source source;
    std::vector<LineGeometry> belowCurves, curves, aboveCurves;
    LineGeometry border;
    std::vector<MarkerGeometry> markers;

    /// @brief the layers that updatePaintNode() must hand to the nodes
    bool layoutDirty = true, curvesDirty = true;
  };

  /// @brief writes the graph to a PDF, SVG or image file, picked from the
  ///        file suffix
  Q_INVOKABLE void exportTo(QUrl file);

protected slots:
  void minMaxPointsChanged();

protected:
  void updatePolish() override;
  QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) override;
  void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;
  void itemChange(ItemChange change, const ItemChangeData& value) override;

  void exportPDF(QUrl fileName);
  void exportSVG(QUrl fileName);
  void exportImage(QUrl fileName);

  enum class Export { Image, Pdf, Svg };

  /// @brief draws the graph with QPainter, the sheet filling p's window
  void paint(QPainter* p, Export target);

  /// @brief the sheet on screen, in drawing units
  /// @param available: the item's size, in drawing units
  QRectF sheetRect(QSizeF available) const;

  ZeGraphSettings& settings;
  zg::ZeViewMapper viewMapper;
  Sampler sampler;
  GraphLayoutEngine layoutEngine;

  /// @brief the on-screen view scale, which InteractiveGraph maps the mouse with
  double totalScaleFactor = 1;

  Frame frame;
};
