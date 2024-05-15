#include <cfenv>
#include <algorithm>

#include "gridcalculator.h"
#include "information.h"

using namespace std;

GridCalculator::GridCalculator(QObject *parent) : QObject(parent)
{


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
    const ZeLinAxisSettings &axisSettings = axisname == ZeAxisName::X ? information.getAxesSettings().x.linSettings : information.getAxesSettings().y.linSettings;

    QString posStr;

    if(! axisSettings.constantMultiplierStr.isEmpty())
    {
        if(fabs(multiplier + 1.0) <= MAX_NUM_PREC_DEC)
            posStr = "-" + axisSettings.constantMultiplierStr;
        else if (fabs(multiplier - 1.0) <= MAX_NUM_PREC_DEC)
            posStr = axisSettings.constantMultiplierStr;
        else
        {
            posStr.setNum(multiplier, 'g', axisSettings.maxDigitsNum+1);
            if(axisSettings.constantMultiplierStr[0].isDigit()) posStr += "×";
            posStr += axisSettings.constantMultiplierStr;
        }
    }
    else posStr.setNum(multiplier, 'g', axisSettings.maxDigitsNum+1);

    return posStr;
}

ZeLinAxisTicks GridCalculator::getLinearAxisTicks(double windowWidth,
                                            const ZeAxisRange &range,
                                            ZeAxisName axisName,
                                            const QFontMetrics &metrics)
{

    // qDebug() << "New tick spacing calculation";

    const ZeLinAxisSettings &axisSettings = axisName == ZeAxisName::X ? information.getAxesSettings().x.linSettings : information.getAxesSettings().y.linSettings;

    double pxPerUnit = windowWidth / range.amplitude();

    ZeLinAxisTicks axisTicks;

    const double &constantMultiplier = axisSettings.constantMultiplier;

    ZeAxisRange scaledOffsetRange(range), offsetRange(range);
    scaledOffsetRange /= constantMultiplier;
    offsetRange /= constantMultiplier;

    double baseStep;
    double realStep;
    double baseMultiplier = 1;

    axisTicks.offset.sumOffset = 0;
    axisTicks.offset.sumPowerOffset = 0;
    axisTicks.offset.basePowerOffset = 0;

    bool goodSpacing = false;

    bool previously_increased = false;

    // TODO: start from biggest step, which is lround(log10(scaledRange.amplitude())) then go down and down till
    // there's no more space for writing numbers

    int amplitudePower = lround(log10(scaledOffsetRange.amplitude()))+1;
    int rangeEdgePower = lround(log10(max(fabs(scaledOffsetRange.min), fabs(scaledOffsetRange.max))));

    // if the range goes from 5434645.345434 to 5434645.345436
    // these digits:          ^^^^^^^^^^^^^     ^^^^^^^^^^^^^
    // need to be offset

    int power_diff = rangeEdgePower - amplitudePower;
    if(power_diff >= axisSettings.maxDigitsNum - 2)
    {
        axisTicks.offset.sumOffset = trunc(scaledOffsetRange.min * int_pow(10.0, -amplitudePower)) / int_pow(10.0, -amplitudePower);
        offsetRange -= axisTicks.offset.sumOffset;

        int sumOffsetPow = lround(log10(axisTicks.offset.sumOffset));
        if(-sumOffsetPow >= axisSettings.maxDigitsNum)
            axisTicks.offset.sumPowerOffset = sumOffsetPow;
    }

    scaledOffsetRange = offsetRange;
    int targetPower = lround(log10(scaledOffsetRange.amplitude() / targetTicksNum));

    while(not goodSpacing)
    {
        if(abs(targetPower) >= axisSettings.maxDigitsNum)
        {
            scaledOffsetRange = offsetRange;
            scaledOffsetRange *= int_pow(10.0, -targetPower);
            pxPerUnit = windowWidth / scaledOffsetRange.amplitude();

            axisTicks.offset.basePowerOffset = targetPower;
            targetPower = 0;
        }

        baseStep = int_pow(10.0, targetPower);
        realStep = baseStep * baseMultiplier;

        int maxStrPxSize = getMaxStrPxSize(axisName, scaledOffsetRange, realStep, metrics);

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

    int relTickSpacing = axisName == ZeAxisName::X ? information.getAxesSettings().x.tickRelSpacing : information.getAxesSettings().y.tickRelSpacing;

    if(relTickSpacing > 0)
    {
        for(int i=0; i < relTickSpacing; i++)
            base10Inc(targetPower, baseMultiplier);
        baseStep = int_pow(10.0, targetPower);
        realStep = baseStep * baseMultiplier;
    }

    fesetround(FE_DOWNWARD);
    // int maxRangePower = max(lrint(log10(fabs(scaledOffsetRange.max))), lrint(log10(fabs(scaledOffsetRange.min))));

    QString axisNameStr = axisName == ZeAxisName::X ? "x axis" : "y axis";
    // qDebug() << axisNameStr << " Max range power " << maxRangePower;
    // qDebug() << axisNameStr << " step power " << targetPower;


    double multiplier = (floor(scaledOffsetRange.min / realStep) - 1) * realStep ;
    double power_offset = int_pow(10.0, axisTicks.offset.basePowerOffset);

    // TODO: offset is not good, figure it out

    do
    {
        ZeLinAxisTick tick;
        multiplier += realStep;
        tick.pos = multiplier * constantMultiplier * power_offset + axisTicks.offset.sumOffset;
        tick.posStr = get_coordinate_string(axisName, multiplier);

        axisTicks.ticks << tick;

    }while(multiplier < scaledOffsetRange.max);

    return axisTicks;
}
