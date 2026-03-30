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

#include <QtQml/qqmlregistration.h>
#include <QObject>
#include <QFont>
#include <QColor>

#include "Utils/axissettings.h"
#include "structures.h"

struct ZeGraphSettings: QObject
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QFont font WRITE setFont MEMBER font NOTIFY fontChanged)
  Q_PROPERTY(QColor backgroundColor WRITE setBackgroundColor READ getBackgroundColor NOTIFY backgroundColorChanged)
  Q_PROPERTY(ZeZoomSettings zoom WRITE setZoomSettings MEMBER zoom NOTIFY zoomSettingsChanged)
  Q_PROPERTY(ZeSizeSettings size WRITE setSizeSettings MEMBER size NOTIFY sizeSettingsChanged)
  Q_PROPERTY(ZeAxesSettings axes WRITE setAxesSettings MEMBER axes NOTIFY axesSettingsChanged)
  Q_PROPERTY(ZeGridSettings grid WRITE setGridSettings MEMBER grid NOTIFY gridSettingsChanged)

  Q_PROPERTY(QSize availableSizePx WRITE setAvailableSizePx MEMBER availableSizePx NOTIFY availableSizePxChanged)
  Q_PROPERTY(QSizeF availableSizeCm READ getAvailableSizeCm NOTIFY availableSizeCmChanged)

  Q_PROPERTY(double targetSamplingDistancePx WRITE setTargetSamplingDistancePx MEMBER targetSamplingDistancePx NOTIFY targetSamplingDistanceChanged)

  QML_VALUE_TYPE(ZeGraphSettings)

public:
  explicit ZeGraphSettings(QObject* parent = nullptr): QObject(parent) {};

  const ZeZoomSettings& getZoom() const { return zoom; };
  const ZeSizeSettings& getSize() const { return size; };
  const ZeAxesSettings& getAxes() const { return axes; };
  const ZeGridSettings& getGrid() const { return grid; };
  QFont getFont() const { return font; };
  QColor getBackgroundColor() const;
  Q_INVOKABLE QSizeF getAvailableSizeCm() const { return availableSizeCm; }
  Q_INVOKABLE QSize getAvailableSizePx() const { return availableSizePx; }
  double getTargetSamplingDistancePx() const { return targetSamplingDistancePx; }

public slots:
  void setZoomSettings(ZeZoomSettings);
  void setSizeSettings(ZeSizeSettings);
  void setAxesSettings(ZeAxesSettings);
  void setGridSettings(ZeGridSettings);
  void setFont(QFont);
  void setBackgroundColor(QColor);
  void updateSizes();
  void computeZoom();
  void setAvailableSizePx(QSize);
  void setTargetSamplingDistancePx(double);

signals:
  void zoomSettingsChanged();
  void sizeSettingsChanged();
  void axesSettingsChanged();
  void gridSettingsChanged();
  void fontChanged();
  void backgroundColorChanged();
  void availableSizePxChanged();
  void availableSizeCmChanged();
  void targetSamplingDistanceChanged();

protected:
  ZeZoomSettings zoom;
  ZeSizeSettings size;
  ZeAxesSettings axes;
  ZeGridSettings grid;

  QFont font;

  QColor backgroundColorLight = Qt::white;
  QColor backgroundColorDark = "#202326";

  QSize availableSizePx;
  QSizeF availableSizeCm;

  /// @brief target distance in pixels that neighboring sampled points should have
  /// @note forwarded to the Sampler class
  double targetSamplingDistancePx = 1;
};
