/****************************************************************************
**  Copyright (c) 2016, Adel Kara Slimane <adel.ks@zegrapher.com>
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



#include "Widgets/pareqcontroller.h"

ParEqController::ParEqController(QList<ParEqWidget *> *list)
{
    parEqs = list;

    freq = INIT_FREQ;
    period = INIT_INCR_PERIOD;

    ratio = (double)(1000.0/freq)/(double)(period);

    animationTimer.setInterval(1000.0/INIT_FREQ);

    connect(&animationTimer, SIGNAL(timeout()), this, SLOT(nextAnimationFrame()));

    animationTimer.start();
}

void ParEqController::nextAnimationFrame()
{
    bool update = false;

    for(int i = 0 ; i < parEqs->size(); i++)
    {
        if(parEqs->at(i)->isAnimated() && parEqs->at(i)->isPlaying())
        {
            parEqs->at(i)->nextFrame();
            update = true;
        }
    }

    if(update)
        emit animationUpdate();
}

void ParEqController::newParEqAdded()
{
    parEqs->last()->setRatio(ratio);
}


void ParEqController::updateRatioInParEq()
{
    for(int i = 0; i < parEqs->size(); i++)
    {
        parEqs->at(i)->setRatio(ratio);
    }
}


void ParEqController::setUpdateFreq(int Hz)
{
    freq = Hz;
    ratio = (double)(1000.0/freq)/(double)(period);
    animationTimer.setInterval(1000.0/Hz);
    updateRatioInParEq();
}

void ParEqController::setIncrPeriod(int msecs)
{
    period = msecs;
    ratio = (double)(1000.0/freq)/(double)(period);
    updateRatioInParEq();
}


