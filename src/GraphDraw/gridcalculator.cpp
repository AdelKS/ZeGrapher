#include "gridcalculator.h"

using namespace std;

GridCalculator::GridCalculator(QObject *parent) : QObject(parent)
{
  targetTicksNum = TARGET_TICKS_NUM;
}

QString GridCalculator::get_coordinate_string(const ZeLinAxisSettings &axisSettings,
                                              double multiplier)
{
  QString posStr;

  if (!axisSettings.constantMultiplierStr.isEmpty())
  {
    if (fabs(multiplier + 1.0) <= MAX_NUM_PREC_DEC)
      posStr = "-" + axisSettings.constantMultiplierStr;
    else if (fabs(multiplier - 1.0) <= MAX_NUM_PREC_DEC)
      posStr = axisSettings.constantMultiplierStr;
    else
    {
      posStr.setNum(multiplier, 'g', axisSettings.maxDigitsNum + 1);
      if (axisSettings.constantMultiplierStr[0].isDigit())
        posStr += "×";
      posStr += axisSettings.constantMultiplierStr;
    }
  }
  else
    posStr.setNum(multiplier, 'g', axisSettings.maxDigitsNum + 1);

  return posStr;
}
