/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "polynomial.h"

Polynomial::Polynomial(const Polynomial &pol) : translation(pol.translation), coefficients(pol.coefficients),
    translatedCoefficients(pol.translatedCoefficients)
{
}

Polynomial::Polynomial(int monicMonomialDegree) : translation(0)
{
    for(int i = 0 ; i < monicMonomialDegree ; i++)
        coefficients << 0;

    coefficients << 1;
}

Polynomial::Polynomial(QList<double> coefs) : translation(0)
{
    coefficients = coefs;
}

QList<double> Polynomial::getCoefs()
{
    return coefficients;
}

QList<double> Polynomial::getTranslatedCoefs()
{
    if(translation == 0)
        return coefficients;
    else return translatedCoefficients;
}

Polynomial::Polynomial()
{
    coefficients << 0;
}

void Polynomial::resetToZero()
{
    coefficients.clear();
    coefficients << 0;
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
    translation += -Dx;

    translatedCoefficients.clear();
    
    Dx = -Dx;

    QList<double> DxPowTable;
    DxPowTable.reserve(coefficients.size());

    double DxPow = 1;

    for(int i = 0 ; i < coefficients.size() ; i++)
    {
        translatedCoefficients << 0;
        DxPowTable << DxPow;
        DxPow *= Dx;
    }

    for(int n = 0 ; n < coefficients.size() ; n++)
        for(int k = 0 ; k <= n ; k++)
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
    QList<double> coefs = coefficients;
    coefs.push_front(0);

    double val = 1;
    for(int i = 1 ; i < coefs.size() ; i++)
    {
        coefs[i] /= val;
        val++;
    }

    Polynomial pol(coefs);
    pol.translateX(translation);

    return pol;
}

void Polynomial::setAffine(Point A, Point B)
{
    translation = 0;
    translatedCoefficients.clear();

    coefficients.clear();
    coefficients << 0 << 0;

    coefficients[1] = (B.y - A.y) / (B.x - A.x);
    coefficients[0] = A.y - coefficients[1] * A.x;
}

Polynomial& Polynomial::operator+=(const Polynomial &P)
{
    QList<double> coefs;
    int deg = std::max(degree(), P.degree());

    if(P.getXTranslation() != 0 || translation != 0)
    {
        double meanTranslation = (P.getXTranslation() + translation)/2;
        Polynomial Q(P);
        Q.translateX(Q.translation - meanTranslation);
        translateX(translation - meanTranslation);
    }

    for(int i = 0 ; i <= deg ; i++)
        coefs << getCoef(i) + P.getCoef(i);

    coefficients = coefs;
    translation = 0;

    return *this;
}

Polynomial& Polynomial::operator*=(double scal)
{
    for(int i = 0 ; i < coefficients.size() ; i++)
        coefficients[i] *= scal;

    for(int i = 0 ; i < translatedCoefficients.size() ; i++)
        translatedCoefficients[i] *= scal;

    return *this;
}

Polynomial& Polynomial::operator*=(const Polynomial &P)
{
    Polynomial Q(P);
    QList<double> coefs;
    int deg = degree() + P.degree();

    for(int i = 0 ; i <=  deg ; i++)
        coefs << 0;

    if(P.getXTranslation() != 0 || translation != 0)
    {
        double meanTranslation = (P.getXTranslation() + translation)/2;
        Q.translateX(meanTranslation - Q.translation);
        translateX(meanTranslation - translation);
    }

    for(int i = 0 ; i <= degree() ; i++)
        for(int j = 0 ; j <= P.degree() ; j++)
            coefs[i+j] += getCoef(i) * Q.getCoef(j);

    coefficients = coefs;
    translation = 0;

    return *this;
}

Polynomial& Polynomial::operator-=(const Polynomial &P)
{
    Polynomial Q(P);
    Q *= -1;

    operator +=(Q);

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

double discreteScalarProduct(const Polynomial &P, const Polynomial &Q, const QList<Point> &data)
{
    Polynomial D(P);
    D *= Q;

    double res = 0;
    for(int i = 0 ; i < data.size() ; i++)
        res += D.eval(data[i].x);

    return res;
}

