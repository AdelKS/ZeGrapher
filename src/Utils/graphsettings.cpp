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

#include "graphsettings.h"
#include "globalvars.h"

#include <QGuiApplication>
#include <QStyleHints>

void ZeGraphSettings::setZoomSettings(ZeZoomSettings s)
{
  if (zoom == s)
    return;

  zoom = s;
  emit zoomSettingsChanged();
};

void ZeGraphSettings::setSizeSettings(ZeSizeSettings s)
{
  if (size == s)
    return;

  size = s;

  emit sizeSettingsChanged();

  updateSizes();
  computeZoom();
};

void ZeGraphSettings::setAxesSettings(ZeAxesSettings s)
{
  if (axes == s)
    return;

  axes = s;
  emit axesSettingsChanged();
};

void ZeGraphSettings::setGridSettings(ZeGridSettings s)
{
  if (grid == s)
    return;

  grid = s;
  emit gridSettingsChanged();
};

void ZeGraphSettings::setSubgridSettings(ZeSubgridSettings s)
{
  if (subgrid == s)
    return;

  subgrid = s;
  emit subgridSettingsChanged();
};

void ZeGraphSettings::setFont(QFont f)
{
  if (font == f)
    return;

  font = f;
  emit fontChanged();
};

void ZeGraphSettings::setBackgroundColor(QColor c)
{
  QColor& bg = [this] {
    if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
      return std::ref(backgroundColorDark);
    else return std::ref(backgroundColorLight);
  }();

  if (bg == c)
    return;

  bg = c;
  emit backgroundColorChanged();
}

QColor ZeGraphSettings::getBackgroundColor() const {
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    return backgroundColorDark;
  else return backgroundColorLight;
}

void ZeGraphSettings::updateSizes()
{
  ZeSizeSettings oldSize = size;

  if (size.sheetFillsWindow)
  {
    size.pxSheetSize = availableSizePx;
    size.cmSheetSize = availableSizePx.toSizeF() / information.getPixelDensity();
  }

  if (size.sizeUnit == SizeUnit::CENTIMETER)
    size.pxSheetSize = (size.cmSheetSize * information.getPixelDensity()).toSize();
  else size.cmSheetSize = size.pxSheetSize.toSizeF() / information.getPixelDensity();

  if (size != oldSize)
    emit sizeSettingsChanged();
}


void ZeGraphSettings::computeZoom()
{
  qDebug() << "Recomputing zoom";

  if (zoom.zoomingType != ZoomingType::FITSHEET)
    return;

  double ratio = availableSizeCm.height() / availableSizeCm.width();
  double targetRatio = size.cmSheetSize.height() / size.cmSheetSize.width();

  double newZoom;
  if (ratio <= targetRatio)
    newZoom = availableSizeCm.height() / size.cmSheetSize.height();
  else
    newZoom = availableSizeCm.width() / size.cmSheetSize.width();

  if (fabs(newZoom - zoom.zoom) > 0.0001)
  {
    zoom.zoom = newZoom;
    emit zoomSettingsChanged();
  }
}

void ZeGraphSettings::setTargetSamplingDistancePx(double target)
{
  if (targetSamplingDistancePx == target)
    return;

  targetSamplingDistancePx = target;
  emit targetSamplingDistanceChanged();
}

void ZeGraphSettings::setAvailableSizePx(QSize s)
{
  if (availableSizePx != s)
  {
    availableSizePx = s;
    availableSizeCm = s.toSizeF() / information.getPixelDensity();
    updateSizes();
    computeZoom();
    emit availableSizePxChanged();
  }
}
