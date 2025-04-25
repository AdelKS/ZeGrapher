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

#include <QColor>
#include <QObject>
#include <QtQmlIntegration>

namespace zg {

/// @brief Contains the information needed to compute the math object and how to plot it
struct PlotStyle: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(bool visible MEMBER visible NOTIFY visibleChanged)
  Q_PROPERTY(QColor color MEMBER color NOTIFY colorChanged)
  Q_PROPERTY(double lineWidth MEMBER lineWidth NOTIFY lineWidthChanged)
  Q_PROPERTY(Qt::PenStyle lineStyle MEMBER lineStyle NOTIFY lineStyleChanged)
  Q_PROPERTY(PointStyle pointStyle MEMBER pointStyle NOTIFY pointStyleChanged)
  Q_PROPERTY(double pointWidth MEMBER pointWidth NOTIFY pointWidthChanged)

public:

  enum PointStyle : int { None, Rhombus, Disc, Square, Triangle, Cross };
  Q_ENUM(PointStyle)

  enum CoordinateSystem: int {Cartesian, Polar};
  Q_ENUM(CoordinateSystem);

  enum ObjectType: int {Continuous, Discrete};
  Q_ENUM(ObjectType);

  explicit PlotStyle(QObject *parent = nullptr);

  bool visible = true;
  QColor color = Qt::black;
  double lineWidth = 1.0;
  Qt::PenStyle lineStyle = Qt::SolidLine;
  double pointWidth = 1.0;
  PointStyle pointStyle = None;
  CoordinateSystem coordinates = Cartesian;
  ObjectType type = Continuous;
  double cartesianStart = 0;
  double cartesianEnd = 10;
  double cartesianStep = 1;
  double polarStart = 0;
  double polarEnd = 2*std::numbers::pi;
  double polarStep = std::numbers::pi/6.;

signals:
  void updated();
  void visibleChanged();
  void colorChanged();
  void lineWidthChanged();
  void pointStyleChanged();
  void pointWidthChanged();
  void lineStyleChanged();
  void backendChanged();
};

}
