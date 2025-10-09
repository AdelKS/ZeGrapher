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

#include "MathObjects/expr.h"

#include "GraphDraw/axismapper.h"

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
  Q_PROPERTY(CoordinateSystem coordinateSystem MEMBER coordinateSystem NOTIFY coordinateSystemChanged)
  Q_PROPERTY(ObjectType objectType WRITE setObjectType MEMBER objectType NOTIFY objectTypeChanged)
  Q_PROPERTY(mathobj::Expr* start MEMBER start)
  Q_PROPERTY(mathobj::Expr* end MEMBER end)
  Q_PROPERTY(mathobj::Expr* step MEMBER step)

public:

  enum ObjectType { NonRepresentable, Continuous, Discrete};
  Q_ENUM(ObjectType);

  enum PointStyle { None, Rhombus, Disc, Square, Triangle, Cross };
  Q_ENUM(PointStyle);

  enum CoordinateSystem {Cartesian, Polar};
  Q_ENUM(CoordinateSystem);

  explicit PlotStyle(QObject *parent = nullptr);

  mathobj::Expr* start = nullptr;
  mathobj::Expr* end = nullptr;
  mathobj::Expr* step = nullptr;

  zg::real_unit getStart() const;
  zg::real_unit getEnd() const;
  zg::real_unit getStep() const;

  zg::real_range1d getRange() const;

  bool visible = true;
  QColor color = Qt::black;
  double lineWidth = 1.0;
  Qt::PenStyle lineStyle = Qt::SolidLine;
  double pointWidth = 1.0;
  PointStyle pointStyle = None;
  CoordinateSystem coordinateSystem = Cartesian;

  ObjectType objectType = NonRepresentable;

public slots:
  void setObjectType(ObjectType);

signals:
  void updated();
  void visibleChanged();
  void colorChanged();
  void lineWidthChanged();
  void pointStyleChanged();
  void pointWidthChanged();
  void lineStyleChanged();
  void backendChanged();
  void coordinateSystemChanged();
  void rangeChanged();
  void objectTypeChanged();
};

}
