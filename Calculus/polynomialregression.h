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
enum DrawRange {Manual, LimitedToData, RelativeExtrapolation};

class PolynomialRegression : public Regression
{
    Q_OBJECT

public:
    explicit PolynomialRegression(int polynomialDegree, ApproxMethod method, DrawRange drawRange, double rangecoef, bool draw, bool isPolar);
    ~PolynomialRegression();

    double eval(double x) const;
    QString getInfo() const;

    void setData(const QList<Point> &data);
    void setDrawRangeCalculusMethod(DrawRange option);
    void setRange(Range rg);

signals:
    void coefsUpdated(QList<double> coefs);

public slots:
    void setApproxMethod(ApproxMethod method);
    void setRelativeRangeCoef(double coef);
    void setPolynomialRegressionDegree(int deg);  


protected:
    void updateOrthonormalBasis();
    void updateDrawRange();
    void recalculateOrthonormalBasis();
    void calculateRegressionPolynomials();

    int regressionDegree;

    DrawRange rangeOption;
    double rangeCoef;

    Polynomial continuousPol, discretePol;
    ApproxMethod approxMethod;
    double xmin, xmax; //integration segment for dot product: integrate between min and max
    QList<Point> dataPoints;
    QList<Polynomial> orthonormalBasisDiscrete, orthonormalBasisContinuous;
};

double discreteScalarProduct(const QList<Point> &data, const Polynomial &P);
double continuousScalarProduct(const QList<Point> &data, const Polynomial &P);

double continuousNorm(const Polynomial &P, double xmin, double xmax);
double discreteNorm(const Polynomial &P, const QList<Point> &data);

#endif // POLYNOMIALREGRESSION_H
