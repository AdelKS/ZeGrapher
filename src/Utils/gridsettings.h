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

#include <QMetaType>
#include <QObject>
#include <QString>
#include <QtQmlIntegration/qqmlintegration.h>

#include "Utils/themedcolor.h"

struct Ze1DGridCommonSettings
{
  Q_GADGET

  Q_PROPERTY(bool show MEMBER show)
  Q_PROPERTY(double lineWidth MEMBER lineWidth)
  Q_PROPERTY(ThemedColor color MEMBER color)

public:
  bool show = defaultShow;
  double lineWidth = defaultLineWidth;
  ThemedColor color = defaultColor;

  static constexpr bool defaultShow = true;
  static constexpr double defaultLineWidth = 0.6;
  static const ThemedColor defaultColor;

  struct POD {
    std::optional<bool> show;
    std::optional<double> line_width;
    std::optional<ThemedColor::POD> color;

    operator bool () const { return show or line_width or color; }
  };

  std::optional<POD> exportPod() const;
  void importPod(POD);

  bool operator == (const Ze1DGridCommonSettings&) const = default;
};

inline const ThemedColor Ze1DGridCommonSettings::defaultColor = {.dark = "#6f6f6f", .light = "#717171"};

struct Ze1DSubgridSettings: Ze1DGridCommonSettings
{
  Q_GADGET

  Q_PROPERTY(int subdivs MEMBER subdivs)
  Q_PROPERTY(bool showCoordinates MEMBER showCoordinates)

public:
  Ze1DSubgridSettings(): Ze1DGridCommonSettings({.show = defaultShow, .lineWidth = defaultLineWidth, .color = defaultColor}) {}

  int subdivs = defaultSubdivs;
  bool showCoordinates = defaultShowCoordinates;

  static constexpr int defaultSubdivs = 2;
  static constexpr bool defaultShowCoordinates = false;
  static constexpr bool defaultShow = true;
  static constexpr double defaultLineWidth = 0.6;
  static const ThemedColor defaultColor;

  struct POD {
    std::optional<bool> show;
    std::optional<double> line_width;
    std::optional<ThemedColor::POD> color;
    std::optional<int> subdivs;

    operator bool () const { return show or line_width or color or subdivs; }
  };

  std::optional<POD> exportPod() const;
  void importPod(POD);

  bool operator == (const Ze1DSubgridSettings&) const = default;
};

inline const ThemedColor Ze1DSubgridSettings::defaultColor = {.dark = "#434343", .light = "#d1d1d1"};

using Ze1DGridSettings = Ze1DGridCommonSettings;

struct ZeGridSettings
{
  Q_GADGET

  Q_PROPERTY(Ze1DGridSettings x MEMBER x)
  Q_PROPERTY(Ze1DGridSettings y MEMBER y)

public:
  Ze1DGridSettings x, y;

  struct POD {
    std::optional<Ze1DGridSettings::POD> x, y;
    operator bool () const { return x or y; }
  };

  std::optional<POD> exportPod() const;
  void importPod(POD);

  bool operator == (const ZeGridSettings&) const = default;
};

struct ZeSubgridSettings
{
  Q_GADGET

  Q_PROPERTY(Ze1DSubgridSettings x MEMBER x)
  Q_PROPERTY(Ze1DSubgridSettings y MEMBER y)

public:
  Ze1DSubgridSettings x, y;

  struct POD {
    std::optional<Ze1DSubgridSettings::POD> x, y;
    operator bool () const { return x or y; }
  };

  std::optional<POD> exportPod() const;
  void importPod(POD);

  bool operator == (const ZeSubgridSettings&) const = default;

};
