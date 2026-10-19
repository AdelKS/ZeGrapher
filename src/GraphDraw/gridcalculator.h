#pragma once

/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
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

#include <QRectF>

#include "axismapper.h"
#include "mathtext.h"
#include "structures.h"
#include "Utils/graphsettings.h"

struct ZeLogAxisTick
{
  double pos;
  QString baseStr, globalConstantStr;
  double base, globalConstant;
  long multiplier, subMultiplier, powerNumerator, powerDenominator;
};

struct ZeLinAxisTick
{
  zg::real_unit pos;
  MathText text;
};

struct ZeOffset
{
  MathText sumOffsetText() const
  {
    if (sumOffset == 0)
      return {};

    const double offset = std::abs(sumOffset);

    MathText text = sumPowerOffset == 0
                      ? formatNumber(offset, 11)
                      : power(formatNumber(offset * int_pow(10.0, -sumPowerOffset), 11).base + "×10",
                              sumPowerOffset);

    text.base.prepend(sumOffset < 0 ? QString(" ") + minusSign + ' ' : " + ");

    if (not multiplierStr.isEmpty())
      text.tail += (multiplierStr[0].isDigit() ? "×" : "") + multiplierStr;

    return text;
  }

  MathText basePowerOffsetText() const
  {
    if (basePowerOffset == 0)
      return {};

    return power("×10", basePowerOffset);
  }

  /// @brief 'base' followed by 'exponent' as a raised number
  static MathText power(const QString& base, int exponent)
  {
    return MathText{.base = base, .exponent = QString::number(exponent).replace('-', minusSign)};
  }

  /// @brief in multiples of 'multiplierStr' when it is not empty
  double sumOffset = 0;
  int sumPowerOffset = 0, basePowerOffset = 0;

  /// @brief the expression that the ticks are multiples of, empty when they
  ///        are plain numbers
  QString multiplierStr;
};

struct ZeLogAxisTicks
{
  QList<ZeLogAxisTick> ticks;
};

struct ZeLinAxisTicks
{
  ZeOffset offset;
  double maxPxWidth = 0;
  double maxPxAscent = 0, maxPxDescent = 0;
  double maxPxHeight() const { return maxPxAscent + maxPxDescent; }
  std::vector<ZeLinAxisTick> ticks;
};

class GridCalculator
{
public:
  explicit GridCalculator(ZeGraphSettings& settings): settings(settings) {}

  template <ZeAxisName axis>
  ZeLinAxisTicks getLinearAxisTicks(const zg::ZeAxisMapper<axis> &mapper,
                                    const MathTextMetrics &metrics);

protected:
  template <ZeAxisName axis>
  double getMaxStrPxSize(const zg::real_range1d &range, double realStep,
                         const QString& multiplierStr, const MathTextMetrics &metrics);

  /// @param multiplierStr  the expression that the ticks are multiples of, empty
  ///                       when they are plain numbers
  static MathText coordinateText(const ZeLinAxisSettings &axisSettings,
                                 const QString& multiplierStr, double multiplier);

  /// @brief how close to 1 a multiple must be to show as the multiplier alone
  static constexpr double unitTolerance = 1e-8;

  static constexpr double targetTicksNum = 20;
  static constexpr int minDistBetweenCoordinates = 5;
  ZeGraphSettings& settings;
};

template <ZeAxisName axis>
double GridCalculator::getMaxStrPxSize(const zg::real_range1d &scaled_range,
                                       double realStep,
                                       const QString& multiplierStr,
                                       const MathTextMetrics &metrics)
{
  const ZeAxesSettings& axesSettings = settings.getAxes();

  // get the next tick value _within_ the range
  double multiplier = std::ceil(scaled_range.min.v / realStep) * realStep;
  double maxStrPxSize = 0, currentStrPxSize = 0;
  MathText text;

  while (multiplier < scaled_range.max.v)
  {
    if constexpr (axis == ZeAxisName::X)
    {
      text = coordinateText(axesSettings.x.linSettings, multiplierStr, multiplier);
      currentStrPxSize = metrics.boundingRect(text).width();
    }
    else
    {
      text = coordinateText(axesSettings.y.linSettings, multiplierStr, multiplier);
      currentStrPxSize = metrics.boundingRect(text).height();
    }

    if (currentStrPxSize > maxStrPxSize)
      maxStrPxSize = currentStrPxSize;

    multiplier += realStep;
  }

  return maxStrPxSize;
}

template <ZeAxisName axis>
ZeLinAxisTicks GridCalculator::getLinearAxisTicks(const zg::ZeAxisMapper<axis> &axis_mapper,
                                                  const MathTextMetrics &metrics)
{
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

  zg::real_range1d range = axis_mapper.template getRange<zg::real>();

  const double windowWidth = axis_mapper.template getRange<zg::pixel>().amplitude().v;

  ZeLinAxisTicks axisTicks;

  // the spacing search below never ends on an axis without pixels, such as in
  // a figure smaller than its margins
  if (not (windowWidth > 0))
    return axisTicks;

  const ZeAxesSettings& axesSettings = settings.getAxes();
  const ZeLinAxisSettings &axisSettings = axis == ZeAxisName::X
                                            ? axesSettings.x.linSettings
                                            : axesSettings.y.linSettings;

  // a multiplier without a value leaves the ticks on plain numbers, and so does
  // one that doesn't leave a valid range, such as zero or a negative number
  const double multiplierValue = settings.multiplier<axis>().getValue();
  zg::real_range1d scaledOffsetRange(range);
  scaledOffsetRange /= multiplierValue;

  const bool multiplied = scaledOffsetRange.is_valid();
  const QString multiplierStr = multiplied ? settings.multiplier<axis>().getExpression() : QString();
  const double constantMultiplier = multiplied ? multiplierValue : 1.0;
  if (not multiplied)
    scaledOffsetRange = range;

  double baseStep;
  double realStep;
  double baseMultiplier = 1;

  axisTicks.offset.sumOffset = 0;
  axisTicks.offset.multiplierStr = multiplierStr;
  axisTicks.offset.sumPowerOffset = 0;
  axisTicks.offset.basePowerOffset = 0;

  int amplitudePower = lround(log10(scaledOffsetRange.amplitude().v)) + 1;
  int rangeEdgePower = lround(
    log10(std::max(fabs(scaledOffsetRange.min.v), fabs(scaledOffsetRange.max.v))));

  // if the range goes from 5434645.345434 to 5434645.345436
  // these digits:          ^^^^^^^^^^^^^     ^^^^^^^^^^^^^
  // need to be offset

  int power_diff = rangeEdgePower - amplitudePower;
  if (power_diff >= axisSettings.maxDigitsNum - 2)
  {
    axisTicks.offset.sumOffset = trunc(scaledOffsetRange.min.v * int_pow(10.0, -amplitudePower))
                                 / int_pow(10.0, -amplitudePower);
    scaledOffsetRange -= zg::real_unit{axisTicks.offset.sumOffset};

    int sumOffsetPow = lround(log10(axisTicks.offset.sumOffset));
    if (-sumOffsetPow >= axisSettings.maxDigitsNum)
      axisTicks.offset.sumPowerOffset = sumOffsetPow;
  }

  int targetPower = lround(log10(scaledOffsetRange.amplitude().v / targetTicksNum));

  bool goodSpacing = false;
  bool previously_increased = false;
  double pxPerUnit = windowWidth / scaledOffsetRange.amplitude().v;

  while (not goodSpacing)
  {
    if (abs(targetPower) >= axisSettings.maxDigitsNum)
    {
      scaledOffsetRange *= int_pow(10.0, -targetPower);
      pxPerUnit = windowWidth / scaledOffsetRange.amplitude().v;

      axisTicks.offset.basePowerOffset += targetPower;
      targetPower = 0;
      previously_increased = false;
    }

    baseStep = int_pow(10.0, targetPower);
    realStep = baseStep * baseMultiplier;

    double maxStrPxSize = getMaxStrPxSize<axis>(scaledOffsetRange, realStep, multiplierStr, metrics);

    if (maxStrPxSize + minDistBetweenCoordinates >= realStep * pxPerUnit)
    {
      base10Inc(targetPower, baseMultiplier);
      previously_increased = true;
    }
    else if (!previously_increased
             and maxStrPxSize + minDistBetweenCoordinates <= baseStep * previousMultiplier(baseMultiplier) * pxPerUnit)
    {
      base10Dec(targetPower, baseMultiplier);
      previously_increased = false;
    }
    else
      goodSpacing = true;
  }

  int relTickSpacing = axis == ZeAxisName::X ? axesSettings.x.tickRelSpacing
                                             : axesSettings.y.tickRelSpacing;

  if (relTickSpacing > 0)
  {
    for (int i = 0; i < relTickSpacing; i++) base10Inc(targetPower, baseMultiplier);
    baseStep = int_pow(10.0, targetPower);
    realStep = baseStep * baseMultiplier;
  }

  double multiplier = (floor(scaledOffsetRange.min.v / realStep) - 1) * realStep;
  double power_offset = int_pow(10.0, axisTicks.offset.basePowerOffset);

  const zg::pixel_unit zero_pt = axis_mapper.template to<zg::pixel>(zg::real_unit{0.});

  auto getPos = [&]
  {
    return zg::real_unit{(multiplier * power_offset + axisTicks.offset.sumOffset)
                         * constantMultiplier};
  };

  do
  {
    ZeLinAxisTick tick;
    multiplier += realStep;
    zg::pixel_unit px_pos = axis_mapper.template to<zg::pixel>(getPos());
    if (fabs(px_pos.v - zero_pt.v) <= 1.)
      multiplier = 0;
    tick.pos = getPos();
    tick.text = coordinateText(axisSettings, multiplierStr, multiplier);

    // only ticks within the range get drawn, take only those into consideration
    if (scaledOffsetRange.min.v < multiplier and multiplier < scaledOffsetRange.max.v)
    {
      const QRectF ink = metrics.tightBoundingRect(tick.text);
      axisTicks.maxPxAscent = std::max(axisTicks.maxPxAscent, -ink.top());
      axisTicks.maxPxDescent = std::max(axisTicks.maxPxDescent, ink.bottom());
      axisTicks.maxPxWidth = std::max(axisTicks.maxPxWidth, metrics.horizontalAdvance(tick.text));
    }

    axisTicks.ticks.push_back(tick);
  }
  while (multiplier < scaledOffsetRange.max.v);

  return axisTicks;
}
