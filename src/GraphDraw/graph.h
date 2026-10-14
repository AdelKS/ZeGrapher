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

#include <QQuickPaintedItem>
#include <QImage>
#include <QVariantList>

#include "graphlayout.h"
#include "mathobjectdraw.h"
#include "Utils/graphsettings.h"

class Graph : public QQuickPaintedItem, public MathObjectDraw
{
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(ZeGraphSettings* settings READ getGraphSettings NOTIFY settingsChanged)

  Q_PROPERTY(QVariantList qmlData READ getQmlData NOTIFY qmlDataChanged)
  Q_PROPERTY(QRectF graphRect READ getGraphRect NOTIFY graphRectChanged)

public:
  explicit Graph(QQuickItem* parent = nullptr);

  Q_INVOKABLE ZeGraphSettings* getGraphSettings() { return &settings; }

  QVariantList getQmlData() const { return qmlData; }
  QRectF getGraphRect() const { return graphRect; }

  /// Live view of the sampler's curves, used by DataPoints for direct
  /// scenegraph rendering without a QVariantList round-trip.
  const auto& curves() const { return sampler.getCurves(); }

public slots:
  void exportPDF(QUrl fileName);
  void exportSVG(QUrl fileName);
  void exportImage(QUrl filename);

  virtual void paint(QPainter *p) override;

signals:
  void settingsChanged();
  void qmlDataChanged();
  void graphRectChanged();

protected slots:
  void minMaxPointsChanged();

protected:
  void scaleView();
  void setPainterFont(QFont font);
  void drawSupport();
  void computeSupportRect();
  void drawAll();
  void drawLines(const LineBatch& batch);
  void drawLabels();

  void updateQmlData();

  ZeGraphSettings& settings;
  GraphLayoutEngine layoutEngine;
  GraphLayout layout;

  double totalScaleFactor;

  QRectF supportRect;

  QSizeF scaledSize;

  enum PaintType {GPU_RENDER, VECTOR_EXPORT, IMAGE_EXPORT};

  /// @brief are we painting into SVG or images ?
  PaintType paintType = GPU_RENDER;

  /// @note we draw continuous curves from QML to avoid a bottleneck
  ///       in using drawPolyline() that's very slow in QQuickPaintedItem
  QVariantList qmlData;

  /// @note we need to tell QML exactly where to draw the curves
  QRectF graphRect;
};
