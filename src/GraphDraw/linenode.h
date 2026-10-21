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
#include <QLineF>
#include <QPolygonF>
#include <QRgba64>
#include <QSGGeometryNode>

#include <span>
#include <vector>

/// @brief a line vertex, with its position relative to the line for
///        lines.frag. All lengths are in drawing units.
struct LineVertex
{
  float x, y;
  /// @brief position along a capsule from its start point
  float along;
  /// @brief signed distance from the center line
  float across;
  /// @brief length of the capsule, negative in a strip
  float length;
  /// @brief distance travelled along the line, for the dashes
  float travelled;
};

/// @brief the lines.frag uniforms: lines with the same style share a material
struct LineStyle
{
  QRgba64 color = {};

  /// @brief half of the drawn width: a line thinner than a device pixel is
  ///        drawn one pixel wide, and fainter
  float halfWidth = 0.5f;
  float alphaScale = 1.f;
  float pxPerUnit = 1.f;

  /// @brief on then off, in drawing units
  std::vector<float> dashes;
  float dashPeriod = 0;

  auto operator<=>(const LineStyle&) const = default;
};

/// @brief antialiased lines of one color, width and dash pattern
///
/// A straight line is a capsule: a quad around the segment, where lines.frag
/// keeps the points closer than half the width. A polyline is a strip with
/// mitered joins and round ends, so its segments never cover each other.
class LineGeometry
{
public:
  /// @param width: in drawing units
  /// @param dashPattern: in widths, as in QPen, on then off
  /// @param pxPerUnit: device pixels per drawing unit
  LineGeometry(QColor color, double width, const QList<qreal>& dashPattern, float pxPerUnit);
  LineGeometry() = default;

  void addSegments(std::span<const QLineF> lines);
  void addPolyline(const QPolygonF& points);

  std::vector<LineVertex> vertices;
  /// @brief triangles, three indices into 'vertices' each
  std::vector<quint32> indices;
  LineStyle style;

protected:
  void addCapsule(QPointF a, QPointF b, float travelled);

  /// @brief two triangles covering the quad of the four vertices from 'first',
  ///        in strip order: left, right, left, right
  void addQuad(quint32 first);

  /// @brief half the side of the quads: lines.frag fades the edge out over
  ///        half a device pixel past the half width
  float extent = 0;
};

class LineNode : public QSGGeometryNode
{
public:
  LineNode();

  void set(const LineGeometry& lines);
};
