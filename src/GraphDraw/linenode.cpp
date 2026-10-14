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

#include "GraphDraw/linenode.h"

#include <QSGMaterial>
#include <QSGMaterialShader>

#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <numeric>
#include <ranges>

namespace {

constexpr int maxDashes = 8;

// std140 layout of the lines.vert and lines.frag uniform block: alignas puts
// each vec4 and mat4 on a 16 byte boundary
struct alignas(16) Vec4 { float x, y, z, w; };
struct alignas(16) Mat4 { float m[16]; };

struct UniformBlock {
  Mat4  qtMatrix;
  float qtOpacity;
  Vec4  lineColor;
  float halfWidth;
  float alphaScale;
  float pxPerUnit;
  int   dashCount;
  float dashPeriod;
  std::array<Vec4, maxDashes / 4> dashes;
};

static_assert(offsetof(UniformBlock, dashes) == 128 and sizeof(UniformBlock) == 160);

class LineMaterial : public QSGMaterial
{
public:
  LineMaterial() { setFlag(Blending, true); }

  QSGMaterialType* type() const override
  {
    static QSGMaterialType t;
    return &t;
  }

  QSGMaterialShader* createShader(QSGRendererInterface::RenderMode) const override;

  int compare(const QSGMaterial* other) const override
  {
    const auto order = style <=> static_cast<const LineMaterial*>(other)->style;
    return order < 0 ? -1 : order > 0;
  }

  LineStyle style;
};

class LineMaterialShader : public QSGMaterialShader
{
public:
  LineMaterialShader()
  {
    setShaderFileName(VertexStage,   QStringLiteral(":/zegrapher/shaders/lines.vert.qsb"));
    setShaderFileName(FragmentStage, QStringLiteral(":/zegrapher/shaders/lines.frag.qsb"));
  }

  bool updateUniformData(RenderState& state, QSGMaterial* newMaterial, QSGMaterial*) override
  {
    QByteArray* buf = state.uniformData();
    Q_ASSERT(buf->size() >= qsizetype(sizeof(UniformBlock)));
    auto* u = reinterpret_cast<UniformBlock*>(buf->data());

    if (state.isMatrixDirty())
      std::memcpy(u->qtMatrix.m, state.combinedMatrix().constData(), sizeof(u->qtMatrix));
    if (state.isOpacityDirty())
      u->qtOpacity = state.opacity();

    const LineStyle& style = static_cast<const LineMaterial*>(newMaterial)->style;
    const QColor color = QColor::fromRgba64(style.color);
    u->lineColor = Vec4{float(color.redF()), float(color.greenF()),
                        float(color.blueF()), float(color.alphaF())};
    u->halfWidth = style.halfWidth;
    u->alphaScale = style.alphaScale;
    u->pxPerUnit = style.pxPerUnit;
    u->dashCount = int(style.dashes.size());
    u->dashPeriod = style.dashPeriod;

    std::array<float, maxDashes> dashes = {};
    std::ranges::copy(style.dashes, dashes.begin());
    u->dashes = std::bit_cast<decltype(u->dashes)>(dashes);

    return true;
  }
};

QSGMaterialShader* LineMaterial::createShader(QSGRendererInterface::RenderMode) const
{
  return new LineMaterialShader;
}

const QSGGeometry::AttributeSet& lineAttributes()
{
  static const QSGGeometry::Attribute attributes[] = {
    QSGGeometry::Attribute::createWithAttributeType(0, 2, QSGGeometry::FloatType, QSGGeometry::PositionAttribute),
    QSGGeometry::Attribute::createWithAttributeType(1, 4, QSGGeometry::FloatType, QSGGeometry::UnknownAttribute),
  };
  static const QSGGeometry::AttributeSet set = {2, sizeof(LineVertex), attributes};
  return set;
}

QPointF normalized(QPointF v)
{
  const double l = std::hypot(v.x(), v.y());
  return l > 0 ? v / l : QPointF(1, 0);
}

QPointF normalOf(QPointF direction)
{
  return QPointF(-direction.y(), direction.x());
}

LineVertex vertex(QPointF p, float along, float across, float length, float travelled)
{
  return LineVertex{float(p.x()), float(p.y()), along, across, length, travelled};
}

} // namespace

LineGeometry::LineGeometry(QColor color, double width, const QList<qreal>& dashPattern, float pxPerUnit)
{
  style.color = color.rgba64();
  style.pxPerUnit = pxPerUnit;

  const float half = float(width) / 2.f;
  const float minHalf = 0.5f / pxPerUnit;
  style.halfWidth = std::max(half, minHalf);
  style.alphaScale = std::min(half / minHalf, 1.f);
  extent = style.halfWidth + 0.6f / pxPerUnit;

  // QPen counts a width of 0 as 1
  const float unit = width > 0 ? float(width) : 1.f;
  const qsizetype count = std::min(dashPattern.size() & ~qsizetype(1), qsizetype(maxDashes));
  style.dashes = dashPattern | std::views::take(count)
                 | std::views::transform([&](qreal d) { return float(d) * unit; })
                 | std::ranges::to<std::vector>();
  style.dashPeriod = std::ranges::fold_left(style.dashes, 0.f, std::plus{});
}

void LineGeometry::addQuad(quint32 first)
{
  indices.insert(indices.end(), {first, first + 1, first + 3, first, first + 3, first + 2});
}

void LineGeometry::addCapsule(QPointF a, QPointF b, float travelled)
{
  const QPointF d = b - a;
  const float length = float(std::hypot(d.x(), d.y()));
  const QPointF dir = normalized(d), n = normalOf(dir);
  const float e = extent;

  auto corner = [&](float along, float across) {
    return vertex(a + dir * along + n * across, along, across, length, travelled + along);
  };

  addQuad(quint32(vertices.size()));
  vertices.insert(vertices.end(), {corner(-e, e), corner(-e, -e),
                                   corner(length + e, e), corner(length + e, -e)});
}

void LineGeometry::addSegments(std::span<const QLineF> lines)
{
  vertices.reserve(vertices.size() + 4 * lines.size());
  indices.reserve(indices.size() + 6 * lines.size());
  for (const QLineF& line : lines)
    addCapsule(line.p1(), line.p2(), 0);
}

void LineGeometry::addPolyline(const QPolygonF& points)
{
  // points closer than a quarter device pixel add vertices but no shape
  const double minStep = 0.25 / style.pxPerUnit;
  QPolygonF p;
  p.reserve(points.size());
  for (const QPointF& point : points)
    if (p.empty() or QLineF(p.back(), point).length() >= minStep)
      p.append(point);

  // the line still ends on its last point
  if (not points.empty() and p.back() != points.back())
  {
    if (p.size() > 1)
      p.back() = points.back();
    else p.append(points.back());
  }

  const qsizetype n = p.size();
  if (n == 0)
    return;

  if (n == 1)
  {
    addCapsule(p[0], p[0], 0);
    return;
  }

  const auto segments = std::views::zip(p, p | std::views::drop(1));

  const auto normals = segments | std::views::transform([](const auto& segment) {
                         const auto& [a, b] = segment;
                         return normalOf(normalized(b - a));
                       })
                       | std::ranges::to<std::vector>();

  const auto lengths = segments | std::views::transform([](const auto& segment) {
                         const auto& [a, b] = segment;
                         return float(QLineF(a, b).length());
                       });
  std::vector<float> travelled(n, 0.f);
  std::partial_sum(lengths.begin(), lengths.end(), travelled.begin() + 1);

  const float e = extent;

  // the miter at each point, at distance e from both segments. Sharp turns cut
  // it to 4e
  std::vector<QPointF> offsets(n);
  offsets[0] = normals[0] * e;
  offsets[n - 1] = normals[n - 2] * e;
  for (qsizetype i = 1; i + 1 < n; i++)
  {
    const QPointF sum = normals[i - 1] + normals[i];
    const double l = std::hypot(sum.x(), sum.y());
    if (l < 1e-6)
      offsets[i] = normals[i] * e;
    else
    {
      const QPointF m = sum / l;
      const double cosine = QPointF::dotProduct(m, normals[i]);
      offsets[i] = m * (e / std::max(cosine, 0.25));
    }
  }

  // neighboring segments share the two miter vertices between them
  const quint32 first = quint32(vertices.size());
  vertices.reserve(vertices.size() + 2 * n + 8);
  indices.reserve(indices.size() + 6 * (n - 1) + 12);
  for (qsizetype i = 0; i < n; i++)
  {
    vertices.push_back(vertex(p[i] + offsets[i], 0, e, -1, travelled[i]));
    vertices.push_back(vertex(p[i] - offsets[i], 0, -e, -1, travelled[i]));
  }
  for (qsizetype i = 0; i + 1 < n; i++)
    addQuad(first + 2 * quint32(i));

  // round ends: half discs past both ends. A dashed line ends on its dashes.
  if (not style.dashes.empty())
    return;

  auto cap = [&](QPointF end, QPointF outward) {
    const QPointF n = normalOf(outward);
    auto corner = [&](float along, float across) {
      return vertex(end + outward * along + n * across, along, across, 0, 0);
    };
    addQuad(quint32(vertices.size()));
    vertices.insert(vertices.end(), {corner(0, e), corner(0, -e), corner(e, e), corner(e, -e)});
  };

  cap(p[0], normalized(p[0] - p[1]));
  cap(p[n - 1], normalized(p[n - 1] - p[n - 2]));
}

LineNode::LineNode()
{
  auto* g = new QSGGeometry(lineAttributes(), 0, 0, QSGGeometry::UnsignedIntType);
  g->setDrawingMode(QSGGeometry::DrawTriangles);
  g->setVertexDataPattern(QSGGeometry::StreamPattern);
  g->setIndexDataPattern(QSGGeometry::StreamPattern);
  setGeometry(g);
  setFlag(OwnsGeometry);
  setMaterial(new LineMaterial);
  setFlag(OwnsMaterial);
}

void LineNode::set(const LineGeometry& lines)
{
  QSGGeometry* g = geometry();
  g->allocate(int(lines.vertices.size()), int(lines.indices.size()));
  std::ranges::copy(lines.vertices, static_cast<LineVertex*>(g->vertexData()));
  std::ranges::copy(lines.indices, g->indexDataAsUInt());
  markDirty(DirtyGeometry);

  static_cast<LineMaterial*>(material())->style = lines.style;
  markDirty(DirtyMaterial);
}
