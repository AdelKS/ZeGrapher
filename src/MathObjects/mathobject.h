#pragma once

/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
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

#include <QtQmlIntegration>
#include <QObject>

#include "Utils/plotstyle.h"
#include "zc.h"

namespace zg {

/// @brief Contains the information needed to compute the math object and how to plot it
struct MathObject: QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(PlotStyle plotStyle MEMBER plotStyle)

public:

  enum Type {ZC, Constant};
  Q_ENUM(Type)

  explicit MathObject(QObject *parent = nullptr);

  Q_INVOKABLE void setBackend(mathobj::ZC*);

  PlotStyle plotStyle;
  mathobj::ZC* zcBackend = nullptr;

};

}
