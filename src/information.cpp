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

Information::Information(QObject* parent): QObject(parent)
{}

void Information::emitDataUpdate()
{
  emit dataUpdated();
}

void Information::addDataList(const std::shared_ptr<UserData const>& userData)
{
  userDataSets.push_back(userData);
}

void Information::removeDataList(const std::shared_ptr<UserData const>& userData)
{
  userDataSets.remove(userData);
  emit updateOccured();
}

int Information::getDataListsCount()
{
  return userDataSets.size();
}

std::shared_ptr<const UserData> Information::getDataPoints(int index)
{
  auto it = userDataSets.begin();
  std::advance(it, index);
  return *it;
}

void Information::setGraphSettings(const ZeGraphSettings& graphSettings)
{
  this->graphSettings = graphSettings;
}

void Information::addDataRegression(Regression* reg)
{
  regressions << reg;
  emit regressionAdded(reg);
}

void Information::removeDataRegression(Regression* reg)
{
  regressions.removeOne(reg);
  emit regressionRemoved(reg);
}

QList<Regression*> Information::getRegressions()
{
  return regressions;
}

Regression* Information::getRegression(int index)
{
  return regressions.at(index);
}

int Information::getRegressionsCount()
{
  return regressions.size();
}

void Information::setParEqsListPointer(QList<ParEqWidget*>* list)
{
  parEqWidgets = list;
}

QList<ParEqWidget*>* Information::getParEqsList()
{
  return parEqWidgets;
}

void Information::emitAnimationUpdate()
{
  emit animationUpdate();
}

void Information::setSequencesList(QList<SeqCalculator*> list)
{
  sequences = list;
}

QList<SeqCalculator*> Information::getSeqsList()
{
  return sequences;
}

void Information::setFunctionsList(QList<Function*> list)
{
  functions = list;
}

QList<Function*> Information::getFuncsList()
{
  return functions;
}

void Information::setGraphRange(const GraphRange& range)
{
  if (this->range != range)
  {
    this->range = range;

    emit graphRangeChanged(range);
  }
}

QPalette Information::getValidSyntaxPalette() const
{
  QPalette validPalette;
  validPalette.setColor(QPalette::Base, appSettings.validSyntax);
  validPalette.setColor(QPalette::Text, Qt::black);
  return validPalette;
}

QPalette Information::getInvalidSyntaxPalette() const
{
  QPalette invalidPalette;
  invalidPalette.setColor(QPalette::Base, appSettings.invalidSyntax);
  invalidPalette.setColor(QPalette::Text, Qt::black);
  return invalidPalette;
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

const GraphRange& Information::getGraphRange() const
{
  return range;
}

const ZeAppSettings& Information::getAppSettings() const
{
  return appSettings;
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

void Information::setAppSettings(const ZeAppSettings& appSettings)
{
  this->appSettings = appSettings;
  emit appSettingsChanged();
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

void Information::mathObjectUpdated()
{
  for (auto&& f: mathObjects)
    f->sync();

  emit updateOccured();
}

void Information::removeMathObject(size_t slot)
{
  mathObjects.at(slot)->deleteLater();
  mathObjects.free(slot);
}

size_t Information::addMathObject()
{
  size_t slot = mathObjects.emplace(new zg::MathObject(this));
  return slot;
}
