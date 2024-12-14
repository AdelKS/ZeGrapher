/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
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

PolynomialRegression::PolynomialRegression(const std::weak_ptr<const UserData> &userData,
                                           int polynomialDegree, ApproxMethod method,
                                           DrawRange drawRange, double rangecoef, bool draw) :
    Regression(), userData(userData), regressionDegree(polynomialDegree), rangeOption(drawRange),
    rangeCoef(rangecoef), approxMethod(method)
{
    drawState = draw;
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

void PolynomialRegression::refresh()
{

    if(userData.lock()->dataPoints.size() <= 1)
    {
        valid = false;
        return;
    }

    valid = true;

    updateMinMax();

    updateNormalisedData();

    updateDrawRange();

    recalculateOrthonormalBasis();
    calculateRegressionPolynomials();

    emit regressionModified();
}

void PolynomialRegression::updateNormalisedData()
{
    normalisedData.clear();
    normalisedData.reserve(userData.lock()->dataPoints.size());

    for(const auto &point : userData.lock()->dataPoints)
    {
        normalisedData.emplace_back(Point({2*(point.x - xmin)/xamp - 1, 2*(point.y - ymin)/yamp - 1}));
    }

}

void PolynomialRegression::updateMinMax()
{
    auto &&minmax_x = std::minmax_element(userData.lock()->dataPoints.cbegin(), userData.lock()->dataPoints.cend());
    xmin = minmax_x.first->x;
    xmax = minmax_x.second->x;
    xamp = xmax - xmin;

    auto &&minmax_y = std::minmax_element(userData.lock()->dataPoints.cbegin(), userData.lock()->dataPoints.cend(), ptCompY);
    ymin = minmax_y.first->y;
    ymax = minmax_y.second->y;
    yamp = ymax - ymin;
}

void PolynomialRegression::updateDrawRange()
{
    if(rangeOption == LimitedToData)
    {
        range.start = xmin;
        range.end = xmax;
        emit rangeUpdated();
    }
    else if(rangeOption == RelativeExtrapolation)
    {
        double amplitude = xmax - xmin;
        range.start = xmin - amplitude * rangeCoef;
        range.end = xmax + amplitude * rangeCoef;
        emit rangeUpdated();
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

    emit regressionModified();
}

bool PolynomialRegression::isPolar()
{
    return !userData.lock()->cartesian;
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

    for(uint n = 0 ; n < orthonormalBasisContinuous.size() && n <= regressionDegree ; n++)
         continuousPol += continuousScalarProduct(orthonormalBasisContinuous.at(n), normalisedData) * orthonormalBasisContinuous.at(n);
    for(uint n = 0 ; n < orthonormalBasisDiscrete.size() && n <= regressionDegree ; n++)
        discretePol += discreteScalarProduct(orthonormalBasisDiscrete.at(n), normalisedData) * orthonormalBasisDiscrete.at(n);

    continuousPol.translateX(1);
    continuousPol.expand(xamp/2);
    continuousPol.translateX(xmin);

    continuousPol.translateY(1);
    continuousPol *= yamp/2;
    continuousPol.translateY(ymin);

    discretePol.translateX(1);
    discretePol.expand(xamp/2);
    discretePol.translateX(xmin);

    discretePol.translateY(1);
    discretePol *= yamp/2;
    discretePol.translateY(ymin);
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
        if(orthonormalBasisContinuous.empty())
        {
            Polynomial P(0);
            P *= 1/continuousNorm(P, -1, 1);
            orthonormalBasisContinuous.emplace_back(std::move(P));
        }

        for(uint n = orthonormalBasisContinuous.size(); n <= regressionDegree ; n++)
        {
            Polynomial P(n);

            for(uint i = 0 ; i < orthonormalBasisContinuous.size(); i++)
                P -= continuousScalarProduct(P, orthonormalBasisContinuous.at(i), -1, 1) * orthonormalBasisContinuous.at(i);

            P *= 1/continuousNorm(P, -1, 1);
            orthonormalBasisContinuous.emplace_back(std::move(P));
        }
    }
    if(regressionDegree > orthonormalBasisDiscrete.size() - 1)
    {
        if(orthonormalBasisDiscrete.empty())
        {
            Polynomial P(0);
            P *= 1/discreteNorm(P, normalisedData);
            orthonormalBasisDiscrete.emplace_back(std::move(P));
        }

        for(uint n = orthonormalBasisDiscrete.size(); n <= regressionDegree && n < normalisedData.size(); n++)
        {
            Polynomial P(n);

            for(uint i = 0 ; i < orthonormalBasisDiscrete.size(); i++)
                P -= discreteScalarProduct(P, orthonormalBasisDiscrete.at(i), normalisedData) * orthonormalBasisDiscrete.at(i);

            P *= 1/discreteNorm(P, normalisedData);
            orthonormalBasisDiscrete.emplace_back(std::move(P));
        }
    }
}

double discreteScalarProduct(const Polynomial &P, const std::vector<Point> &data)
{
    double res = 0;

    for(const Point &pt: data)
        res += pt.y * P.eval(pt.x);

    return res;
}

double continuousScalarProduct(const Polynomial &P, const std::vector<Point> &data)
{
    double res = 0;
    Polynomial segment, prod;

    for(uint i = 0 ; i < data.size() - 1 ; i++)
    {
        segment.setAffine(data[i], data[i+1]);
        prod = (segment * P).antiderivative();
        res += prod.eval(data[i+1].x) - prod.eval(data[i].x);
    }

    return res;
}

double continuousNorm(const Polynomial &P, double xmin, double xmax)
{
    Polynomial Q = (P*P).antiderivative();

    return sqrt(Q.eval(xmax) - Q.eval(xmin));
}

double discreteNorm(const Polynomial &P, const std::vector<Point> &data)
{
    double res = 0, tmp;

    for(const Point &pt: data)
    {
        tmp = P.eval(pt.x);
        res += tmp * tmp;
    }

    return sqrt(res);
}

PolynomialRegression::~PolynomialRegression()
{

}
