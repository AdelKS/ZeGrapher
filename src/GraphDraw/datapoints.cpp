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

#include "GraphDraw/datapoints.h"

#include <QColor>
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGMaterial>
#include <QSGMaterialShader>
#include <QSGNode>
#include <cstring>

namespace {

// Lateral padding past the SDF iso-line so the fragment shader's coverage
// gradient has somewhere to render. ~1px is enough for a 1-pixel AA band;
// the extra absorbs slight gradient anisotropy at oblique angles.
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
  float  glSize      = 0.f;   // gl_PointSize incl. AA padding (pixels)
  float  radius      = 0.f;   // visible half-extent for the SDF (pixels)
  float  strokeWidth = 0.f;   // Cross arm half-thickness (pixels; ignored otherwise)
  int    markerType  = 0;     // matches zg::CurveStyle::PointStyle
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

DataPoints::DataPoints(QQuickItem *parent)
  : QQuickItem(parent)
{
  setFlag(ItemHasContents, true);
}

QSGNode *DataPoints::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
  QSGNode *root = oldNode ? oldNode : new QSGNode;

  while (QSGNode *c = root->firstChild())
  {
    root->removeChildNode(c);
    delete c;
  }

  if (not curve) return root;
  if (not curve->style.visible) return root;
  if (not curve->discrete)      return root;
  if (curve->style.pointStyle == zg::CurveStyle::None) return root;
  if (curve->px_curve.empty())  return root;

  // Count first so we can allocate the QSGGeometry exactly. Skipping
  // NaNs keeps disconnected sample slots out of the geometry.
  qsizetype n = 0;
  for (const QPointF& p : curve->px_curve)
    if (not std::isnan(p.x()) and not std::isnan(p.y()))
      ++n;
  if (n == 0) return root;

  auto *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), n);
  geometry->setDrawingMode(QSGGeometry::DrawPoints);

  auto *points = geometry->vertexDataAsPoint2D();
  qsizetype i = 0;
  for (const QPointF& p : curve->px_curve)
  {
    if (std::isnan(p.x()) or std::isnan(p.y()))
      continue;
    points[i++].set(float(p.x() * totalScaleFactor), float(p.y() * totalScaleFactor));
  }

  auto *material = new MarkersMaterial;
  material->color       = curve->style.color;
  material->glSize      = 4.0f * float(curve->style.pointWidth) + 2.0f * k_aa_pad;
  material->radius      = float(curve->style.pointWidth);
  material->strokeWidth = float(curve->style.lineWidth);
  material->markerType  = int(curve->style.pointStyle);

  auto *node = new QSGGeometryNode;
  node->setGeometry(geometry);
  node->setFlag(QSGNode::OwnsGeometry);
  node->setMaterial(material);
  node->setFlag(QSGNode::OwnsMaterial);

  root->appendChildNode(node);


  return root;
}
