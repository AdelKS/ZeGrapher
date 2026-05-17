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

#include "GraphDraw/axismapper.h"
#include "MathObjects/expr.h"
#include "Utils/themedcolor.h"

namespace zg {

/// @brief Contains the information needed to compute the math object and how to plot it
struct PlotStyle: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(bool visible MEMBER visible NOTIFY visibleChanged)
  Q_PROPERTY(ThemedColor color MEMBER color NOTIFY colorChanged)
  Q_PROPERTY(ThemedColor secondColor MEMBER secondColor NOTIFY secondColorChanged)
  Q_PROPERTY(double lineWidth MEMBER lineWidth NOTIFY lineWidthChanged)
  Q_PROPERTY(QList<qreal> dashPattern MEMBER dashPattern NOTIFY dashPatternChanged)
  Q_PROPERTY(bool drawLine MEMBER drawLine NOTIFY drawLineChanged)
  Q_PROPERTY(PointStyle pointStyle MEMBER pointStyle NOTIFY pointStyleChanged)
  Q_PROPERTY(double pointWidth MEMBER pointWidth NOTIFY pointWidthChanged)

public:

  enum ObjectType { NonRepresentable, Continuous, Discrete};
  Q_ENUM(ObjectType);

  enum PointStyle { None, Rhombus, Disc, Square, Triangle, Cross };
  Q_ENUM(PointStyle);

  explicit PlotStyle(QObject *parent = nullptr);

  zg::real_range1d getRange() const;

  void setVisible(bool);

  bool visible = true;
  ThemedColor color = {.dark = Qt::lightGray, .light = Qt::black};

  /// @brief used for simultaneous plotting
  ThemedColor secondColor = {.dark = "#009999", .light = "#00fefe"};

  /// @brief returns a linear interpolation between color and secondColor in the current theme
  /// @param t: coeff in [0, 1]
  QColor colorLerp(double t) const;

  double lineWidth = 2.0;
  QList<qreal> dashPattern;
  bool drawLine = true;
  double pointWidth = 5.0;
  PointStyle pointStyle = None;

signals:
  void updated();
  void visibleChanged();
  void colorChanged();
  void lineWidthChanged();
  void pointStyleChanged();
  void pointWidthChanged();
  void dashPatternChanged();
  void drawLineChanged();
  void backendChanged();
  void rangeChanged();
  void secondColorChanged();

protected:
  friend class MathWorld;
};

}
