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

/// @brief Contains the information needed to compute the math object and how to plot it
struct PlotStyle: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(bool visible MEMBER visible NOTIFY visibleChanged)
  Q_PROPERTY(ThemedColor color MEMBER color NOTIFY colorChanged)
  Q_PROPERTY(ThemedColor secondColor MEMBER secondColor NOTIFY secondColorChanged)
  Q_PROPERTY(double lineWidth MEMBER lineWidth NOTIFY lineWidthChanged)
  Q_PROPERTY(LineStyle lineStyle MEMBER lineStyle NOTIFY lineStyleChanged)
  Q_PROPERTY(PointStyle pointStyle MEMBER pointStyle NOTIFY pointStyleChanged)
  Q_PROPERTY(double pointWidth MEMBER pointWidth NOTIFY pointWidthChanged)

public:

  enum ObjectType { NonRepresentable, Continuous, Discrete };
  Q_ENUM(ObjectType);

  enum PointStyle { None, Rhombus, Disc, Square, Triangle, Cross };
  Q_ENUM(PointStyle);

  enum LineStyle { NoLine, Solid, Dash, DashDot, Dot };
  Q_ENUM(LineStyle);

  explicit PlotStyle(QObject *parent = nullptr);

  void setVisible(bool);

  bool visible = true;
  ThemedColor color = defaultColor;

  /// @brief used for simultaneous plotting
  ThemedColor secondColor = defaultSecondColor;

  /// @brief returns a linear interpolation between color and secondColor in the current theme
  /// @param t: coeff in [0, 1]
  QColor colorLerp(double t) const;

  double lineWidth = defaultLineWidth;
  LineStyle lineStyle = Solid;
  double pointWidth = defaultPointWidth;
  PointStyle pointStyle = None;

  QList<qreal> getDashPattern() const;

  // defaults

  const static ThemedColor defaultColor;
  const static ThemedColor defaultSecondColor;

  static constexpr double defaultLineWidth = 2.0;
  static constexpr double defaultPointWidth = 5.0;

  LineStyle defaultLineStyle() const;
  PointStyle defaultPointStyle() const;

  /// @brief only used for determining defaults
  void setDiscrete(bool d);

  struct POD {
    std::optional<ThemedColor::POD> color;
    std::optional<ThemedColor::POD> second_color;
    std::optional<double> line_width;
    std::optional<LineStyle> line_style;
    std::optional<double> point_width;
    std::optional<PointStyle> point_style;

    operator bool() const
    {
      return color or second_color or line_width or line_style or point_width or point_style;
    }
  };

  std::optional<POD> exportPod() const;


signals:
  void updated();
  void visibleChanged();
  void colorChanged();
  void lineWidthChanged();
  void pointStyleChanged();
  void pointWidthChanged();
  void lineStyleChanged();
  void drawLineChanged();
  void backendChanged();
  void rangeChanged();
  void secondColorChanged();

protected:
  bool discrete = false;

  friend class MathWorld;
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
      "noline",  NoLine,
      "solid",   Solid,
      "dash",    Dash,
      "dot",     Dot,
      "dashdot", DashDot);
};
