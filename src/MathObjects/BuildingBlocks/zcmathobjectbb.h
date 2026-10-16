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

#include "structures.h"

#include <zecalculator/zecalculator.h>

namespace zg {
namespace mathobj {
namespace shared {

/// @brief true for an object that takes integer inputs: data and sequences
inline bool holdsDiscrete(const zc::DynMathObject<zc_t>& o)
{
  return o.holds(zc::ObjectType::DATA) or o.holds(zc::ObjectType::SEQUENCE);
}

/// @brief holdsDiscrete(), or nothing when a syntax error leaves 'o' without
///        a type. A sequence that calls an undefined object is still a sequence
inline std::optional<bool> parsedDiscrete(const zc::DynMathObject<zc_t>& o)
{
  if (o.holds(zc::ObjectType::BAD_EQUATION))
    return {};
  return holdsDiscrete(o);
}

/// @brief Shared base class for backends that use zc::DynMathObject
///        creates a new handle on instantiation, and releases it on destruction
struct ZcMathObjectBB {

public:
  explicit ZcMathObjectBB();
  ~ZcMathObjectBB();

  bool isDiscrete() const;
  bool isContinuous() const;

  zc::DynMathObject<zc_t>& zcMathObj;
};

}
}
}
