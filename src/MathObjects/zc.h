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

#include "base.h"
#include "type.h"

namespace zg {
namespace mathobj {

/// @brief ZeGrapher math objects that are entirely defined by a single math expression
///        which also fits in a single zc::DynMathObject
struct ZC: Base {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QString equation WRITE setEquation MEMBER equation)
  Q_PROPERTY(Type type WRITE setType MEMBER type NOTIFY typeChanged)

  static constexpr std::array valid_types = {CONSTANT, FUNCTION, SEQUENCE};

public:

  explicit ZC(QObject *parent = nullptr);

  /// @brief changes the target ZC type
  void setType(Type type);
  void setEquation(QString eq);

  Q_INVOKABLE void refresh(bool canChangeType = true);

  QString getName() const { return name; }

signals:
  void typeChanged(Type);

protected:
  QString equation;
  QString name;

  /// @brief The type of the math object
  /// @note not necessarily in a DynMathObject
  Type type = Type::FUNCTION;
};

}
}
