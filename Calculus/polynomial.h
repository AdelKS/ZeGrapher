/****************************************************************************
**  Copyright (c) 2016, Adel Kara Slimane <adel.ks@zegrapher.com>
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





#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <QList>
#include <algorithm>
#include "structures.h"


class Polynomial
{

public:
    Polynomial(const Polynomial &pol);    
    Polynomial(int monicMonomialDegree);
    Polynomial(QList<double> coefs);
    Polynomial();    

    void translateX(double Dx);
    void translateY(double Dy);
    void expand(double coef);
    void resetToZero();

    double eval(double xval) const;

    double getCoef(int degree) const;
    double getTranslatedCoef(int degree) const;
    double getXTranslation() const;

    QList<double> getCoefs();
    QList<double> getTranslatedCoefs();
    int degree() const;
    Polynomial antiderivative();

    void setAffine(Point A, Point B);

    Polynomial& operator+=(const Polynomial &P);
    Polynomial& operator*=(double scal);
    Polynomial& operator*=(const Polynomial &P);
    Polynomial& operator-=(const Polynomial &P);

    ~Polynomial();

protected:
    double translation;
    QList<double> coefficients, translatedCoefficients;

};

Polynomial operator+(const Polynomial &P, const Polynomial &Q);
Polynomial operator-(const Polynomial &P, const Polynomial &Q);
Polynomial operator*(const Polynomial &P, const Polynomial &Q);
Polynomial operator*(double scal, const Polynomial &Q);

double continuousScalarProduct(const Polynomial &P, const Polynomial &Q, double a, double b);
double discreteScalarProduct(const Polynomial &P, const Polynomial &Q, const QList<Point> &data);


#endif // POLYNOMIAL_H
