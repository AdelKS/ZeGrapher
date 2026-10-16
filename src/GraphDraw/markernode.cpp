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

#include "GraphDraw/markernode.h"
#include "GraphDraw/styledmaterial.h"

#include <QRgba64>
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGNode>
#include <algorithm>
#include <cmath>
#include <ranges>

namespace {

/// @brief the margin past the shape where markers.frag fades the edge out, in
///        device pixels
constexpr float aaPad = 1.5f;

// the uniform block of markers.vert and markers.frag
struct UniformBlock {
  Std140Mat4 qtMatrix;
  float qtOpacity;
  Std140Vec4 markerColor;
  float glSize;
  float radius;
  float strokeWidth;
  int   markerType;
};

/// @brief the markers.frag uniforms: markers with the same style share a material
struct MarkerStyle
{
  QRgba64 color = {};

  /// @brief the side of a marker's quad, fading edge included
  float glSize = 0;

  /// @brief the half extent of the shape
  float radius = 0;

  float strokeWidth = 0;

  /// @brief a zg::PlotStyle::PointStyle
  int markerType = 0;

  auto operator<=>(const MarkerStyle&) const = default;
};

class MarkersMaterial : public StyledMaterial<MarkerStyle>
{
public:
  MarkersMaterial() { setFlag(Blending, true); }

  QSGMaterialShader* createShader(QSGRendererInterface::RenderMode) const override;
};

class MarkersMaterialShader : public QSGMaterialShader
{
public:
  MarkersMaterialShader()
  {
    setShaderFileName(VertexStage,   QStringLiteral(":/zegrapher/shaders/markers.vert.qsb"));
    setShaderFileName(FragmentStage, QStringLiteral(":/zegrapher/shaders/markers.frag.qsb"));
  }

  bool updateUniformData(RenderState& state, QSGMaterial* newMaterial, QSGMaterial*) override
  {
    UniformBlock& u = uniformBlock<UniformBlock>(state);

    const MarkerStyle& style = static_cast<const MarkersMaterial*>(newMaterial)->style;
    u.markerColor = toStd140(style.color);
    u.glSize = style.glSize;
    u.radius = style.radius;
    u.strokeWidth = style.strokeWidth;
    u.markerType = style.markerType;

    return true;
  }
};

QSGMaterialShader* MarkersMaterial::createShader(QSGRendererInterface::RenderMode) const
{
  return new MarkersMaterialShader;
}

} // namespace

MarkerNode::MarkerNode()
{
  // the batch renderer merges 16-bit indexed geometry and regenerates 16-bit
  // indices, which wrap past 65535 vertices
  auto* g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0, 0, QSGGeometry::UnsignedIntType);
  g->setDrawingMode(QSGGeometry::DrawTriangles);
  g->setVertexDataPattern(QSGGeometry::StreamPattern);
  setGeometry(g);
  setFlag(OwnsGeometry);
  setMaterial(new MarkersMaterial);
  setFlag(OwnsMaterial);
}

void MarkerNode::set(const MarkerGeometry& markers)
{
  // NaN centers mark the breaks in the curve
  auto centers = markers.centers | std::views::filter([](const QPointF& p) {
    return not std::isnan(p.x()) and not std::isnan(p.y());
  });

  QSGGeometry* g = geometry();
  g->allocate(int(std::ranges::distance(centers) * 6));

  QSGGeometry::Point2D* points = g->vertexDataAsPoint2D();
  for (const QPointF& p : centers)
    points = std::ranges::fill_n(points, 6, QSGGeometry::Point2D{float(p.x()), float(p.y())});
  markDirty(DirtyGeometry);

  const zg::CurveStyle& style = markers.style;
  static_cast<MarkersMaterial*>(material())->style = MarkerStyle{
    .color = style.color.rgba64(),
    .glSize = 4.0f * float(style.pointWidth) + 2.0f * aaPad / markers.pxPerUnit,
    .radius = float(style.pointWidth),
    .strokeWidth = float(style.lineWidth),
    .markerType = int(style.pointStyle),
  };
  markDirty(DirtyMaterial);
}
