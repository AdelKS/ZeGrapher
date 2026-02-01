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

#include "information.h"

Information::Information(QObject* parent):
  QObject(parent), appSettings(new ZeAppSettings(this)), graph_range(new zg::GraphRange(this))
{
  emit appSettingsChanged();
}

void Information::emitDataUpdate()
{
  emit dataUpdated();
}

void Information::setGraphSettings(const ZeGraphSettings& graphSettings)
{
  this->graphSettings = graphSettings;
}

void Information::emitAnimationUpdate()
{
  emit animationUpdate();
}

const ZeSizeSettings& Information::getGraphSizeSettings() const
{
  return sizeSettings;
}

const ZeZoomSettings& Information::getGraphZoomSettings() const
{
  return zoomSettings;
}

const ZeGraphSettings& Information::getGraphSettings() const
{
  return graphSettings;
}

const ZeGridSettings& Information::getGridSettings() const
{
  return gridSettings;
}

const ZeAxesSettings& Information::getAxesSettings() const
{
  return axesSettings;
}

void Information::setAppFont(QFont font)
{
  if (font != appFont)
  {
    qDebug() << "Changing app font to: " << font.family();
    qGuiApp->setFont(font);
    appFont = font;

    emit appFontChanged();
  }
}

void Information::setGraphRangeMouseEdit(const zg::real_range2d& snapshot)
{
  graph_range->update(snapshot);
  emit rangeChangedWithMouse();
}

void Information::setOrthonormal([[maybe_unused]] bool state)
{
  // TODO
  emit updateOccured();
}

void Information::setGraphSizeSettings(const ZeSizeSettings& graphSizeSettings)
{
  if (sizeSettings != graphSizeSettings)
  {
    sizeSettings = graphSizeSettings;

    emit graphSizeSettingsChanged();
  }
}

void Information::setGraphZoomSettings(const ZeZoomSettings& zoomSettings)
{
  if (this->zoomSettings != zoomSettings)
  {
    this->zoomSettings = zoomSettings;

    emit graphZoomSettingsChanged();
  }
}

void Information::setGridSettings(const ZeGridSettings& gridSettings)
{
  if (this->gridSettings != gridSettings)
  {
    this->gridSettings = gridSettings;

    emit gridSettingsChanged();
  }
}

void Information::setAxesSettings(const ZeAxesSettings& axesSettings)
{
  if (this->axesSettings != axesSettings)
  {
    this->axesSettings = axesSettings;

    emit axesSettingsChanged();
  }
}

void Information::emitUpdateSignal()
{
  emit updateOccured();
}

void Information::emitDrawStateUpdate()
{
  emit drawStateUpdateOccured();
}

void Information::setExportFileName(QString fileName)
{
  exportFileName = fileName;
}

QString Information::getExportFileName()
{
  return exportFileName;
}

void Information::setScreenDpi(double dpi)
{
  if (screenDpi != dpi)
  {
    screenDpi = dpi;
    updateSizes();
    emit screenDpiChanged();
  }
}

void Information::primaryScreenChanged()
{
  QObject::connect(qGuiApp->primaryScreen(), &QScreen::physicalDotsPerInchChanged, this, &Information::primaryScreenChanged);

  setScreenDpi(qGuiApp->primaryScreen()->physicalDotsPerInch());
}

void Information::setAvailableSheetSizePx(QSize size)
{
  if (availableSheetSizePx != size)
  {
    availableSheetSizePx = size;
    availableSheetSizeCm = size.toSizeF() / screenDpi * CM_PER_INCH;
    updateSizes();
    emit availableSheetSizePxChanged();
  }
}

void Information::updateSizes()
{
  ZeSizeSettings old = sizeSettings;

  if (sizeSettings.sheetFillsWindow)
  {
    sizeSettings.pxSheetSize = availableSheetSizePx;
    sizeSettings.cmSheetSize = sizeSettings.pxSheetSize / screenDpi * CM_PER_INCH;
  }

  if (sizeSettings.figureFillsSheet)
  {
    sizeSettings.cmMargins = 0;
    sizeSettings.pxMargins = 0;

    sizeSettings.pxFigureSize = sizeSettings.pxSheetSize;
    sizeSettings.cmFigureSize = sizeSettings.cmSheetSize;
  }

  if (sizeSettings.sizeUnit == SizeUnit::CENTIMETER)
  {
    sizeSettings.pxSheetSize = (sizeSettings.cmSheetSize * screenDpi / CM_PER_INCH).toSize();
  }

  if (old != sizeSettings)
    emit graphSizeSettingsChanged();
}
