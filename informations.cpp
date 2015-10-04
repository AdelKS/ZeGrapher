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








#include "informations.h"

Informations::Informations()
{
    range.Xmax = range.Ymax = 10;
    range.Xmin = range.Ymin = -10;
    range.Xscale = range.Yscale = 1;   

    gridState = orthonormal = updatingLock = false;

}

void Informations::setUnits(Point vec)
{
    units = vec;
}

Point Informations::getUnits()
{
    return units;
}

void Informations::emitDataUpdate()
{
    emit dataUpdated();
}

void Informations::addDataList()
{
    data << QList<QPointF>();

    DataStyle style;
    dataStyle << style;
}

void Informations::removeDataList(int index)
{
    data.removeAt(index);
    dataStyle.removeAt(index);
    emit updateOccured();
}

void Informations::setDataStyle(int index, DataStyle style)
{
    dataStyle[index] = style;
    emit dataUpdated();
}

void Informations::setData(int index, QList<QPointF> list)
{
    data[index] = list;
    emit dataUpdated();
}

int Informations::getDataListsCount()
{
    return data.size();
}

QList<QPointF> Informations::getDataList(int index)
{
    return data[index];
}

DataStyle Informations::getDataStyle(int index)
{
    return dataStyle[index];
}

void Informations::recalculateRegressionCurves(double xUnit, double yUnit, GraphRange range)
{
    for(int i = 0 ; i < regressions.size() ; i++)
        regressions[i]->recalculate(xUnit, yUnit, range);
}

void Informations::addDataRegression(RegressionValuesSaver *reg)
{
    regressions << reg;
    emit regressionAdded();
}

void Informations::removeDataRegression(RegressionValuesSaver *reg)
{
    regressions.removeOne(reg);
    emit regressionRemoved();
}

RegressionValuesSaver *Informations::getRegression(int index)
{
    return regressions.at(index);
}

int Informations::getRegressionsCount()
{
    return regressions.size();
}

void Informations::setParEqsListPointer(QList<ParEqWidget*> *list)
{
    parEqWidgets = list;
}

QList<ParEqWidget*>* Informations::getParEqsList()
{
    return parEqWidgets;
}

void Informations::emitAnimationUpdate()
{
    emit animationUpdate();
}

void Informations::setTangentsListPointer(QList<TangentWidget*> *list)
{
    tangents = list;
}

QList<TangentWidget*>* Informations::getTangentsList()
{
    return tangents;
}

void Informations::setStraightLinesListPointer(QList<StraightLineWidget*> *list)
{
    lines = list;
}

QList<StraightLineWidget*>* Informations::getStraightLinesList()
{
    return lines;
}

void Informations::setSequencesList(QList<SeqCalculator*> list)
{
    sequences = list;
}

QList<SeqCalculator*> Informations::getSeqsList()
{
    return sequences;
}

void Informations::setFunctionsList(QList<FuncCalculator*> list)
{
    functions = list;
}

QList<FuncCalculator*> Informations::getFuncsList()
{
    return functions;
}

void Informations::setRange(const GraphRange &newFenetre)
{
    range = newFenetre;
    emit updateOccured();
}

void Informations::setGridState(bool etat)
{
    gridState = etat;
    emit updateOccured();
}

void Informations::setOrthonormal(bool state)
{
    orthonormal = state;
    emit newOrthonormalityState(state);
    emit updateOccured();
}

void Informations::setOptions(Options opt)
{
    parametres = opt;

    emit updateOccured();
}

void Informations::emitUpdateSignal()
{
    emit updateOccured();
}

void Informations::emitDrawStateUpdate()
{
    emit drawStateUpdateOccured();
}

GraphRange Informations::getRange()
{
    return range;
}

bool Informations::getGridState()
{
    return gridState;

}

bool Informations::isOrthonormal()
{
    return orthonormal;   
}

Options Informations::getOptions()
{
    return parametres;    
}
