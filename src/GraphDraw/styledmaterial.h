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

#include <QByteArray>
#include <QColor>
#include <QRgba64>
#include <QSGMaterial>
#include <QSGMaterialShader>

#include <compare>
#include <cstring>

// std140 layout: alignas puts each vec4 and mat4 on a 16 byte boundary
struct alignas(16) Std140Vec4 { float x, y, z, w; };
struct alignas(16) Std140Mat4 { float m[16]; };

inline Std140Vec4 toStd140(QRgba64 rgba)
{
  const QColor c = QColor::fromRgba64(rgba);
  return {float(c.redF()), float(c.greenF()), float(c.blueF()), float(c.alphaF())};
}

/// @brief a material whose uniforms all come from 'style', so nodes with equal
///        styles share a batch
template <class Style>
class StyledMaterial : public QSGMaterial
{
public:
  QSGMaterialType* type() const override
  {
    static QSGMaterialType t;
    return &t;
  }

  int compare(const QSGMaterial* other) const override
  {
    const auto order = style <=> static_cast<const StyledMaterial*>(other)->style;
    return order < 0 ? -1 : order > 0;
  }

  Style style;
};

/// @brief the shader's uniform block, with its matrix and opacity updated
/// @note 'Block' starts with 'Std140Mat4 qtMatrix' and 'float qtOpacity', for
///       the qt_Matrix and qt_Opacity that start the shaders' uniform block
template <class Block>
Block& uniformBlock(QSGMaterialShader::RenderState& state)
{
  QByteArray* buf = state.uniformData();
  Q_ASSERT(buf->size() >= qsizetype(sizeof(Block)));
  auto& u = *reinterpret_cast<Block*>(buf->data());

  if (state.isMatrixDirty())
    std::memcpy(u.qtMatrix.m, state.combinedMatrix().constData(), sizeof(u.qtMatrix));
  if (state.isOpacityDirty())
    u.qtOpacity = state.opacity();

  return u;
}
