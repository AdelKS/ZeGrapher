#include <cfenv>
#include <algorithm>

#include "gridcalculator.h"

using namespace std;

GridCalculator::GridCalculator(Information *info, QObject *parent) : QObject(parent)
{
    information = info;

    targetTicksNum = TARGET_TICKS_NUM;
}

double nextMultiplier(double baseMultiplier)
{
    if(baseMultiplier == 2)
        return 2.5;
    else return 2;
}

double previousMultiplier(double baseMultiplier)
{
    if(baseMultiplier == 2)
        return 2;
    else return 2.5;
}

void base10Inc(int &targetPower, double &baseMultiplier)
{
    if(baseMultiplier == 1)
        baseMultiplier = 2;
    else if(baseMultiplier == 2)
        baseMultiplier = 5;
    else if(baseMultiplier == 5)
    {
        baseMultiplier = 1;
        targetPower++;
    }
}

void base10Dec(int &targetPower, double &baseMultiplier)
{
    if(baseMultiplier == 1)
    {
        targetPower--;
        baseMultiplier = 5;
    }
    else if(baseMultiplier == 2)
        baseMultiplier = 1;
    else if(baseMultiplier == 5)
        baseMultiplier = 2;

}

int GridCalculator::getMaxStrPxSize(ZeAxisName axisname, const ZeAxisRange &scaled_range, double realStep, const QFontMetrics &metrics)
{
    double multiplier = floor(scaled_range.min / realStep) * realStep;
    int maxStrPxSize = 0, currentStrPxSize = 0;
    QString posStr;

    while(multiplier < scaled_range.max)
    {
        posStr = get_coordinate_string(axisname, multiplier);

        currentStrPxSize = axisname == ZeAxisName::X ? metrics.horizontalAdvance(posStr) : metrics.height() ;

        if(currentStrPxSize > maxStrPxSize)
            maxStrPxSize = currentStrPxSize;

        multiplier += realStep;
    }

    return maxStrPxSize;
}

QString GridCalculator::get_coordinate_string(ZeAxisName axisname, double multiplier)
{
    const ZeLinAxisSettings &axisSettings = axisname == ZeAxisName::X ? information->getAxesSettings().x.linSettings : information->getAxesSettings().y.linSettings;

    QString posStr;

    if(! axisSettings.constantMultiplierStr.isEmpty())
    {
        if(fabs(multiplier + 1.0) <= MAX_NUM_PREC_DEC)
            posStr = "-" + axisSettings.constantMultiplierStr;
        else if (fabs(multiplier - 1.0) <= MAX_NUM_PREC_DEC)
            posStr = axisSettings.constantMultiplierStr;
        else
        {
            posStr.setNum(multiplier, 'g', axisSettings.maxDigitsNum);
            if(axisSettings.constantMultiplierStr[0].isDigit()) posStr += "×";
            posStr += axisSettings.constantMultiplierStr;
        }
    }
    else posStr.setNum(multiplier, 'g', axisSettings.maxDigitsNum);

    return posStr;
}

ZeLinAxisTicks GridCalculator::getLinearAxisTicks(double windowWidth,
                                            const ZeAxisRange &range,
                                            ZeAxisName axisName,
                                            const QFontMetrics &metrics)
{

    qDebug() << "New tick spacing calculation";

    const ZeLinAxisSettings &axisSettings = axisName == ZeAxisName::X ? information->getAxesSettings().x.linSettings : information->getAxesSettings().y.linSettings;

    double pxPerUnit = windowWidth / range.amplitude();

    ZeLinAxisTicks axisTicks;

    const double &constantMultiplier = axisSettings.constantMultiplier;

    ZeAxisRange scaledRange(range);
    scaledRange /= constantMultiplier;

    double scaled_amplitude = scaledRange.amplitude();

    double amplitudeLog10 = floor(log10(scaled_amplitude));
    double minLog10 = floor(log10(fabs(scaledRange.min)));

    if(amplitudeLog10 < minLog10 - axisSettings.maxDigitsNum)
    {
        double digitsNum = minLog10 - axisSettings.maxDigitsNum - amplitudeLog10;

        double tenPower = pow(10, digitsNum);
        axisTicks.offset.sumOffset = trunc(range.min * tenPower) / tenPower;
    }

    int targetPower = lround(log10(scaled_amplitude / targetTicksNum));

    double baseStep;
    double realStep;
    double baseMultiplier = 1;

    bool goodSpacing = false;

    bool previously_increased = false;

    while(not goodSpacing)
    {
        baseStep = int_pow(10.0, targetPower);
        realStep = baseStep * baseMultiplier;

        int maxStrPxSize = getMaxStrPxSize(axisName, scaledRange, realStep, metrics);

        if(double(maxStrPxSize) - constantMultiplier * realStep * pxPerUnit > 0)
        {
            base10Inc(targetPower, baseMultiplier);
            previously_increased = true;
        }
        else if(realStep * pxPerUnit * constantMultiplier / previousMultiplier(baseMultiplier) - double(maxStrPxSize) > 0 && ! previously_increased)
        {
            base10Dec(targetPower, baseMultiplier);
            previously_increased = false;
        }
        else goodSpacing = true;
    }

    int relTickSpacing = axisName == ZeAxisName::X ? information->getAxesSettings().x.tickRelSpacing : information->getAxesSettings().y.tickRelSpacing;

    if(relTickSpacing > 0)
    {
        for(int i=0; i < relTickSpacing; i++)
            base10Inc(targetPower, baseMultiplier);
        baseStep = pow(10, targetPower);
        realStep = baseStep * baseMultiplier;
    }

    fesetround(FE_DOWNWARD);
    int maxRangePower = max(lrint(log10(fabs(scaledRange.max))), lrint(log10(fabs(scaledRange.min))));

    QString axisNameStr = axisName == ZeAxisName::X ? "x axis" : "y axis";
    qDebug() << axisNameStr << " Max range power " << maxRangePower;
    qDebug() << axisNameStr << " step power " << targetPower;

    axisTicks.offset.sumOffset = 0;
    axisTicks.offset.sumPowerOffset = 0;
    axisTicks.offset.basePowerOffset = 0;
    if(abs(targetPower) > axisSettings.maxDigitsNum or
            maxRangePower - targetPower > axisSettings.maxDigitsNum)
    {
        axisTicks.offset.basePowerOffset = targetPower - axisSettings.maxDigitsNum;
    }

    double multiplier = (floor(scaledRange.min / realStep) - 1) * realStep ;

    do
    {
        ZeLinAxisTick tick;
        multiplier += realStep;
        tick.pos = multiplier * constantMultiplier;
        tick.posStr = get_coordinate_string(axisName, multiplier);

        axisTicks.ticks << tick;

    }while(multiplier < scaledRange.max);

    return axisTicks;
}
