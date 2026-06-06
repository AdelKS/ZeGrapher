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
#include <QObject>
#include <QFont>
#include <QColor>

#include "Utils/axissettings.h"
#include "Utils/graphrange.h"
#include "Utils/gridsettings.h"
#include "Utils/themedcolor.h"
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
  Q_PROPERTY(zg::GraphRange* range READ getRange)

  Q_PROPERTY(QSize availableSizePx WRITE setAvailableSizePx MEMBER availableSizePx NOTIFY availableSizePxChanged)
  Q_PROPERTY(QSizeF availableSizeCm READ getAvailableSizeCm NOTIFY availableSizeCmChanged)

  Q_PROPERTY(double totalScaleFactor READ getTotalScaleFactor NOTIFY totalScaleFactorChanged)

  Q_PROPERTY(int minPointsLg2 MEMBER minPointsLg2 NOTIFY minPointsLg2Changed)
  Q_PROPERTY(int maxPointsLg2 MEMBER maxPointsLg2 NOTIFY maxPointsLg2Changed)

public:
  explicit ZeGraphSettings(QObject* parent = nullptr);

  const ZeZoomSettings& getZoom() const { return zoom; };
  const ZeSizeSettings& getSize() const { return size; };
  const ZeAxesSettings& getAxes() const { return axes; };
  const ZeGridSettings& getGrid() const { return grid; };
  const ZeSubgridSettings& getSubgrid() const { return subgrid; };
  zg::GraphRange* getRange() { return &range; }
  QFont getFont() const { return font; };
  const ThemedColor& getBackgroundColor() const { return backgroundColor; };
  Q_INVOKABLE QSizeF getAvailableSizeCm() const { return availableSizeCm; }
  Q_INVOKABLE QSize getAvailableSizePx() const { return availableSizePx; }
  double getTotalScaleFactor() const { return zoom.zoom * size.scalingFactor; }

  int getMinPointsLg2() const { return minPointsLg2; }
  int getMaxPointsLg2() const { return maxPointsLg2; }

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

protected slots:
  void onSystemPaletteChange();

protected:
  ZeZoomSettings zoom;
  ZeSizeSettings size;
  ZeAxesSettings axes;
  ZeGridSettings grid;
  ZeSubgridSettings subgrid;
  zg::GraphRange range;

  // log2 for adaptive sampling
  int minPointsLg2 = 7;
  int maxPointsLg2 = 14;

  QFont font;

  ThemedColor defaultBgColor;
  ThemedColor backgroundColor;

  QSize availableSizePx;
  QSizeF availableSizeCm;
};
