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
  Q_PROPERTY(mathobj::Expr* min READ getMinBackend)
  Q_PROPERTY(mathobj::Expr* max READ getMaxBackend)

public:

  ZeAxisRange(QObject *parent = nullptr);
  ~ZeAxisRange();

  Q_INVOKABLE mathobj::Expr* getMinBackend() const { return min; }
  Q_INVOKABLE mathobj::Expr* getMaxBackend() const { return max; }

  mathobj::Expr* const min;
  mathobj::Expr* const max;

  real_range1d getSnapshot();

  real_range1d getLatestValidSnapshot();

  real_range1d snapshot() const;
  void update(const real_range1d& range);

protected:
  real_range1d latestValidSnapshot = real_range1d{.min = {-10.}, .max = {10.}};
};

struct GraphRange: QObject
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(ZeAxisRange* x READ getXBackend)
  Q_PROPERTY(ZeAxisRange* y READ getYBackend)

public:
  GraphRange(QObject* parent = nullptr);

  Q_INVOKABLE ZeAxisRange* getXBackend() const { return x; }
  Q_INVOKABLE ZeAxisRange* getYBackend() const { return y; }

  ZeAxisRange* const x;
  ZeAxisRange* const y;

  QRectF getLatestValidRect() const;
  real_range2d getLatestValidSnapshot() const;

  void update(const real_range2d& range);
};

} // namespace zg
