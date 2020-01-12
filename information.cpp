/****************************************************************************
**  Copyright (c) 2019, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
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

Information::Information()
{

}

void Information::emitDataUpdate()
{
    emit dataUpdated();
}

void Information::addDataList()
{
    data << QList<QPointF>();

    DataStyle style;
    dataStyle << style;
}

void Information::removeDataList(int index)
{
    data.removeAt(index);
    dataStyle.removeAt(index);
    emit updateOccured();
}

void Information::setDataStyle(int index, DataStyle style)
{
    dataStyle[index] = style;
    emit dataUpdated();
}

void Information::setData(int index, QList<QPointF> list)
{
    data[index] = list;
    emit dataUpdated();
}

int Information::getDataListsCount()
{
    return data.size();
}

QList<QPointF> Information::getDataList(int index)
{
    return data[index];
}

DataStyle Information::getDataStyle(int index)
{
    return dataStyle[index];
}

void Information::addDataRegression(Regression *reg)
{
    regressions << reg;
    emit regressionAdded(reg);
}

void Information::removeDataRegression(Regression *reg)
{
    regressions.removeOne(reg);
    emit regressionRemoved(reg);
}

QList<Regression*> Information::getRegressions()
{
    return regressions;
}

Regression *Information::getRegression(int index)
{
    return regressions.at(index);
}

int Information::getRegressionsCount()
{
    return regressions.size();
}

void Information::setParEqsListPointer(QList<ParEqWidget*> *list)
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

void Information::setTangentsListPointer(QList<TangentWidget*> *list)
{
    tangents = list;
}

QList<TangentWidget*>* Information::getTangentsList()
{
    return tangents;
}

void Information::setStraightLinesListPointer(QList<StraightLineWidget*> *list)
{
    lines = list;
}

QList<StraightLineWidget*>* Information::getStraightLinesList()
{
    return lines;
}

void Information::setSequencesList(QList<SeqCalculator*> list)
{
    sequences = list;
}

QList<SeqCalculator*> Information::getSeqsList()
{
    return sequences;
}

void Information::setFunctionsList(QList<FuncCalculator*> list)
{
    functions = list;
}

QList<FuncCalculator*> Information::getFuncsList()
{
    return functions;
}

void Information::setGraphRange(const GraphRange &range)
{
    viewSettings.range = range;

    emit graphRangeChanged(range);
}

const ZeGraphSettings& Information::getGraphSettings()
{
    return viewSettings.graph;
}

const ZeGridSettings& Information::getGridSettings()
{
    return viewSettings.grid;
}

const ZeAxesSettings& Information::getAxesSettings()
{
    return viewSettings.axes;
}

const GraphRange& Information::getGraphRange()
{
    return viewSettings.range;
}

void Information::setOrthonormal(bool state)
{

    emit updateOccured();
}

bool Information::isOrthonormal()
{
    return viewSettings.orthonormal;
}

void Information::setViewSettings(const ZeViewSettings &viewSettings)
{
    this->viewSettings = viewSettings;

    emit newViewSettings();
}

void Information::emitUpdateSignal()
{
    emit updateOccured();
}

void Information::emitDrawStateUpdate()
{
    emit drawStateUpdateOccured();
}

const ZeViewSettings& Information::getViewSettings()
{
    return viewSettings;
}
