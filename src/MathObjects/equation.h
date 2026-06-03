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

#include "BuildingBlocks/base.h"
#include "BuildingBlocks/zcmathobjectbb.h"
#include "Utils/state.h"

namespace zg {
namespace mathobj {

/// @brief ZeGrapher math objects that are entirely defined by a single math expression
///        which also fits in a single zc::DynMathObject
struct Equation: QObject, shared::ZcMathObjectBB {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString equation WRITE setEquation MEMBER equation NOTIFY equationChanged)
  Q_PROPERTY(State state READ getState WRITE setState NOTIFY stateChanged)

public:

  explicit Equation(QObject *parent = nullptr);

  Q_INVOKABLE State setEquation(QString eq);
  Q_INVOKABLE QString getName() const;
  State sync();
  void setState(State s);

  Q_INVOKABLE bool isValid();
  const State& getState() const { return state; }

  Base base;

signals:
  void updated();
  void equationChanged();
  void stateChanged();

protected:
  QString equation;
  State state;
};

}
}
