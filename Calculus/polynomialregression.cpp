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

PolynomialRegression::PolynomialRegression(int dataNumber)
{
    approxMethod = ApproachSegments;
    dataNum = dataNumber;
}

void PolynomialRegression::setAbscissaName(QString name)
{
    abscissa = name;
}

void PolynomialRegression::setOrdinateName(QString name)
{
    ordinate = name;
}

void PolynomialRegression::setInfo(QString info)
{
    infos = info;
}

void PolynomialRegression::setDrawState(bool state)
{
    drawState = state;
}

void PolynomialRegression::setDataNumber(int num)
{
    dataNum = num;
}   

double PolynomialRegression::eval(double x)
{
    return pol.eval(x);
}

QString PolynomialRegression::getAbscissaName()
{
    return abscissa;
}

QString PolynomialRegression::getOrdinateName()
{
    return ordinate;
}

QString PolynomialRegression::getInfo()
{
    return infos;
}

bool PolynomialRegression::getDrawState()
{
    return drawState;
}

int PolynomialRegression::getDataNum()
{
    return dataNum;
}

void PolynomialRegression::setData(const QList<Point> &data)
{
    dataPoints = data;
    qSort(dataPoints);

    xmin = dataPoints.first().x;
    xmax = dataPoints.last().x;

    orthonormalBasis.clear();
}

void PolynomialRegression::calculatePolynomialRegression(int polynomialDegree, ApproxMethod method)
{
    if(approxMethod != method)
        orthonormalBasis.clear();

    approxMethod = method;

    updateOrthonormalBasis(polynomialDegree);   

    pol.resetToZero();

    for(int n = 0 ; n <= polynomialDegree ; n++)
    {
        if(approxMethod == ApproachSegments)
            pol += continuousScalarProduct(data, orthonormalBasis.at(n))*orthonormalBasis.at(n);
        else pol += discreteScalarProduct(data, orthonormalBasis.at(n))*orthonormalBasis.at(n);
    }    
}

void PolynomialRegression::updateOrthonormalBasis(int maxDegree)
{
    //we use Gram-Schmidt orthonormalisation process

    if(approxMethod == ApproachSegments)
    {
        if(orthonormalBasis.isEmpty())
        {
            Polynomial P(0);
            P *= 1/continuousNorm(P, xmin, xmax);
            orthonormalBasis << P;
        }

        for(n = orthonormalBasis.size(); n <= maxDegree; n++)
        {
            Polynomial P(n);

            for(int i = 0 ; i < orthonormalBasis.size(); i++)
                P -= continuousScalarProduct(P, orthonormalBasis.at(i), min, max)*orthonormalBasis.at(i);

            P *= 1/continuousNorm(P, xmin, xmax);
            orthonormalBasis << P;
        }
    }
    else if(approxMethod == ApproachPoints)
    {
        if(orthonormalBasis.isEmpty())
        {
            Polynomial P(0);
            P *= 1/discreteNorm(P, dataPoints);
            orthonormalBasis << P;
        }

        for(n = orthonormalBasis.size(); n <= maxDegree; n++)
        {
            Polynomial P(n);

            for(int i = 0 ; i < orthonormalBasis.size(); i++)
                P -= discreteScalarProduct(P, orthonormalBasis.at(i), dataPoints) * orthonormalBasis.at(i);

            P *= 1/discreteNorm(P, dataPoints);
            orthonormalBasis << P;
        }
    }
}

double discreteScalarProduct(const QList<Point> &data, const Polynomial &P)
{
    double res = 0;

    for(int i = 0 ; i < data.size() ; i++)
        res += data[i].y * P.eval(data[i].x);

    return res;

}

double continuousScalarProduct(const QList<Point> &data, const Polynomial &P)
{
    double res = 0;
    Polynomial segment, prod;

    for(int i = 0 ; i < data.size() - 1 ; i++)
    {
        segment.setAffine(data[i], data[i+1]);
        prod = segment * P;
        res = prod.eval(data[i+1].x) - prod.eval(data[i].x);
    }

    return res;
}

double continuousNorm(const Polynomial &P, double xmin, double xmax)
{
    Polynomial Q = (P*P).antiderivative();

    return sqrt(Q.eval(xmax) - Q.eval(xmin));
}

double discreteNorm(const Polynomial &P, const QList<Point> &data)
{
    Polynomial Q = P*P;
    double res = 0;

    for(int i = 0 ; i < data.size() ; i++)
        res += Q.eval(data[i].x);

    return sqrt(res);
}

PolynomialRegression::~PolynomialRegression()
{

}

