#include "gridcalculator.h"

GridCalculator::GridCalculator(Information *info, QObject *parent) : QObject(parent)
{
    information = info;

    targetTicksNum = TARGET_TICKS_NUM;
}

ZeLinAxisTicks GridCalculator::getLinearAxisTicks(double windowWidth,
                                            ZeAxisRange range,
                                            ZeAxisName axisName,
                                            QFontMetrics metrics)
{

    const ZeAxisSettings &axisSettings = axisName == ZeAxisName::X ? information->getAxesSettings().x : information->getAxesSettings().y;

    double pxPerUnit = windowWidth / range.amplitude();

    ZeLinAxisTicks axisTicks;
    axisTicks.offset.sumOffset = 0;
    axisTicks.offset.powerOffset = 0;

    ZeLinAxisSettings axisSettings = axisName == ZeAxisName::X ? axesSettings.x.linSettings : axesSettings.y.linSettings;

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

    double targetPower = floor(log10(amplitude / constantMultiplier / targetTicksNum));

    double baseStep;
    double realStep;
    double baseMultiplier = 1;

    double maxStrPxSize = 0;

    realStep = baseStep * baseMultiplier * constantMultiplier;
    baseStep = pow(10, targetPower);

    ZeLinAxisTick firstTick;
    firstTick.pos = ceil(range.min / realStep) * realStep;
    firstTick.multiplier = firstTick.pos / axisSettings.constantMultiplier / baseMultiplier - axisTicks.offset.sumOffset;

    double pos = firstTick.pos;
    QString posStr;

    while(pos < range.max)
    {
        posStr = QString(pos, 'g', axisSettings.maxDigitsNum);

        if(metrics.horizontalAdvance(posStr) > maxStrPxSize)
        {

        }


        axisTicks.ticks << tick;
    }


    axisTicks.ticks << firstTick;

    ZeLinAxisTick tick;

    while(axisTicks.ticks.last().pos < range.max)
    {
        tick.pos = axisTicks.ticks.last().pos + realStep;
        tick.multiplier = axisTicks.ticks.last().multiplier + baseStep;
        tick.posStr = QString(tick.pos, 'g', axisSettings.maxDigitsNum);

        if(metrics.horizontalAdvance(tick.posStr) > maxCoordinateSize)
        {

        }


        axisTicks.ticks << tick;
    }

    axisTicks.ticks.removeLast();


    return axisTicks;
}
