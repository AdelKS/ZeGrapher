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

#include "BuildingBlocks/stateful.h"
#include "BuildingBlocks/zcmathobjectbb.h"
#include "Utils/state.h"

namespace zg {

class AnimatedConstant;

namespace mathobj {

/// @brief ZeGrapher math objects that are entirely defined by a single math expression
///        which also fits in a single zc::DynMathObject
struct Constant: Stateful, shared::ZcMathObjectBB {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString name WRITE setName MEMBER input_name)
  Q_PROPERTY(double value WRITE set_value MEMBER value NOTIFY valueChanged)

public:

  explicit Constant(QObject *parent = nullptr);

  Q_INVOKABLE State setName(QString name);
  QString getName() const { return input_name; }
  State sync();

  Q_INVOKABLE void set_value(double val);

  Q_INVOKABLE bool isValid();

  double get_value() const { return value; }

signals:
  void updated();
  void valueChanged();

protected:
  double value = std::nan("");

  QString input_name;

  /// @brief so value can be changed without triggering signals
  ///        used in animations
  friend class zg::AnimatedConstant;
};

}
}
