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

#include "polynomial.h"

#include <boost/math/special_functions/binomial.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

Polynomial::Polynomial(int monicMonomialDegree) : translation(0)
{
    coefficients.assign(monicMonomialDegree+1, 0);

    coefficients[monicMonomialDegree] = 1;
}

Polynomial::Polynomial(std::vector<double> &&coefs) : translation(0), coefficients(std::move(coefs))
{

}

const std::vector<double> &Polynomial::getCoefs()
{
    return coefficients;
}

const std::vector<double> &Polynomial::getTranslatedCoefs()
{
    if(translation == 0)
        return coefficients;
    else return translatedCoefficients;
}

Polynomial::Polynomial() : coefficients({0})
{
}

void Polynomial::resetToZero()
{
    translatedCoefficients.clear();
    translation = 0;
    coefficients.clear();
    coefficients.push_back(0);
}

double Polynomial::eval(double xval) const
{
    double x = xval + translation;

    double val = 1, res = 0;

    for(int i = 0 ; i <= degree() ; i++)
    {
        res += val * coefficients[i];
        val *= x;
    }

    return res;
}

double Polynomial::getXTranslation() const
{
    return translation;
}

void Polynomial::translateX(double Dx)
{
    translation -= Dx;

    translatedCoefficients.clear();

    std::vector<double> DxPowTable;
    DxPowTable.reserve(coefficients.size());

    double DxPow = 1;

    for(uint i = 0 ; i < coefficients.size() ; i++)
    {
        translatedCoefficients.push_back(0);
        DxPowTable.push_back(DxPow);
        DxPow *= translation;
    }

    for(uint n = 0 ; n < coefficients.size() ; n++)
        for(uint k = 0 ; k <= n ; k++)
            translatedCoefficients[k] += coefficients[n] * boost::math::binomial_coefficient<double>(n,k) * DxPowTable[n-k];
}

void Polynomial::translateY(double Dy)
{
    coefficients[0] += Dy;
    translatedCoefficients[0] += Dy;
}

void Polynomial::expand(double coef)
{
    double multCoef = 1;

    for(auto &c : coefficients)
    {
        c *= multCoef;
        multCoef /= coef;
    }

    multCoef = 1;

    translation *= coef;

    for(auto &c : translatedCoefficients)
    {
        c *= multCoef;
        multCoef /= coef;
    }
}

double Polynomial::getCoef(int deg) const
{
    if(deg <= degree())
    {
        if(translation != 0)
            return translatedCoefficients[deg];
        else return coefficients[deg];
    }
    else return 0;
}

int Polynomial::degree() const
{
    return coefficients.size() - 1;
}

Polynomial Polynomial::antiderivative()
{
    std::vector<double> coefs;
    coefs.reserve(coefficients.size()+1);
    coefs.push_back(0);

    for(const double &val: coefficients)
        coefs.push_back(val);

    double val = 1;
    for(uint i = 1 ; i < coefs.size() ; i++)
    {
        coefs[i] /= val;
        val++;
    }

    Polynomial pol(std::move(coefs));
    pol.translateX(translation);

    return pol;
}

void Polynomial::setAffine(Point A, Point B)
{
    translation = 0;
    translatedCoefficients.clear();

    coefficients.clear();
    coefficients.assign(2, 0);

    coefficients[1] = (B.y - A.y) / (B.x - A.x);
    coefficients[0] = A.y - coefficients[1] * A.x;
}

Polynomial& Polynomial::operator+=(const Polynomial &P)
{
    int deg = std::max(degree(), P.degree());
    coefficients.resize(deg, 0);

    double translation_diff = translation - P.getXTranslation();
    if(translation_diff != 0)
        translateX(translation_diff);

    for(int i = 0 ; i <= deg ; i++)
        coefficients[i] += P.getCoef(i);

    return *this;
}

Polynomial& Polynomial::operator*=(double scal)
{
    for(uint i = 0 ; i < coefficients.size() ; i++)
        coefficients[i] *= scal;

    for(uint i = 0 ; i < translatedCoefficients.size() ; i++)
        translatedCoefficients[i] *= scal;

    return *this;
}

Polynomial& Polynomial::operator*=(const Polynomial &P)
{
    std::vector<double> coefs;
    int deg = degree() + P.degree();
    coefs.assign(deg, 0);

    double translation_diff = translation - P.getXTranslation();
    if(translation_diff != 0)
        translateX(translation_diff);

    for(int i = 0 ; i <= degree() ; i++)
        for(int j = 0 ; j <= P.degree() ; j++)
            coefs[i+j] += coefficients[i] * P.getCoef(j);

    coefficients = std::move(coefs);
    return *this;
}

Polynomial& Polynomial::operator-=(const Polynomial &P)
{
    int deg = std::max(degree(), P.degree());
    coefficients.resize(deg, 0);

    double translation_diff = translation - P.getXTranslation();
    if(translation_diff != 0)
        translateX(translation_diff);

    for(int i = 0 ; i <= deg ; i++)
        coefficients[i] -= P.getCoef(i);

    return *this;
}

Polynomial::~Polynomial()
{

}

Polynomial operator+(const Polynomial &P, const Polynomial &Q)
{
    Polynomial D(P);
    D += Q;
    return D;
}

Polynomial operator-(const Polynomial &P, const Polynomial &Q)
{
    Polynomial D(P);
    D -= Q;
    return D;
}

Polynomial operator*(const Polynomial &P, const Polynomial &Q)
{
    Polynomial D(P);
    D *= Q;
    return D;
}

Polynomial operator*(double scal, const Polynomial &Q)
{
    Polynomial D(Q);
    D *= scal;
    return D;
}

double continuousScalarProduct(const Polynomial &P, const Polynomial &Q, double a, double b)
{
    Polynomial D(P * Q);

    D = D.antiderivative();

    return D.eval(b) - D.eval(a);
}

double discreteScalarProduct(const Polynomial &P, const Polynomial &Q, const std::vector<Point> &data)
{
    Polynomial D(P * Q);

    double res = 0;
    for(const Point &pt: data)
        res += D.eval(pt.x);

    return res;
}
