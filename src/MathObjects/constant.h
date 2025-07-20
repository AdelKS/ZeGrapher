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

#include "BuildingBlocks/zcmathobjectbb.h"
#include "GraphDraw/axismapper.h"
#include "Utils/state.h"

#include <optional>

namespace zg {
namespace mathobj {

/// @brief ZeGrapher math objects that are entirely defined by a single math expression
///        which also fits in a single zc::DynMathObject
struct Constant: QObject, shared::ZcMathObjectBB {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString name WRITE setName MEMBER input_name)
  Q_PROPERTY(State state READ getState)

public:

  explicit Constant(QObject *parent = nullptr);

  State setName(QString name);
  QString getName() const { return input_name; }
  void setSlot(size_t slot);

  Q_INVOKABLE void set_value(double val);
  Q_INVOKABLE State getState() const;

  Q_INVOKABLE bool isValid() const;

  zg::real_unit operator () () const;
  zg::real_unit evaluate() const;

protected:
  QString input_name;
  std::optional<size_t> slot;
};

}
}
