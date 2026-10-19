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

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "Utils/themedcolor.h"
#include "Utils/yaml.h"

namespace zg {

/// @brief how a curve is drawn: its colors, line and points
struct PlotStyle: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(bool visible MEMBER visible NOTIFY visibleChanged)
  Q_PROPERTY(ThemedColor color MEMBER color NOTIFY colorChanged)
  Q_PROPERTY(ThemedColor secondColor MEMBER secondColor NOTIFY secondColorChanged)
  Q_PROPERTY(double lineWidth WRITE setLineWidth MEMBER lineWidth NOTIFY lineWidthChanged)
  Q_PROPERTY(LineStyle lineStyle READ getLineStyle WRITE setLineStyle NOTIFY lineStyleChanged)
  Q_PROPERTY(PointStyle pointStyle READ getPointStyle WRITE setPointStyle NOTIFY pointStyleChanged)
  Q_PROPERTY(double pointWidth WRITE setPointWidth MEMBER pointWidth NOTIFY pointWidthChanged)

public:

  enum PointStyle { None, Rhombus, Disc, Square, Triangle, Cross };
  Q_ENUM(PointStyle)

  enum LineStyle { NoLine, Solid, Dash, DashDot, Dot };
  Q_ENUM(LineStyle)

  explicit PlotStyle(QObject *parent = nullptr);

  void setVisible(bool);
  void setLineWidth(double);
  void setLineStyle(LineStyle);
  void setPointStyle(PointStyle);
  void setPointWidth(double);

  bool visible = true;
  ThemedColor color = defaultColor;

  /// @brief used for simultaneous plotting
  ThemedColor secondColor = defaultSecondColor;

  /// @brief returns a linear interpolation between color and secondColor in the current theme
  /// @param t: coeff in [0, 1]
  QColor colorLerp(double t) const;

  double lineWidth = defaultLineWidth;
  double pointWidth = defaultPointWidth;

  /// @brief the style the user picked, or the default of the object's kind
  LineStyle getLineStyle() const { return lineStyle.value_or(defaultLineStyle()); }
  PointStyle getPointStyle() const { return pointStyle.value_or(defaultPointStyle()); }

  QList<qreal> getDashPattern() const;

  // defaults

  const static ThemedColor defaultColor;
  const static ThemedColor defaultSecondColor;

  static constexpr double defaultLineWidth = 2.0;
  static constexpr double defaultPointWidth = 5.0;
  static constexpr double minLineWidth = 0.1, maxLineWidth = 10;
  static constexpr double minPointWidth = 0.25, maxPointWidth = 25;

  LineStyle defaultLineStyle() const;
  PointStyle defaultPointStyle() const;

  /// @brief picks the default line and point styles
  void setDiscrete(bool d);

  struct POD {
    std::optional<bool> visible;
    std::optional<ThemedColor::POD> color;
    std::optional<ThemedColor::POD> second_color;
    std::optional<double> line_width;
    std::optional<LineStyle> line_style;
    std::optional<double> point_width;
    std::optional<PointStyle> point_style;
  };

  std::optional<POD> exportPod() const;
  void importPod(POD);

signals:
  void updated();
  void visibleChanged();
  void colorChanged();
  void lineWidthChanged();
  void pointStyleChanged();
  void pointWidthChanged();
  void lineStyleChanged();
  void secondColorChanged();

protected:
  bool discrete = false;

  /// @brief the styles the user picked, nothing to follow the object's kind
  std::optional<LineStyle> lineStyle;
  std::optional<PointStyle> pointStyle;
};

inline const ThemedColor PlotStyle::defaultColor = {.dark = Qt::lightGray, .light = Qt::black};
inline const ThemedColor PlotStyle::defaultSecondColor = {.dark = "#00fefe", .light = "#009999"};

}

template <>
struct glz::meta<zg::PlotStyle::PointStyle>
{
   using enum zg::PlotStyle::PointStyle;
   static constexpr auto value = glz::enumerate(
      "none",     None,
      "rhombus",  Rhombus,
      "disc",     Disc,
      "square",   Square,
      "triangle", Triangle,
      "cross",    Cross);
};

template <>
struct glz::meta<zg::PlotStyle::LineStyle>
{
   using enum zg::PlotStyle::LineStyle;
   static constexpr auto value = glz::enumerate(
      "no_line",  NoLine,
      "solid",    Solid,
      "dash",     Dash,
      "dot",      Dot,
      "dash_dot", DashDot);
};
