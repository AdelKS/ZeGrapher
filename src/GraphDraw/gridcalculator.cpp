#include "gridcalculator.h"

QString GridCalculator::get_coordinate_string(const ZeLinAxisSettings &axisSettings,
                                              const QString& multiplierStr,
                                              double multiplier)
{
  QString posStr;

  if (not multiplierStr.isEmpty() and multiplier != 0)
  {
    const QString& expr = multiplierStr;
    if (fabs(multiplier + 1.0) <= MAX_NUM_PREC_DEC)
      posStr = "-" + expr;
    else if (fabs(multiplier - 1.0) <= MAX_NUM_PREC_DEC)
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
