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

#include <QtQmlIntegration/qqmlintegration.h>
#include <QWindow>
#include <QObject>
#include <QFont>
#include <QColor>
#include <QTimer>
#include <QPointer>

#include "Utils/axissettings.h"
#include "Utils/fonts.h"
#include "Utils/graphrange.h"
#include "Utils/gridsettings.h"
#include "Utils/themedcolor.h"
#include "Utils/yaml.h"
#include "structures.h"

struct ZeGraphSettings: QObject
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QFont font WRITE setFont MEMBER font NOTIFY fontChanged)
  Q_PROPERTY(ThemedColor backgroundColor WRITE setBackgroundColor READ getBackgroundColor NOTIFY backgroundColorChanged)
  Q_PROPERTY(ZeZoomSettings zoom WRITE setZoomSettings MEMBER zoom NOTIFY zoomSettingsChanged)
  Q_PROPERTY(ZeSizeSettings size WRITE setSizeSettings MEMBER size NOTIFY sizeSettingsChanged)
  Q_PROPERTY(ZeAxesSettings axes WRITE setAxesSettings MEMBER axes NOTIFY axesSettingsChanged)
  Q_PROPERTY(ZeGridSettings grid WRITE setGridSettings MEMBER grid NOTIFY gridSettingsChanged)
  Q_PROPERTY(ZeSubgridSettings subgrid WRITE setSubgridSettings MEMBER subgrid NOTIFY subgridSettingsChanged)
  Q_PROPERTY(zg::GraphRange* range READ getRangePtr CONSTANT)
  Q_PROPERTY(zg::mathobj::Expr* xMultiplier READ getXMultiplierPtr CONSTANT)
  Q_PROPERTY(zg::mathobj::Expr* yMultiplier READ getYMultiplierPtr CONSTANT)

  Q_PROPERTY(QSize availableSizePx WRITE setAvailableSizePx MEMBER availableSizePx NOTIFY availableSizePxChanged)
  Q_PROPERTY(QSizeF availableSizeCm READ getAvailableSizeCm NOTIFY availableSizeCmChanged)

  Q_PROPERTY(double totalScaleFactor READ getTotalScaleFactor NOTIFY totalScaleFactorChanged)

  Q_PROPERTY(int minPointsLg2 MEMBER minPointsLg2 NOTIFY minPointsLg2Changed)
  Q_PROPERTY(int maxPointsLg2 MEMBER maxPointsLg2 NOTIFY maxPointsLg2Changed)
  Q_PROPERTY(double pixelDensity READ getPixelDensity NOTIFY pixelDensityChanged)

public:
  explicit ZeGraphSettings(QObject* parent = nullptr);

  const ZeZoomSettings& getZoom() const { return zoom; };
  const ZeSizeSettings& getSize() const { return size; };
  const ZeAxesSettings& getAxes() const { return axes; };
  const ZeGridSettings& getGrid() const { return grid; };
  const ZeSubgridSettings& getSubgrid() const { return subgrid; };
  zg::GraphRange* getRangePtr() { return &range; }
  zg::mathobj::Expr* getXMultiplierPtr() { return &xMultiplier; }
  zg::mathobj::Expr* getYMultiplierPtr() { return &yMultiplier; }

  /// @brief the expression that an axis's ticks are multiples of, such as
  ///        'pi'
  template <ZeAxisName axis>
  zg::mathobj::Expr& multiplier()
  {
    if constexpr (axis == ZeAxisName::X)
      return xMultiplier;
    else return yMultiplier;
  }
  QFont getFont() const { return font; };
  const ThemedColor& getBackgroundColor() const { return backgroundColor; };
  QSizeF getAvailableSizeCm() const { return availableSizeCm; }
  double getTotalScaleFactor() const { return zoom.zoom * size.scalingFactor * pixelDensity / targetPixelDensity; }

  int getMinPointsLg2() const { return minPointsLg2; }
  int getMaxPointsLg2() const { return maxPointsLg2; }

  double getPixelDensity() const { return pixelDensity; }

  /// @note in pixels per centimeter
  double pixelDensity = 35;

  /// @brief baseline pixel density that 1.0x scaling should achieve
  static constexpr double targetPixelDensity = 35;

  /// @brief the pixel densities a screen can have, 25 to 400 dpi
  static constexpr double minPixelDensity = 25 / cmPerInch;
  static constexpr double maxPixelDensity = 400 / cmPerInch;

  struct POD {
    std::optional<zg::GraphRange::POD> range;
    std::optional<double> zoom;
    std::optional<ZeSizeSettings::POD> size;
    std::optional<ZeAxesSettings::POD> axes;
    std::optional<ZeGridSettings::POD> grid;
    std::optional<ZeSubgridSettings::POD> subgrid;
    std::optional<zg::yml::QFontPOD> font;
    std::optional<ThemedColor::POD> background;
    std::optional<int> min_points_log2;
    std::optional<int> max_points_log2;

    operator bool () const { return range or zoom or size or axes or grid or subgrid or font or background or min_points_log2 or max_points_log2; }
  };

  std::optional<POD> exportPod() const;
  void importPod(POD);

public slots:
  void setZoomSettings(ZeZoomSettings);
  void setSizeSettings(ZeSizeSettings);
  void setAxesSettings(ZeAxesSettings);
  void setGridSettings(ZeGridSettings);
  void setSubgridSettings(ZeSubgridSettings);
  void setFont(QFont);
  void setBackgroundColor(ThemedColor);
  void updateSizes();
  void computeZoom();
  void setAvailableSizePx(QSize);

signals:
  void zoomSettingsChanged();
  void sizeSettingsChanged();
  void axesSettingsChanged();
  void gridSettingsChanged();
  void subgridSettingsChanged();
  void fontChanged();
  void backgroundColorChanged();
  void availableSizePxChanged();
  void availableSizeCmChanged();
  void totalScaleFactorChanged();
  void minPointsLg2Changed();
  void maxPointsLg2Changed();
  void pixelDensityChanged();

public slots:
  void screenChanged(QWindow* win);

protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

  /// @brief a default background follows the window color in the dark theme
  void followWindowColor();

  void updatePixelDensity();

  QTimer paletteTimer;

  QPointer<QScreen> screen;

public:
  zg::GraphRange range;
  zg::mathobj::Expr xMultiplier, yMultiplier;

protected:
  static constexpr int defaultMinPoints = 7;
  static constexpr int defaultMaxPoints = 14;
  static constexpr int lowestPointsLg2 = 2, highestPointsLg2 = 16;

  // log2 for adaptive sampling
  int minPointsLg2 = defaultMinPoints;
  int maxPointsLg2 = defaultMaxPoints;

  QFont font = zg::mathFont();

  ThemedColor defaultBgColor;
  ThemedColor backgroundColor;

  ZeZoomSettings zoom;
  ZeSizeSettings size;
  ZeAxesSettings axes;
  ZeGridSettings grid;
  ZeSubgridSettings subgrid;

  QSize availableSizePx;
  QSizeF availableSizeCm;
};
