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

#ifndef POLYNOMIALREGRESSION_H
#define POLYNOMIALREGRESSION_H

#include "polynomial.h"
#include "Structures.h"
#include "regression.h"
#include <QList>

enum ApproxMethod { ApproachPoints, ApproachSegments};

class PolynomialRegression : public Regression
{
public:
    PolynomialRegression(int dataNumber);

    void setDataNumber(int num);

    double eval(double x);
    QString getAbscissaName();
    QString getOrdinateName();
    QString getInfo();
    bool getDrawState();

    void setData(const QList<Point> &data);
    void calculatePolynomialRegression(int polynomialDegree, ApproxMethod method);

    ~PolynomialRegression();

protected:
    void updateOrthonormalBasis(int maxDegree);
    double continuousNorm(const Polynomial &P);
    double discreteNorm(const Polynomial &P);

    int dataNum;
    Polynomial pol;
    ApproxMethod approxMethod;
    double min, max; //integration segment for dot product: integrate between min and max
    QList<Point> dataPoints;
    QList<Polynomial> orthonormalBasis;
};

double discreteScalarProduct(const QList<Point> &data, const Polynomial &P);
double continuousScalarProduct(const QList<Point> &data, const Polynomial &P);

#endif // POLYNOMIALREGRESSION_H
