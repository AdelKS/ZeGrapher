/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.1.
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

#ifndef REGRESSIONVALUESSAVER_H
#define REGRESSIONVALUESSAVER_H

#include "informations.h"


class RegressionValuesSaver
{
public:
    RegressionValuesSaver(Informations *info);

    void calculateAll(double new_xUnit, double new_yUnit);
    void move(double pixels);

    double getStartAbsicssaUnit();
    double getStartAbscissaPixel();

    QList< QList<double> >* getRegressionValsListPointer();

    ~RegressionValuesSaver();

protected:

    Informations *informations;
    GraphRange graphRange;    

    double xUnit, yUnit, pixelStep, unitStep;
    double startAbscissa_pixel, startAbscissa_unit, endAbscissa_pixel, endAbscissa_unit;
    double deplacement;

    QList< QList<double> > regressionVals;
};

#endif // REGRESSIONVALUESSAVER_H
