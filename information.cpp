/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane <adel.ks@zegrapher.com>
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
    range.Xmax = range.Ymax = 10;
    range.Xmin = range.Ymin = -10;
    range.Xscale = range.Yscale = 1;   

    gridState = orthonormal = updatingLock = false;

}

void Information::setUnits(Point vec)
{
    units = vec;
}

Point Information::getUnits()
{
    return units;
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

void Information::recalculateRegressionCurves(double xUnit, double yUnit, GraphRange range)
{
    for(int i = 0 ; i < regressions.size() ; i++)
        regressions[i]->recalculate(xUnit, yUnit, range);
}

void Information::addDataRegression(RegressionValuesSaver *reg)
{
    regressions << reg;
    emit regressionAdded();
}

void Information::removeDataRegression(RegressionValuesSaver *reg)
{
    regressions.removeOne(reg);
    emit regressionRemoved();
}

RegressionValuesSaver *Information::getRegression(int index)
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

void Information::setRange(const GraphRange &newWindow)
{
    range = newWindow;
    emit updateOccured();
}

void Information::setGridState(bool etat)
{
    gridState = etat;
    emit updateOccured();
}

void Information::setOrthonormal(bool state)
{
    orthonormal = state;
    emit newOrthonormalityState(state);
    emit updateOccured();
}

void Information::setOptions(Options opt)
{
    auto oldParameters = parameters;

    parameters = opt;

    if(oldParameters.distanceBetweenPoints != parameters.distanceBetweenPoints)
    {
        for(auto reg : regressions)
            reg->setPixelStep(parameters.distanceBetweenPoints);

    }

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

GraphRange Information::getRange()
{
    return range;
}

bool Information::getGridState()
{
    return gridState;

}

bool Information::isOrthonormal()
{
    return orthonormal;   
}

Options Information::getOptions()
{
    return parameters;    
}
