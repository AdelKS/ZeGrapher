#pragma once

/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <QObject>

#include "axismapper.h"
#include "information.h"
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
  ZeOffset() : sumOffset(0), sumPowerOffset(0), basePowerOffset(0) {}
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

  template <ZeAxisName axis>
  ZeLinAxisTicks getLinearAxisTicks(const zg::ZeAxisMapper<axis> &mapper,
                                    const QFontMetrics &metrics);

protected:
  template <ZeAxisName axis>
  int getMaxStrPxSize(const ZeAxisRange &range, double realStep, const QFontMetrics &metrics);

  QString get_coordinate_string(const ZeLinAxisSettings &axisSettings, double multiplier);

  double targetTicksNum;
};

template <ZeAxisName axis>
int GridCalculator::getMaxStrPxSize(const ZeAxisRange &scaled_range,
                                    double realStep,
                                    const QFontMetrics &metrics)
{
  double multiplier = floor(scaled_range.min / realStep) * realStep;
  int maxStrPxSize = 0, currentStrPxSize = 0;
  QString posStr;

  while (multiplier < scaled_range.max)
  {
    if constexpr (axis == ZeAxisName::X)
    {
      posStr = get_coordinate_string(information.getAxesSettings().x.linSettings, multiplier);
      currentStrPxSize = metrics.boundingRect(posStr).width();
    }
    else
    {
      posStr = get_coordinate_string(information.getAxesSettings().y.linSettings, multiplier);
      currentStrPxSize = metrics.boundingRect(posStr).height();
    }

    if (currentStrPxSize > maxStrPxSize)
      maxStrPxSize = currentStrPxSize;

    multiplier += realStep;
  }

  return maxStrPxSize;
}

template <ZeAxisName axis>
ZeLinAxisTicks GridCalculator::getLinearAxisTicks(const zg::ZeAxisMapper<axis> &axis_mapper,
                                                  const QFontMetrics &metrics)
{
  // qDebug() << "New tick spacing calculation";

  auto previousMultiplier = [](double baseMultiplier)
  {
    if (baseMultiplier == 2)
      return 2.;
    else
      return 2.5;
  };

  auto base10Inc = [](int &targetPower, double &baseMultiplier)
  {
    if (baseMultiplier == 1)
      baseMultiplier = 2;
    else if (baseMultiplier == 2)
      baseMultiplier = 5;
    else if (baseMultiplier == 5)
    {
      baseMultiplier = 1;
      targetPower++;
    }
  };

  auto base10Dec = [](int &targetPower, double &baseMultiplier)
  {
    if (baseMultiplier == 1)
    {
      targetPower--;
      baseMultiplier = 5;
    }
    else if (baseMultiplier == 2)
      baseMultiplier = 1;
    else if (baseMultiplier == 5)
      baseMultiplier = 2;
  };

  ZeAxisRange range{.min = axis_mapper.template getMin<zg::real>().v,
                    .max = axis_mapper.template getMax<zg::real>().v};

  const double windowWidth = axis_mapper.template getRange<zg::pixel>().amplitude().v;

  double pxPerUnit = windowWidth / range.amplitude();

  ZeLinAxisTicks axisTicks;

  const ZeLinAxisSettings &axisSettings = axis == ZeAxisName::X
                                            ? information.getAxesSettings().x.linSettings
                                            : information.getAxesSettings().y.linSettings;

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

  int amplitudePower = lround(log10(scaledOffsetRange.amplitude())) + 1;
  int rangeEdgePower = lround(
    log10(std::max(fabs(scaledOffsetRange.min), fabs(scaledOffsetRange.max))));

  // if the range goes from 5434645.345434 to 5434645.345436
  // these digits:          ^^^^^^^^^^^^^     ^^^^^^^^^^^^^
  // need to be offset

  int power_diff = rangeEdgePower - amplitudePower;
  if (power_diff >= axisSettings.maxDigitsNum - 2)
  {
    axisTicks.offset.sumOffset = trunc(scaledOffsetRange.min * int_pow(10.0, -amplitudePower))
                                 / int_pow(10.0, -amplitudePower);
    offsetRange -= axisTicks.offset.sumOffset;

    int sumOffsetPow = lround(log10(axisTicks.offset.sumOffset));
    if (-sumOffsetPow >= axisSettings.maxDigitsNum)
      axisTicks.offset.sumPowerOffset = sumOffsetPow;
  }

  scaledOffsetRange = offsetRange;
  int targetPower = lround(log10(scaledOffsetRange.amplitude() / targetTicksNum));

  while (not goodSpacing)
  {
    if (abs(targetPower) >= axisSettings.maxDigitsNum)
    {
      scaledOffsetRange = offsetRange;
      scaledOffsetRange *= int_pow(10.0, -targetPower);
      pxPerUnit = windowWidth / scaledOffsetRange.amplitude();

      axisTicks.offset.basePowerOffset = targetPower;
      targetPower = 0;
    }

    baseStep = int_pow(10.0, targetPower);
    realStep = baseStep * baseMultiplier;

    int maxStrPxSize = getMaxStrPxSize<axis>(scaledOffsetRange, realStep, metrics);

    if (double(maxStrPxSize) - constantMultiplier * realStep * pxPerUnit > 0)
    {
      base10Inc(targetPower, baseMultiplier);
      previously_increased = true;
    }
    else if (realStep * pxPerUnit * constantMultiplier / previousMultiplier(baseMultiplier)
                 - double(maxStrPxSize)
               > 0
             && !previously_increased)
    {
      base10Dec(targetPower, baseMultiplier);
      previously_increased = false;
    }
    else
      goodSpacing = true;
  }

  int relTickSpacing = axis == ZeAxisName::X ? information.getAxesSettings().x.tickRelSpacing
                                             : information.getAxesSettings().y.tickRelSpacing;

  if (relTickSpacing > 0)
  {
    for (int i = 0; i < relTickSpacing; i++) base10Inc(targetPower, baseMultiplier);
    baseStep = int_pow(10.0, targetPower);
    realStep = baseStep * baseMultiplier;
  }

  // int maxRangePower = max(lrint(log10(fabs(scaledOffsetRange.max))), lrint(log10(fabs(scaledOffsetRange.min))));

  // QStringView axisNameStr = axis == ZeAxisName::X ? "x axis" : "y axis";
  // qDebug() << axisNameStr << " Max range power " << maxRangePower;
  // qDebug() << axisNameStr << " step power " << targetPower;

  double multiplier = (floor(scaledOffsetRange.min / realStep) - 1) * realStep;
  double power_offset = int_pow(10.0, axisTicks.offset.basePowerOffset);

  // TODO: offset is not good, figure it out

  do
  {
    ZeLinAxisTick tick;
    multiplier += realStep;
    tick.pos = multiplier * constantMultiplier * power_offset + axisTicks.offset.sumOffset;
    tick.posStr = get_coordinate_string(axisSettings, multiplier);

    axisTicks.ticks.push_back(tick);

  } while (multiplier < scaledOffsetRange.max);

  return axisTicks;
}
