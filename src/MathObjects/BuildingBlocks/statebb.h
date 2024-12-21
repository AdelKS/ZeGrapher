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

#include "Utils/state.h"

#include <zecalculator/zecalculator.h>

namespace zg {
namespace mathobj {
namespace shared {

/// @brief Shared base class for backends that use zc::DynMathObject
struct StateBB : QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("This is an internal class that is used as a building block for QML math object backends");

  Q_PROPERTY(State* state MEMBER state)

public:

  explicit StateBB(QObject *parent = nullptr);

  Q_INVOKABLE bool isValid() const;

protected:
  State* state = nullptr;
};

}
}
}
