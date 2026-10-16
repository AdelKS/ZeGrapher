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

#include <QColor>
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGMaterial>
#include <QSGMaterialShader>
#include <QSGNode>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <ranges>

namespace {

// Lateral padding past the SDF iso-line so the fragment shader's coverage
// gradient has somewhere to render, in device pixels. ~1 px is enough for a
// 1-pixel AA band; the extra absorbs slight gradient anisotropy at oblique angles.
constexpr float k_aa_pad = 1.5f;

// Typed std140 building blocks. The alignas attribute encodes the std140
// rule that vec4 / mat4 must sit on a 16-byte boundary, so adding or
// reordering members in UniformBlock below doesn't require manual padding
// bookkeeping — the compiler inserts the right padding automatically.
struct alignas(16) Vec4 { float x, y, z, w; };
struct alignas(16) Mat4 { float m[16]; };

// Mirrors the std140 uniform block in markers.{vert,frag}. Scalars (float
// / int) follow C++ natural alignment, which matches std140's scalar
// packing rule; Vec4 / Mat4 carry their own 16-byte alignment.
struct UniformBlock {
  Mat4  qtMatrix;
  float qtOpacity;
  Vec4  markerColor;
  float glSize;
  float radius;
  float strokeWidth;
  int   markerType;
};

/// One material instance per curve. Each draw call writes its style fields
/// into the std140 uniform block alongside qt_Matrix / qt_Opacity.
class MarkersMaterial : public QSGMaterial
{
public:
  MarkersMaterial() { setFlag(Blending, true); }

  QSGMaterialType   *type() const override
  {
    static QSGMaterialType t;
    return &t;
  }

  QSGMaterialShader *createShader(QSGRendererInterface::RenderMode) const override;

  int compare(const QSGMaterial *other) const override
  {
    // Field-by-field comparison so the batched renderer can merge curves
    // that share style into a single draw call. compare() must be a strict
    // ordering, hence the cascading lexicographic checks.
    const auto *o = static_cast<const MarkersMaterial *>(other);
    if (markerType   != o->markerType)   return markerType   < o->markerType   ? -1 : 1;
    if (glSize       != o->glSize)       return glSize       < o->glSize       ? -1 : 1;
    if (radius       != o->radius)       return radius       < o->radius       ? -1 : 1;
    if (strokeWidth  != o->strokeWidth)  return strokeWidth  < o->strokeWidth  ? -1 : 1;
    if (color.rgba() != o->color.rgba()) return color.rgba() < o->color.rgba() ? -1 : 1;
    return 0;
  }

  // Per-curve uniform state. Read by the shader at the std140 offsets above.
  QColor color       = Qt::black;
  float  glSize      = 0.f;   // Quad side incl. AA padding (drawing units)
  float  radius      = 0.f;   // Visible half-extent for the SDF (drawing units)
  float  strokeWidth = 0.f;   // Cross arm half-thickness (drawing units; ignored otherwise)
  int    markerType  = 0;     // matches zg::PlotStyle::PointStyle
};

class MarkersMaterialShader : public QSGMaterialShader
{
public:
  MarkersMaterialShader()
  {
    setShaderFileName(VertexStage,   QStringLiteral(":/zegrapher/shaders/markers.vert.qsb"));
    setShaderFileName(FragmentStage, QStringLiteral(":/zegrapher/shaders/markers.frag.qsb"));
  }

  bool updateUniformData(RenderState& state, QSGMaterial *newMat, QSGMaterial *) override
  {
    QByteArray *buf = state.uniformData();
    Q_ASSERT(buf->size() >= qsizetype(sizeof(UniformBlock)));
    auto *u = reinterpret_cast<UniformBlock *>(buf->data());

    if (state.isMatrixDirty())
      memcpy(u->qtMatrix.m, state.combinedMatrix().constData(), sizeof(u->qtMatrix));
    if (state.isOpacityDirty())
      u->qtOpacity = state.opacity();

    // Style fields are written every call: cheap, and the framework only
    // calls updateUniformData when state actually changed.
    const auto *mat = static_cast<const MarkersMaterial *>(newMat);
    u->markerColor.x  = float(mat->color.redF());
    u->markerColor.y  = float(mat->color.greenF());
    u->markerColor.z  = float(mat->color.blueF());
    u->markerColor.w  = float(mat->color.alphaF());
    u->glSize         = mat->glSize;
    u->radius         = mat->radius;
    u->strokeWidth    = mat->strokeWidth;
    u->markerType     = mat->markerType;

    return true;
  }
};

QSGMaterialShader *MarkersMaterial::createShader(QSGRendererInterface::RenderMode) const
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
  auto* m = static_cast<MarkersMaterial*>(material());
  m->color       = style.color;
  m->glSize      = 4.0f * float(style.pointWidth) + 2.0f * k_aa_pad / markers.pxPerUnit;
  m->radius      = float(style.pointWidth);
  m->strokeWidth = float(style.lineWidth);
  m->markerType  = int(style.pointStyle);
  markDirty(DirtyMaterial);
}
