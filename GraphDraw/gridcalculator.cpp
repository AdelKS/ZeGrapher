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

    const ZeAxesSettings &axesSettings = information->getAxesSettings();

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

    double baseStep = pow(10, targetPower);
    double realStep = baseStep * constantMultiplier;

    ZeLinAxisTick firstTick;
    firstTick.pos = ceil(range.min / realStep) * realStep;
    firstTick.multiplier = firstTick.pos / axisSettings.constantMultiplier - axisTicks.offset.sumOffset;

    axisTicks.ticks << firstTick;

    ZeLinAxisTick tick;

    while(axisTicks.ticks.last().pos < range.max)
    {
        tick.pos = axisTicks.ticks.last().pos + realStep;
        tick.multiplier = axisTicks.ticks.last().multiplier + baseStep;

        axisTicks.ticks << tick;
    }

    axisTicks.ticks.removeLast();

    return axisTicks;
}
