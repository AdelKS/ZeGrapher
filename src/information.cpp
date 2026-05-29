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
#include "globalvars.h"

Information::Information(QObject* parent):
  QObject(parent), appSettings(this)
{
  emit appSettingsChanged();
}

void Information::emitDataUpdate()
{
  emit dataUpdated();
}

void Information::emitAnimationUpdate()
{
  emit animationUpdate();
}

void Information::setAppFont(QFont font)
{
  appSettings.setFont(font);
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

  double px_per_cm = sqrt(pixelSize.width() * pixelSize.height() / (cmSize.width() * cmSize.height()));

  if (pixelDensity != px_per_cm)
  {
    qDebug() << "pixel density " << px_per_cm << "px per cm";
    pixelDensity = px_per_cm;
    emit pixelDensityChanged();
  }
}

void Information::exportYaml(QUrl filename)
{
  Q_UNUSED(filename);
}
