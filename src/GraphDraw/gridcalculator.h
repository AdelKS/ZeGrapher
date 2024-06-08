#ifndef GRAPHGRIDCALCULATOR_H
#define GRAPHGRIDCALCULATOR_H

#include <QObject>

#include "structures.h"

#define TARGET_TICKS_NUM 20

struct ZeLogAxisTick
{
    double pos;
    QString baseStr, globalConstantStr;
    double base, globalConstant;
    long multiplier, subMultiplier, powerNumerator, powerDenominator;
};

struct ZeLinAxisTick
{
    double pos;
    QString posStr;
};

struct ZeAxisSubTick
{
    double pos;
    int numerator, denominator;
};

struct ZeOffset
{
    ZeOffset(): sumOffset(0), sumPowerOffset(0), basePowerOffset(0) {}
    double sumOffset;
    int sumPowerOffset, basePowerOffset;
};

struct ZeLogAxisTicks
{
    QList<ZeLogAxisTick> ticks;
    QList<ZeAxisSubTick> axisSubticks;
};

struct ZeLinAxisTicks
{
    ZeOffset offset;
    std::vector<ZeLinAxisTick> ticks;
    std::vector<ZeAxisSubTick> axisSubticks;
};

class GridCalculator : public QObject
{
    Q_OBJECT
public:
    explicit GridCalculator(QObject *parent = nullptr);

    ZeLinAxisTicks getLinearAxisTicks(double pxWidth,
                                   const ZeAxisRange &range,
                                   ZeAxisName axisName,
                                   const QFontMetrics &metrics);

protected:
    int getMaxStrPxSize(ZeAxisName axisname, const ZeAxisRange &range, double realStep, const QFontMetrics &metrics);
    QString get_coordinate_string(ZeAxisName axisname, double multiplier);

    double targetTicksNum;

};

#endif // GRAPHGRIDCALCULATOR_H
