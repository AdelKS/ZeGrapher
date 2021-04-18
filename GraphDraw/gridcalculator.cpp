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

int getMaxStrPxSize(ZeAxisName axisname, const ZeAxisRange &range, const QString cstMulStr, double realStep, int maxDigitsNum, const QFontMetrics &metrics)
{
    double pos = ceil(range.min / realStep) * realStep;
    int maxStrPxSize = 0, currentStrPxSize = 0;
    QString posStr;

    while(pos < range.max)
    {
        posStr.setNum(pos, 'g', maxDigitsNum);
        posStr += cstMulStr;

        currentStrPxSize = axisname == ZeAxisName::X ? metrics.horizontalAdvance(posStr) : metrics.height() ;

        if(currentStrPxSize > maxStrPxSize)
            maxStrPxSize = currentStrPxSize;

        pos += realStep;
    }

    return maxStrPxSize;
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
    double amplitude = range.amplitude();

    double amplitudeLog10 = floor(log10(amplitude));
    double minLog10 = floor(log10(fabs(range.min) / constantMultiplier));

    if(amplitudeLog10 < minLog10 - axisSettings.maxDigitsNum)
    {
        double digitsNum = minLog10 - axisSettings.maxDigitsNum - amplitudeLog10;

        double tenPower = pow(10, digitsNum);
        axisTicks.offset.sumOffset = trunc(range.min * tenPower) / tenPower;
    }

    int targetPower = lround(log10(amplitude / constantMultiplier / targetTicksNum));

    double baseStep;
    double realStep;
    double baseMultiplier = 1;
    double pos;

    int maxStrPxSize = 0;
    int currentStrPxSize = 0;

    bool goodSpacing = false;

    int previousLoopChange = 0;

    while(not goodSpacing)
    {
        baseStep = pow(10, targetPower);
        realStep = baseStep * baseMultiplier * constantMultiplier;

        int maxStrPxSize = getMaxStrPxSize(axisName, range, axisSettings.constantMultiplierStr, realStep, axisSettings.maxDigitsNum, metrics);

        if(double(maxStrPxSize) - realStep * pxPerUnit > 0)
        {
            base10Inc(targetPower, baseMultiplier);
            previousLoopChange = 1;
        }
        else if(realStep * pxPerUnit / previousMultiplier(baseMultiplier) - double(maxStrPxSize) > 0 and previousLoopChange != 1)
        {
            base10Dec(targetPower, baseMultiplier);
            previousLoopChange = -1;
        }
        else goodSpacing = true;
    }

    int relTickSpacing = axisName == ZeAxisName::X ? information->getAxesSettings().x.tickRelSpacing : information->getAxesSettings().y.tickRelSpacing;

    if(relTickSpacing > 0)
    {
        for(int i=0; i < relTickSpacing; i++)
            base10Inc(targetPower, baseMultiplier);
        baseStep = pow(10, targetPower);
        realStep = baseStep * baseMultiplier * constantMultiplier;
    }

    fesetround(FE_DOWNWARD);
    int maxRangePower = max(lrint(log10(fabs(range.max))), lrint(log10(fabs(range.min))));

    QString axisNameStr = axisName == ZeAxisName::X ? "x axis" : "y axis";
    qDebug() << axisNameStr << " Max range power " << maxRangePower;
    qDebug() << axisNameStr << " step power " << targetPower;

    axisTicks.offset.sumOffset = 0;
    axisTicks.offset.sumPowerOffset = 0;
    axisTicks.offset.basePowerOffset = 0;
    if(abs(targetPower) > axisSettings.maxDigitsNum or
            maxRangePower - targetPower > axisSettings.maxDigitsNum)
    {
//        axisTicks.offset.basePowerOffset = targetPower - axisSettings;
    }

    ZeLinAxisTick tick;
    tick.pos = ceil(range.min / realStep) * realStep;
    tick.posStr.setNum(tick.pos, 'g', axisSettings.maxDigitsNum);

    while(tick.pos < range.max)
    {
        axisTicks.ticks << tick;

        tick.pos += realStep;
        tick.posStr.setNum(tick.pos, 'g', axisSettings.maxDigitsNum);
        tick.posStr += axisSettings.constantMultiplierStr;

    }

    return axisTicks;
}
