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
#include <QMetaType>
#include <QObject>
#include <QString>
#include <QtQml/qqmlregistration.h>

#include "MathObjects/expr.h"
#include "Utils/themedcolor.h"

struct ZeLogAxisSettings
{
  Q_GADGET

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

  Q_PROPERTY(zg::mathobj::Expr* constantMultiplier MEMBER constantMultiplier)
  Q_PROPERTY(int maxDigitsNum MEMBER maxDigitsNum)

public:

  /// @brief arbitrary expression that will be used a multiplier to the coordinates
  /// @example if it's "π", then the coordinates will be multiples of π
  /// @note non-owning pointer: needs to be instantiated and owned in QML then assigned here
  zg::mathobj::Expr* constantMultiplier = nullptr;

  int maxDigitsNum = 3;

  bool operator == (const ZeLinAxisSettings &other) const = default;
};

struct ZeAxisSettings
{
  Q_GADGET

  Q_PROPERTY(ZeLinAxisSettings linear MEMBER linSettings)
  Q_PROPERTY(ZeLogAxisSettings log MEMBER logSettings)
  Q_PROPERTY(ViewType axisType MEMBER axisType)
  Q_PROPERTY(int tickSpacing MEMBER tickRelSpacing)

public:
  enum ViewType
  {
      LINEAR, LOG
  };

  int tickRelSpacing = 0;
  ZeLinAxisSettings linSettings;
  ZeLogAxisSettings logSettings;
  ViewType axisType = LINEAR;

  bool operator == (const ZeAxisSettings &other) const = default;
};

struct ZeAxesSettings
{
  Q_GADGET

  Q_PROPERTY(ZeAxisSettings x MEMBER x)
  Q_PROPERTY(ZeAxisSettings y MEMBER y)
  Q_PROPERTY(ThemedColor color MEMBER color)
  Q_PROPERTY(double lineWidth MEMBER lineWidth)

public:
  ZeAxisSettings x, y;
  bool orthonormal = false;

  ThemedColor color = {.dark = "#dfdfdf", .light = Qt::black};

  double lineWidth = 2.0;

  bool operator == (const ZeAxesSettings &other) const = default;
};
