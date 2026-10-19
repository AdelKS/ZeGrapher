/****************************************************************************
**  Copyright (c) 2020, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "gridcalculator.h"

MathText GridCalculator::coordinateText(const ZeLinAxisSettings &axisSettings,
                                        const QString& multiplierStr,
                                        double multiplier)
{
  if (multiplierStr.isEmpty() or multiplier == 0)
    return formatNumber(multiplier, axisSettings.maxDigitsNum + 1);

  if (fabs(multiplier + 1.0) <= unitTolerance)
    return MathText{.base = minusSign + multiplierStr};

  if (fabs(multiplier - 1.0) <= unitTolerance)
    return MathText{.base = multiplierStr};

  MathText text = formatNumber(multiplier, axisSettings.maxDigitsNum + 1);
  text.tail += (multiplierStr[0].isDigit() ? "×" : "") + multiplierStr;
  return text;
}
