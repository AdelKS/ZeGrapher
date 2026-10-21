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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#pragma once

#include <QColor>
#include <QPointF>
#include <QSGGeometryNode>

#include <vector>

#include "Utils/curvestyle.h"

/// @brief the markers of a discrete curve, in drawing units
struct MarkerGeometry
{
  std::vector<QPointF> centers;
  zg::CurveStyle style;
  float pxPerUnit = 1.f;
};

/// @brief draws markers as quads, where an SDF in markers.frag shapes each one
///
/// The geometry holds the centers, and the curve's style goes to the
/// material's uniform block, so one node draws tens of thousands of markers.
class MarkerNode : public QSGGeometryNode
{
public:
  MarkerNode();

  void set(const MarkerGeometry& markers);
};
