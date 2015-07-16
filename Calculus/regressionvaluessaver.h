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

#include <algorithm>

#include "informations.h"



class RegressionValuesSaver : public QObject
{
    Q_OBJECT

public:
    RegressionValuesSaver(Informations *info);
    ~RegressionValuesSaver();

    void recalculate(double new_xUnit, double new_yUnit);

    QPolygonF& getCurve(int reg);

public slots:
     void calculateNewCurves();

protected:
    void calculatePolarRegressionCurve(Regression *reg);
    void calculateCartesianRegressionCurve(Regression *reg);

    Informations *informations;      

    double xUnit, yUnit, pixelStep, xUnitStep;

    QList< QPolygonF > regressionCurves;
};

#endif // REGRESSIONVALUESSAVER_H
