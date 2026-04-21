#pragma once

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

#include "Utils/themedcolor.h"

#include <QMetaType>
#include <QPen>
#include <QtQmlIntegration/qqmlintegration.h>

namespace zg {

/// @brief Style info relevant to drawing a sampled curve
struct CurveStyle {
  Q_GADGET
  QML_VALUE_TYPE(curveStyle)

  Q_PROPERTY(bool visible MEMBER visible)
  Q_PROPERTY(QColor color MEMBER color)
  Q_PROPERTY(double lineWidth MEMBER lineWidth)
  Q_PROPERTY(QList<qreal> dashPattern MEMBER dashPattern)
  Q_PROPERTY(bool drawLine MEMBER drawLine)
  Q_PROPERTY(double pointWidth MEMBER pointWidth)
  Q_PROPERTY(PointStyle pointStyle MEMBER pointStyle)

public:
  enum PointStyle { None, Rhombus, Disc, Square, Triangle, Cross };
  Q_ENUM(PointStyle)

  bool visible = true;
  QColor color = Qt::black;
  double lineWidth = 1.0;
  QList<qreal> dashPattern;
  bool drawLine = true;
  double pointWidth = 1.0;
  PointStyle pointStyle = None;
};

}
