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

#include <QString>
#include <QColor>
#include <QMetaType>
#include <QtQml/qqmlregistration.h>

struct ZeLogAxisSettings
{
  Q_GADGET

  Q_PROPERTY(double constantMultiplier MEMBER constantMultiplier)
  Q_PROPERTY(double base MEMBER base)
  Q_PROPERTY(QString constantMultiplierStr MEMBER constantMultiplierStr)
  Q_PROPERTY(QString baseStr MEMBER baseStr)

public:
  ZeLogAxisSettings() = default;

  double constantMultiplier = 1, base = 10;
  QString constantMultiplierStr, baseStr = "10";

  bool operator == (const ZeLogAxisSettings &other) const = default;
};

struct ZeLinAxisSettings
{
  Q_GADGET

  Q_PROPERTY(double constantMultiplier MEMBER constantMultiplier)
  Q_PROPERTY(QString constantMultiplierStr MEMBER constantMultiplierStr)
  Q_PROPERTY(int maxDigitsNum MEMBER maxDigitsNum)

public:
    ZeLinAxisSettings() = default;

    bool operator == (const ZeLinAxisSettings &other) const = default;

    double constantMultiplier = 1;
    QString constantMultiplierStr;
    int maxDigitsNum = 3;
};

struct ZeAxisSettings
{
  enum ViewType
  {
      LINEAR, LOG
  };

  Q_GADGET

  Q_PROPERTY(ZeLinAxisSettings linear MEMBER linSettings)
  Q_PROPERTY(ZeLogAxisSettings log MEMBER logSettings)
  Q_PROPERTY(ViewType axisType MEMBER axisType)

public:

  ZeAxisSettings() = default;

  bool operator == (const ZeAxisSettings &other) const = default;
   int tickRelSpacing = 0;
  ZeLinAxisSettings linSettings;
  ZeLogAxisSettings logSettings;
  ViewType axisType = LINEAR;
};

struct ZeAxesSettings
{
  Q_GADGET

  Q_PROPERTY(ZeAxisSettings x MEMBER x)
  Q_PROPERTY(ZeAxisSettings y MEMBER y)
  Q_PROPERTY(QColor color MEMBER color)
  Q_PROPERTY(double lineWidth MEMBER lineWidth)

public:
    ZeAxisSettings x, y;
    bool orthonormal = false;
    QColor color = Qt::black;
    double lineWidth = 2.0;

    bool operator == (const ZeAxesSettings &other) const = default;
};
