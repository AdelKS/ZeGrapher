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

#pragma once

#include "zecalculator/parsing/types.h"
#include <glaze/yaml.hpp>

#include <QMetaType>
#include <QObject>
#include <QPointF>
#include <QSizeF>
#include <QtQmlIntegration/qqmlintegration.h>

inline constexpr double cmPerInch = 2.54;

/// @brief type of math objects to use in ZeCalculator
inline constexpr zc::parsing::Type zc_t = zc::parsing::Type::RPN;

enum struct ZeAxisName {X, Y};


struct ZeSizeSettings
{
  Q_GADGET
  QML_VALUE_TYPE(zeSizeSettings)

  Q_PROPERTY(bool sheetFillsWindow MEMBER sheetFillsWindow)
  Q_PROPERTY(double scalingFactor MEMBER scalingFactor)
  Q_PROPERTY(QSizeF cmSheetSize MEMBER cmSheetSize)


public:

  bool sheetFillsWindow = true;

  double scalingFactor = 1.0;

  QSizeF cmSheetSize;

  bool operator == (const ZeSizeSettings &other) const = default;

  struct POD
  {
    std::optional<double> scaling;
    std::optional<double> width;
    std::optional<double> height;

    operator bool () const { return scaling or width or height; }
  };

  std::optional<POD> exportPod() const;
  void importPod(POD);

  static constexpr double minScaling = 0.1, maxScaling = 5;
  static constexpr double minCm = 1, maxCm = 1000;
};

struct ZoomingType: QObject
{
  Q_OBJECT
  QML_ELEMENT

public:
  ZoomingType(QObject *parent = nullptr): QObject(parent) {}

  enum Type {FITSHEET, CUSTOM};
  Q_ENUM(Type)
};

struct ZeZoomSettings
{
  Q_GADGET
  Q_PROPERTY(ZoomingType::Type zoomingType MEMBER zoomingType)
  Q_PROPERTY(double zoom MEMBER zoom)
  QML_VALUE_TYPE(zeZoomSettings)

public:

  ZoomingType::Type zoomingType = ZoomingType::FITSHEET;
  double zoom = 1.0;

  static constexpr double minZoom = 0.05, maxZoom = 10;

  bool operator == (const ZeZoomSettings &other) const = default;
};

struct Range
{
    Range() : start(0), end(0), step(1) {}
    double start, end, step;
};

struct Point
{
    double x, y;

    bool operator<(const Point &b) const
    {
        return x < b.x;
    }

    operator QPointF() const
    {
        return QPointF(x, y);
    }
};

bool ptCompY(const Point &pt1, const Point &pt2);

template<class Number>
Number int_pow(Number a, int b)
{
    Number res = 1;
    if(b < 0)
    {
        for(int i = 0 ; i > b ; i--)
            res /= a;
    }
    else
    {
        for(int i = 0 ; i < b ; i++)
            res *= a;
    }

    return res;
}

Point operator * (const Point &pt1, const Point &pt2);
