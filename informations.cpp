/****************************************************************************
**  Copyright (c) 2013, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.0.
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

    parametres.couleurDuFond = QColor(Qt::white);
    parametres.couleurDesAxes = QColor(Qt::black);
    parametres.couleurQuadrillage = QColor(Qt::gray);
    parametres.epaisseurDesCourbes = 1;
    parametres.distanceEntrePoints = 2.625;
    parametres.numSize = 11;
    parametres.lissage = true;
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
