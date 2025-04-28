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
  Q_PROPERTY(bool continuous MEMBER continuous NOTIFY continuousChanged)
  Q_PROPERTY(double start WRITE setStart READ getStart NOTIFY rangeChanged)
  Q_PROPERTY(double end WRITE setEnd READ getEnd NOTIFY rangeChanged)
  Q_PROPERTY(double step WRITE setStep READ getStep NOTIFY rangeChanged)

public:

  enum PointStyle : int { None, Rhombus, Disc, Square, Triangle, Cross };
  Q_ENUM(PointStyle)

  enum CoordinateSystem: int {Cartesian, Polar};
  Q_ENUM(CoordinateSystem);

  explicit PlotStyle(QObject *parent = nullptr);

  Q_INVOKABLE double getStart();
  Q_INVOKABLE double getEnd();
  Q_INVOKABLE double getStep();

  bool visible = true;
  QColor color = Qt::black;
  double lineWidth = 1.0;
  Qt::PenStyle lineStyle = Qt::SolidLine;
  double pointWidth = 1.0;
  PointStyle pointStyle = None;
  CoordinateSystem coordinateSystem = Cartesian;
  bool continuous = true;
  zg::real_range1d range;
  zg::real_unit step = {1.};

public slots:
  void setStart(double);
  void setEnd(double);
  void setStep(double);

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
  void continuousChanged();
  void rangeChanged();
};

}
