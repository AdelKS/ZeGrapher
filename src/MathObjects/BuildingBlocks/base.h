#pragma once

/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "MathObjects/expr.h"
#include "axismapper.h"

namespace zg {

struct StringRange {
  QString start, end;
  bool operator == (const StringRange&) const = default;
};

/// @brief Shared base class for backends that use zc::DynMathObject
///        creates a new handle on instantiation, and releases it on destruction
struct Base: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(mathobj::Expr* start READ getStartPtr)
  Q_PROPERTY(mathobj::Expr* end READ getEndPtr)
  Q_PROPERTY(bool discrete WRITE setDiscrete MEMBER discrete NOTIFY discreteChanged)
  Q_PROPERTY(CoordinateSystem coordinateSystem WRITE setCoordinateSystem MEMBER coordinateSystem NOTIFY coordinateSystemChanged)

public:
  enum CoordinateSystem {Cartesian, Polar};
  Q_ENUM(CoordinateSystem);

  /// @returns stringRange given a coordinate system and is_discrete boolean
  using DefaultStringRangeProvider = std::function<StringRange(CoordinateSystem, bool)>;
  explicit Base(DefaultStringRangeProvider, QObject* parent = nullptr);

  mathobj::Expr* getStartPtr() { return &start; }
  mathobj::Expr* getEndPtr() { return &end; }

  void setCoordinateSystem(CoordinateSystem);
  void setDiscrete(bool);

  void setStart(QString s) { start.setExpression(s); }
  void setEnd(QString s) { end.setExpression(s); }

  QString getStartStr() const { return start.getExpression(); }
  QString getEndStr() const { return end.getExpression(); }

  CoordinateSystem getCoordinateSystem() const { return coordinateSystem; }
  bool isDiscrete() const { return discrete; }

  zg::real_range1d getSnapshot();

  bool isDefaultStart() const;
  bool isDefaultEnd() const;

  void resetStart();
  void resetEnd();

  StringRange getDefaultStringRange() const;

signals:
  void coordinateSystemChanged();
  void discreteChanged();

protected:

  mathobj::Expr start;
  mathobj::Expr end;
  DefaultStringRangeProvider defaultStringRangeProvider;
  CoordinateSystem coordinateSystem = Cartesian;
  bool discrete = false;
};

using CoordinateSystem = Base::CoordinateSystem;

}
