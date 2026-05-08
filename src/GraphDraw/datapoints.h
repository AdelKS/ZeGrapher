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

#include <QPointer>
#include <QQuickItem>
#include <QtQmlIntegration/qqmlintegration.h>

#include "Utils/sampledcurve.h"

Q_DECLARE_METATYPE(const zg::SampledCurve*);

/// QQuickItem that renders discrete-curve markers as GPU point sprites with
/// an SDF in the fragment shader. Built to scale to tens of thousands of
/// markers per curve, where the previous QPolygonF-per-marker pipeline
/// became the dominant cost.
///
/// One QSGGeometryNode is emitted per discrete curve; the geometry holds
/// only the marker centers, while per-curve style (color, size, shape)
/// lives in the material's std140 uniform block. Positions are read
/// straight from the parent Graph's Sampler — no QVariantList round-trip.
class DataPoints : public QQuickItem
{
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(const zg::SampledCurve* curve MEMBER curve)
  Q_PROPERTY(float totalScaleFactor MEMBER totalScaleFactor)

public:
  explicit DataPoints(QQuickItem *parent = nullptr);

signals:
  void graphChanged();

protected:
  QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
  const zg::SampledCurve* curve = nullptr;
  float totalScaleFactor = 1.0f;
};
