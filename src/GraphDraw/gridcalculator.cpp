#include "gridcalculator.h"

using namespace std;

GridCalculator::GridCalculator(const ZeAxesSettings& axesSettings, QObject *parent)
  : QObject(parent),
    axesSettings(axesSettings)
{
  targetTicksNum = TARGET_TICKS_NUM;
}

QString GridCalculator::get_coordinate_string(const ZeLinAxisSettings &axisSettings,
                                              double multiplier)
{
  QString posStr;

  if (axisSettings.constantMultiplier->isValid())
  {
    QString expr = axisSettings.constantMultiplier->getExpression();
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
