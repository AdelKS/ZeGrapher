/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/


#pragma once

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "MathObjects/constant.h"
#include "Utils/axissettings.h"
#include "Utils/graphsettings.h"
#include "Utils/gridsettings.h"
#include "Utils/plotstyle.h"
#include "Utils/sizesettings.h"

namespace zg {

/// @brief the bounds of the settings, for the panel's fields
/// @note documents are clamped to the same bounds when they load
class Limits : public QObject
{
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(double minCurveLineWidth MEMBER minCurveLineWidth CONSTANT)
  Q_PROPERTY(double maxCurveLineWidth MEMBER maxCurveLineWidth CONSTANT)
  Q_PROPERTY(double minPointWidth MEMBER minPointWidth CONSTANT)
  Q_PROPERTY(double maxPointWidth MEMBER maxPointWidth CONSTANT)

  Q_PROPERTY(int minSteps MEMBER minSteps CONSTANT)
  Q_PROPERTY(int maxSteps MEMBER maxSteps CONSTANT)
  Q_PROPERTY(double minDuration MEMBER minDuration CONSTANT)
  Q_PROPERTY(double maxDuration MEMBER maxDuration CONSTANT)

  Q_PROPERTY(double minScaling MEMBER minScaling CONSTANT)
  Q_PROPERTY(double maxScaling MEMBER maxScaling CONSTANT)
  Q_PROPERTY(double minSheetCm MEMBER minSheetCm CONSTANT)
  Q_PROPERTY(double maxSheetCm MEMBER maxSheetCm CONSTANT)
  Q_PROPERTY(double minZoom MEMBER minZoom CONSTANT)
  Q_PROPERTY(double maxZoom MEMBER maxZoom CONSTANT)
  Q_PROPERTY(int lowestPointsLg2 MEMBER lowestPointsLg2 CONSTANT)
  Q_PROPERTY(int highestPointsLg2 MEMBER highestPointsLg2 CONSTANT)

  Q_PROPERTY(double minAxisLineWidth MEMBER minAxisLineWidth CONSTANT)
  Q_PROPERTY(double maxAxisLineWidth MEMBER maxAxisLineWidth CONSTANT)
  Q_PROPERTY(int maxTickSpacing MEMBER maxTickSpacing CONSTANT)
  Q_PROPERTY(int minDigits MEMBER minDigits CONSTANT)
  Q_PROPERTY(int maxDigits MEMBER maxDigits CONSTANT)

  Q_PROPERTY(double minGridLineWidth MEMBER minGridLineWidth CONSTANT)
  Q_PROPERTY(double maxGridLineWidth MEMBER maxGridLineWidth CONSTANT)
  Q_PROPERTY(int minSubdivs MEMBER minSubdivs CONSTANT)
  Q_PROPERTY(int maxSubdivs MEMBER maxSubdivs CONSTANT)

public:
  explicit Limits(QObject* parent = nullptr): QObject(parent) {}

  const double minCurveLineWidth = PlotStyle::minLineWidth;
  const double maxCurveLineWidth = PlotStyle::maxLineWidth;
  const double minPointWidth = PlotStyle::minPointWidth;
  const double maxPointWidth = PlotStyle::maxPointWidth;

  const int minSteps = mathobj::Constant::minSteps;
  const int maxSteps = mathobj::Constant::maxSteps;
  const double minDuration = mathobj::Constant::minDuration;
  const double maxDuration = mathobj::Constant::maxDuration;

  const double minScaling = ZeSizeSettings::minScaling;
  const double maxScaling = ZeSizeSettings::maxScaling;
  const double minSheetCm = ZeSizeSettings::minCm;
  const double maxSheetCm = ZeSizeSettings::maxCm;
  const double minZoom = ZeZoomSettings::minZoom;
  const double maxZoom = ZeZoomSettings::maxZoom;
  const int lowestPointsLg2 = ZeGraphSettings::lowestPointsLg2;
  const int highestPointsLg2 = ZeGraphSettings::highestPointsLg2;

  const double minAxisLineWidth = ZeAxesSettings::minLineWidth;
  const double maxAxisLineWidth = ZeAxesSettings::maxLineWidth;
  const int maxTickSpacing = ZeAxisSettings::maxTickSpacing;
  const int minDigits = ZeAxisSettings::minDigits;
  const int maxDigits = ZeAxisSettings::maxDigits;

  const double minGridLineWidth = Ze1DGridSettings::minLineWidth;
  const double maxGridLineWidth = Ze1DGridSettings::maxLineWidth;
  const int minSubdivs = Ze1DSubgridSettings::minSubdivs;
  const int maxSubdivs = Ze1DSubgridSettings::maxSubdivs;
};

}
