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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "BuildingBlocks/base.h"
#include "BuildingBlocks/zcmathobjectbb.h"
#include "Utils/state.h"

namespace zg {
namespace mathobj {

/// @brief ZeGrapher math objects that are entirely defined by a single math expression
///        which also fits in a single zc::DynMathObject
struct Equation: Base, shared::ZcMathObjectBB {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString equation WRITE setEquation MEMBER equation NOTIFY equationChanged)
  Q_PROPERTY(zg::State state READ getState NOTIFY stateChanged)

public:

  explicit Equation(QObject *parent = nullptr);

  Q_INVOKABLE void setEquation(QString eq);
  State sync();

  bool isValid() const { return state.isValid(); }
  const State& getState() const { return state; }
  QString getEquation() const { return equation; }

  EvalHandle getZcObject() const { return &zcMathObj; }
  Revision getRevision() const { return {std::pair(&zcMathObj, zcMathObj.get_revision())}; }

 struct POD {
    std::optional<std::string> equation;
    std::optional<std::string> start;
    std::optional<std::string> end;
    std::optional<CoordinateSystem> coordinates;
    std::optional<PlotStyle::POD> style;
  };

  POD exportPod() const;
  void importPod(POD);

signals:
  void equationChanged();
  void stateChanged();

protected:
  void setState(State s);

  QString equation;
  State state;
};

}
}
