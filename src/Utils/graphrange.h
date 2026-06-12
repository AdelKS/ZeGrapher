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

#include <QtQmlIntegration/qqmlintegration.h>

#include "GraphDraw/axismapper.h"
#include "MathObjects/expr.h"

namespace zg {

struct ZeAxisRange: QObject
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(mathobj::Expr* min READ getMinPtr CONSTANT)
  Q_PROPERTY(mathobj::Expr* max READ getMaxPtr CONSTANT)
  Q_PROPERTY(bool state READ getState NOTIFY stateChanged)

public:

  ZeAxisRange(QObject *parent = nullptr);

  mathobj::Expr* getMinPtr() { return &min; }
  mathobj::Expr* getMaxPtr() { return &max; }
  bool getState() const { return state; }

  mathobj::Expr min;
  mathobj::Expr max;

  real_range1d getSnapshot();

  real_range1d getLatestValidSnapshot();

  void update(const real_range1d& range);

  struct POD {
    std::optional<std::string> min, max;
    operator bool () const { return min or max; }
  };

  void importPod(POD);
  std::optional<POD> exportPod() const;

signals:
  void stateChanged();

protected:
  /// @brief tells whether the latest requested snapshot was valid
  bool state = false;

  real_range1d latestValidSnapshot = real_range1d{.min = {-10.}, .max = {10.}};
};

struct GraphRange: QObject
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(ZeAxisRange* x READ getXPtr CONSTANT)
  Q_PROPERTY(ZeAxisRange* y READ getYPtr CONSTANT)

public:
  GraphRange(QObject* parent = nullptr);

  ZeAxisRange* getXPtr() { return &x; }
  ZeAxisRange* getYPtr() { return &y; }

  ZeAxisRange x;
  ZeAxisRange y;

  QRectF getLatestValidRect();
  real_range2d getLatestValidSnapshot();

  void interactiveUpdate(const real_range2d& range);

  struct POD {
    std::optional<ZeAxisRange::POD> x, y;
    operator bool () const { return x or y; }
  };

  void importPod(POD);
  std::optional<POD> exportPod() const;

signals:
  void interactiveUpdate();
};

} // namespace zg
