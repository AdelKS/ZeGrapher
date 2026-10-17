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

QString GridCalculator::get_coordinate_string(const ZeLinAxisSettings &axisSettings,
                                              const QString& multiplierStr,
                                              double multiplier)
{
  QString posStr;

  if (not multiplierStr.isEmpty() and multiplier != 0)
  {
    const QString& expr = multiplierStr;
    if (fabs(multiplier + 1.0) <= unitTolerance)
      posStr = "-" + expr;
    else if (fabs(multiplier - 1.0) <= unitTolerance)
      posStr = expr;
    else
    {
      posStr.setNum(multiplier, 'g', axisSettings.maxDigitsNum + 1);
      if (expr[0].isDigit())
        posStr += "×";
      posStr += expr;
    }
  }
  else
    posStr.setNum(multiplier, 'g', axisSettings.maxDigitsNum + 1);

  return posStr;
}
