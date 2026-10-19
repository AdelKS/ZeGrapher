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
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#pragma once

#include <QColor>
#include <QFont>
#include <QMetaType>
#include <QObject>
#include <QString>
#include <QtQmlIntegration/qqmlintegration.h>

#include "Utils/fonts.h"
#include "Utils/themedcolor.h"
#include "Utils/unit.h"
#include "Utils/yaml.h"

struct ZeLogAxisSettings
{
  Q_GADGET
  QML_VALUE_TYPE(zeLogAxisSettings)

  Q_PROPERTY(double base MEMBER base)
  Q_PROPERTY(QString baseStr MEMBER baseStr)

public:

  double base = 10;
  QString baseStr = "10";

  bool operator == (const ZeLogAxisSettings &other) const = default;
};

struct ZeLinAxisSettings
{
  Q_GADGET
  QML_VALUE_TYPE(zeLinAxisSettings)

  Q_PROPERTY(int maxDigitsNum MEMBER maxDigitsNum)

public:
  static constexpr int defaultDigits = 3;

  int maxDigitsNum = defaultDigits;

  bool operator == (const ZeLinAxisSettings &other) const = default;
};

struct ZeAxisSettings
{
  Q_GADGET
  QML_VALUE_TYPE(zeAxisSettings)

  Q_PROPERTY(ZeLinAxisSettings linear MEMBER linSettings)
  Q_PROPERTY(ZeLogAxisSettings log MEMBER logSettings)
  Q_PROPERTY(ViewType axisType MEMBER axisType)
  Q_PROPERTY(int tickSpacing MEMBER tickRelSpacing)
  Q_PROPERTY(QString title MEMBER title)

public:
  enum ViewType
  {
    LINEAR, LOG
  };

  int tickRelSpacing = 0;
  ZeLinAxisSettings linSettings;
  ZeLogAxisSettings logSettings;
  ViewType axisType = LINEAR;
  QString title;

  struct POD {
    std::optional<std::string> title;
    std::optional<int> tick_spacing;
    std::optional<int> max_digits;
    /// @brief the expression of ZeGraphSettings::xMultiplier or yMultiplier,
    ///        which ZeGraphSettings reads
    std::optional<std::string> multiplier;
  };

  static constexpr int maxTickSpacing = 5;
  static constexpr int minDigits = 2, maxDigits = 10;

  /// @param multiplier  the axis multiplier's expression
  std::optional<POD> exportPod(const QString& multiplier) const;
  void importPod(POD);

  bool operator == (const ZeAxisSettings &other) const = default;
};

struct ZeAxesSettings
{
  Q_GADGET
  QML_VALUE_TYPE(zeAxesSettings)

  Q_PROPERTY(ZeAxisSettings x MEMBER x)
  Q_PROPERTY(ZeAxisSettings y MEMBER y)
  Q_PROPERTY(ThemedColor color MEMBER color)
  Q_PROPERTY(double lineWidth MEMBER lineWidth)
  Q_PROPERTY(QFont titleFont MEMBER titleFont)

public:
  ZeAxisSettings x, y;

  ThemedColor color = defaultColor;
  double lineWidth = defaultLineWidth;

  QFont titleFont = zg::mathFont();

  ///@brief spacing between different things in the graph
  ///       currently: between the axis and the coordinates
  ///                  between the coordinates and the axis titles
  static constexpr zg::pixel_unit spacing = {6};

  static const ThemedColor defaultColor;
  static constexpr double defaultLineWidth = 2.0;
  static constexpr double minLineWidth = 0.1, maxLineWidth = 5;

  bool operator == (const ZeAxesSettings &other) const = default;

  struct POD {
    std::optional<ThemedColor::POD> color;
    std::optional<double> line_width;
    std::optional<ZeAxisSettings::POD> x, y;
    std::optional<zg::yml::QFontPOD> title_font;
  };

  std::optional<POD> exportPod(const QString& xMultiplier, const QString& yMultiplier) const;
  void importPod(POD);
};

inline const ThemedColor ZeAxesSettings::defaultColor = {.dark = "#dfdfdf", .light = Qt::black};
