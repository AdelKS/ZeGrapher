#include "gridcalculator.h"

GridCalculator::GridCalculator(Information *info, QObject *parent) : QObject(parent)
{
    information = info;

    targetTicksNum = TARGET_TICKS_NUM;
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
    if(baseMultiplier == 2)
        baseMultiplier = 1;
    if(baseMultiplier == 5)
        baseMultiplier = 2;

}

ZeLinAxisTicks GridCalculator::getLinearAxisTicks(double windowWidth,
                                            ZeAxisRange range,
                                            ZeAxisName axisName,
                                            QFontMetrics metrics)
{

    const ZeLinAxisSettings &axisSettings = axisName == ZeAxisName::X ? information->getAxesSettings().x.linSettings : information->getAxesSettings().y.linSettings;

    double pxPerUnit = windowWidth / range.amplitude();

    ZeLinAxisTicks axisTicks;
    axisTicks.offset.sumOffset = 0;
    axisTicks.offset.powerOffset = 0;

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

    double baseStep = pow(10, targetPower);
    double realStep;
    double baseMultiplier = 1;
    double pos;

    int maxStrPxSize = 0;
    int currentStrPxSize = 0;

    bool goodSpacing = false;

    while(not goodSpacing)
    {
        realStep = baseStep * baseMultiplier * constantMultiplier;
        baseStep = pow(10, targetPower);

        pos = ceil(range.min / realStep) * realStep;
        QString posStr;

        while(pos < range.max)
        {
            posStr.setNum(pos, 'g', axisSettings.maxDigitsNum);
            posStr += axisSettings.constantMultiplierStr;

            currentStrPxSize = axisName == ZeAxisName::X ? metrics.horizontalAdvance(posStr) : metrics.height() ;

            if(currentStrPxSize > maxStrPxSize)
                maxStrPxSize = currentStrPxSize;

            pos += realStep;
        }

        if(double(maxStrPxSize) > MAX_REL_TICK_SPACING * realStep * pxPerUnit)
            base10Inc(targetPower, baseMultiplier);
        else if(double(maxStrPxSize) < MIN_REL_TICK_SPACING * realStep * pxPerUnit)
            base10Dec(targetPower, baseMultiplier);
        else goodSpacing = true;
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
