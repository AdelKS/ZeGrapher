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

#include "polynomialregression.h"

PolynomialRegression::PolynomialRegression()
{
    approxMethod = ApproachSegments;
}

void PolynomialRegression::setData(const QList<Point> &data)
{
    dataPoints = data;
    qSort(dataPoints);

    min = dataPoints.first().x;
    max = dataPoints.last().x;

    orthonormalBasis.clear();
}

Polynomial PolynomialRegression::getPolynomialRegression(int polynomialDegree, ApproxMethod method)
{
    if(approxMethod != method)
        orthonormalBasis.clear();

    approxMethod = method;

    updateOrthonormalBasis(polynomialDegree);

    QList<double> coefs;
    coefs.reserve(polynomialDegree+1);

    Polynomial pol;

    for(int n = 0 ; n <= polynomialDegree ; n++)
    {
        if(approxMethod == ApproachSegments)
            pol += continuousScalarProduct(data, orthonormalBasis.at(n))*orthonormalBasis.at(n);
        else pol += discreteScalarProduct(data, orthonormalBasis.at(n))*orthonormalBasis.at(n)*orthonormalBasis.at(n);
    }

    return pol;
}

void PolynomialRegression::updateOrthonormalBasis(int maxDegree)
{
    //we use Gram-Schmidt orthonormalisation process

    if(approxMethod == ApproachSegments)
    {
        if(orthonormalBasis.isEmpty())
        {
            Polynomial P(0);
            P *= 1/continuousNorm(P);
            orthonormalBasis << P;
        }

        for(n = orthonormalBasis.size(); n <= maxDegree; n++)
        {
            Polynomial P(n);

            for(int i = 0 ; i < orthonormalBasis.size(); i++)
                P -= continuousScalarProduct(P, orthonormalBasis.at(i), min, max)*orthonormalBasis.at(i);

            P *= 1/continuousNorm(P);
            orthonormalBasis << P;
        }
    }
    else if(approxMethod == ApproachPoints)
    {
        if(orthonormalBasis.isEmpty())
        {
            Polynomial P(0);
            P *= 1/discreteNorm(P);
            orthonormalBasis << P;
        }

        for(n = orthonormalBasis.size(); n <= maxDegree; n++)
        {
            Polynomial P(n);

            for(int i = 0 ; i < orthonormalBasis.size(); i++)
                P -= discreteScalarProduct(P, orthonormalBasis.at(i), dataPoints)*orthonormalBasis.at(i);

            P *= 1/discreteNorm(P);
            orthonormalBasis << P;
        }
    }
}

PolynomialRegression::~PolynomialRegression()
{

}

