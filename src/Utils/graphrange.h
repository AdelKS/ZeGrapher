#pragma once

/****************************************************************************
**  Copyright (c) 2025, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include <QtQml/qqmlregistration.h>

#include "GraphDraw/axismapper.h"
#include "MathObjects/expr.h"

namespace zg {

struct ZeAxisRange: QObject
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(mathobj::Expr* min MEMBER min)
  Q_PROPERTY(mathobj::Expr* max MEMBER max)

public:

  ZeAxisRange(QObject *parent = nullptr) : QObject(parent) {};

  mathobj::Expr* min = nullptr;
  mathobj::Expr* max = nullptr;

  double amplitude() const;

  double getMin() const;

  double getMax() const;

  real_range1d snapshot() const;
  void update(const real_range1d& range);
};

struct GraphRange: QObject
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(ZeAxisRange* x MEMBER x)
  Q_PROPERTY(ZeAxisRange* y MEMBER y)

public:
  GraphRange(QObject* parent = nullptr)
    : QObject(parent), x(new ZeAxisRange(this)), y(new ZeAxisRange(this)) {};

  ZeAxisRange* x;
  ZeAxisRange* y;

  QRectF getRect() const;
  void update(const real_range2d& range);
  real_range2d snapshot() const;
};

} // namespace zg
