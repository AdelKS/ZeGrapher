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

#pragma once

#include "zecalculator/parsing/types.h"

inline constexpr double cmPerInch = 2.54;

/// @brief type of math objects to use in ZeCalculator
inline constexpr zc::parsing::Type zc_t = zc::parsing::Type::RPN;

enum struct ZeAxisName {X, Y};

template<class Number>
Number int_pow(Number a, int b)
{
    Number res = 1;
    if(b < 0)
    {
        for(int i = 0 ; i > b ; i--)
            res /= a;
    }
    else
    {
        for(int i = 0 ; i < b ; i++)
            res *= a;
    }

    return res;
}
