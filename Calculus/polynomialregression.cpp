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

PolynomialRegression::PolynomialRegression(int polynomialDegree, ApproxMethod method, DrawRange drawRange, double rangecoef, bool draw, bool isPolar) : Regression()
{
    approxMethod = method;
    regressionDegree = polynomialDegree;
    rangeOption = drawRange;
    rangeCoef = rangecoef;
    drawState = draw;
    polar = isPolar;

    range.step = 1; //this variable is useless in this class
}

double PolynomialRegression::eval(double x) const
{
    if(approxMethod == ApproachPoints)
        return discretePol.eval(x);
    else return continuousPol.eval(x);
}

QString PolynomialRegression::getInfo() const
{
    return QString(); // need to be implemented
}

void PolynomialRegression::setData(const QList<Point> &data)
{
    if(data.size() <= 1)
    {
        valid = false;
        return;
    }

    valid = true;

    dataPoints = data;
    qSort(dataPoints);

    xmin = dataPoints.first().x;
    xmax = dataPoints.last().x;

    updateDrawRange();

    recalculateOrthonormalBasis();
    calculateRegressionPolynomials();
}

void PolynomialRegression::updateDrawRange()
{
    if(rangeOption == LimitedToData)
    {
        range.start = xmin;
        range.end = xmax;        
    }
    else if(rangeOption == RelativeExtrapolation)
    {
        double amplitude = xmax - xmin;
        range.start = xmin - amplitude * rangeCoef;
        range.end = xmax + amplitude * rangeCoef;
    }
}

void PolynomialRegression::setDrawRangeCalculusMethod(DrawRange option)
{
    rangeOption = option;
    updateDrawRange();

    emit regressionModified();
}

void PolynomialRegression::setRange(Range rg)
{
    range = rg;
}

void PolynomialRegression::setRelativeRangeCoef(double coef)
{
    rangeCoef = coef;
    updateDrawRange();

    emit regressionModified();
}

void PolynomialRegression::calculateRegressionPolynomials()
{
    continuousPol.resetToZero();
    discretePol.resetToZero();

    for(int n = 0 ; n <= regressionDegree ; n++)
    {
         continuousPol += continuousScalarProduct(dataPoints, orthonormalBasisContinuous.at(n)) * orthonormalBasisContinuous.at(n);
         discretePol += discreteScalarProduct(dataPoints, orthonormalBasisDiscrete.at(n)) * orthonormalBasisDiscrete.at(n);
    }
}

void PolynomialRegression::setPolynomialRegressionDegree(int deg)
{
    regressionDegree = deg;
    updateOrthonormalBasis();
    calculateRegressionPolynomials();

    emit regressionModified();
}

void PolynomialRegression::setApproxMethod(ApproxMethod method)
{
    approxMethod = method;

    emit regressionModified();
}

void PolynomialRegression::recalculateOrthonormalBasis()
{
    orthonormalBasisContinuous.clear();
    orthonormalBasisDiscrete.clear();

    updateOrthonormalBasis();
}

void PolynomialRegression::updateOrthonormalBasis()
{
    //we use Gram-Schmidt orthonormalisation process

    if(regressionDegree > orthonormalBasisContinuous.size() - 1)
    {
        if(orthonormalBasisContinuous.isEmpty())
        {
            Polynomial P(0);
            P *= 1/continuousNorm(P, xmin, xmax);
            orthonormalBasisContinuous << P;
        }

        for(int n = orthonormalBasisContinuous.size(); n <= regressionDegree; n++)
        {
            Polynomial P(n);

            for(int i = 0 ; i < orthonormalBasisContinuous.size(); i++)
                P -= continuousScalarProduct(P, orthonormalBasisContinuous.at(i), xmin, xmax) * orthonormalBasisContinuous.at(i);

            P *= 1/continuousNorm(P, xmin, xmax);
            orthonormalBasisContinuous << P;
        }
    }
    if(regressionDegree > orthonormalBasisDiscrete.size() - 1)
    {
        if(orthonormalBasisDiscrete.isEmpty())
        {
            Polynomial P(0);
            P *= 1/discreteNorm(P, dataPoints);
            orthonormalBasisDiscrete << P;
        }

        for(int n = orthonormalBasisDiscrete.size(); n <= regressionDegree; n++)
        {
            Polynomial P(n);

            for(int i = 0 ; i < orthonormalBasisDiscrete.size(); i++)
                P -= discreteScalarProduct(P, orthonormalBasisDiscrete.at(i), dataPoints) * orthonormalBasisDiscrete.at(i);

            P *= 1/discreteNorm(P, dataPoints);
            orthonormalBasisDiscrete << P;
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

