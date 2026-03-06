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
  QObject(parent), appSettings(new ZeAppSettings(this)), graphSettings(new ZeGraphSettings(this)), graph_range(new zg::GraphRange(this))
{
  emit appSettingsChanged();
  emit graphSettingsChanged();
}

void Information::emitDataUpdate()
{
  emit dataUpdated();
}

void Information::emitAnimationUpdate()
{
  emit animationUpdate();
}

const ZeGraphSettings& Information::getGraphSettings() const
{
  return *graphSettings;
}

void Information::setFont(QFont font)
{
  appSettings->setFont(font);
  graphSettings->setFont(font);
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

void Information::screenChanged(QWindow* win)
{

  QSizeF pixelSize = win->screen()->size().toSizeF();
  QSizeF cmSize = win->screen()->physicalSize() / 10;

  qDebug() << "Monitor pixel size: " << pixelSize;
  qDebug() << "Monitor cm size: " << cmSize;

  double cm_per_mm = sqrt(pixelSize.width() * pixelSize.height() / (cmSize.width() * cmSize.height()));

  if (pixelDensity != cm_per_mm)
  {
    qDebug() << "pixel density " << cm_per_mm << "px per cm";
    pixelDensity = cm_per_mm;
    emit pixelDensityChanged();
  }
}
