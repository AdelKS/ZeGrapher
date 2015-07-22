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

#include "polynomial.h"

Polynomial::Polynomial(const Polynomial &pol) : coefficients(pol.coefficients)
{

}

Polynomial::Polynomial(int monicMonomialDegree)
{
    for(int i = 0 ; i < monicMonomialDegree ; i++)
        coefficients << 0;

    coefficients << 1;
}

Polynomial::Polynomial(QList<double> coefs)
{
    coefficients = coefs;
}

QList<double> Polynomial::getCoefs()
{
    return coefficients;
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

double Polynomial::eval(double x) const
{
    double val = 1, res = 0;

    for(int i = 0 ; i <= degree() ; i++)
    {
        res += val * coefficients[i];
        val *= x;
    }

    return res;
}

double Polynomial::getCoef(int deg) const
{
    if(deg <= degree())
        return coefficients[deg];
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

    return Polynomial(coefs);
}

void Polynomial::setAffine(Point A, Point B)
{
    coefficients.clear();
    coefficients << 0 << 0;

    coefficients[1] = (B.y - A.y) / (B.x - A.x);
    coefficients[0] = A.y - coefficients[1] * A.x;
}

Polynomial& Polynomial::operator+=(const Polynomial &P)
{
    QList<double> coefs;
    int deg = std::max(degree(), P.degree());

    for(int i = 0 ; i <= deg ; i++)
        coefs << getCoef(i) + P.getCoef(i);

    coefficients = coefs;

    return *this;
}

Polynomial& Polynomial::operator*=(double scal)
{
    for(int i = 0 ; i < coefficients.size() ; i++)
        coefficients[i] *= scal;

    return *this;
}

Polynomial& Polynomial::operator*=(const Polynomial &P)
{
    QList<double> coefs;
    int deg = degree() + P.degree();

    for(int i = 0 ; i <=  deg ; i++)
        coefs << 0;

    for(int i = 0 ; i <= degree() ; i++)
        for(int j = 0 ; j <= P.degree() ; j++)
            coefs[i+j] += coefficients[i] * P.coefficients[j];

    coefficients = coefs;

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

