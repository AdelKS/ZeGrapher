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

#include "zcmathobject.h"
#include <zecalculator/zecalculator.h>

namespace zg {

struct ZcMathObject;

/// @brief holds the names of the two objects used to plot a parametric equation
struct Parametric: QObject {
  Q_OBJECT
  QML_ELEMENT

public:

  explicit Parametric(QObject *parent = nullptr);

  void sync();

  Q_INVOKABLE ZcMathObject* getObj1() { return obj1; }
  Q_INVOKABLE ZcMathObject* getObj2() { return obj2; }

  ZcMathObject* obj1;
  ZcMathObject* obj2;
};

} // namespace zg
