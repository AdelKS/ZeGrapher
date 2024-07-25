#pragma once

/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
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

#include "Utils/opterror.h"
#include "structures.h"

#include <zecalculator/zecalculator.h>

namespace zg {
namespace mathobj {

/// @brief Shared base class for backends that use zc::DynMathObject
struct ZcBase : QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(OptError optError MEMBER optError NOTIFY optErrorChanged)

public:

  explicit ZcBase(QObject *parent = nullptr);

  ~ZcBase();

  Q_INVOKABLE bool isValid() const;

  /// @note either contains a zc::Function or an Error
  zc::DynMathObject<zc_t>& zcMathObj;

signals:
  void optErrorChanged();

protected:
  OptError optError = {};
};

}
}
