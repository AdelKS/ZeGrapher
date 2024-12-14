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





#ifndef POLYNOMIALREGRESSION_H
#define POLYNOMIALREGRESSION_H

#include "polynomial.h"
#include "regression.h"
#include <QList>

enum ApproxMethod { ApproachPoints = true, ApproachSegments = false};
enum DrawRange {Manual, LimitedToData, RelativeExtrapolation};

// TODO: Remove dependence on Qt

class PolynomialRegression : public Regression
{
    Q_OBJECT

public:
    explicit PolynomialRegression(const std::weak_ptr<const UserData> &userData,
                                  int polynomialDegree, ApproxMethod method,
                                  DrawRange drawRange, double rangecoef, bool draw);
    ~PolynomialRegression();

    double eval(double x) const;
    QString getInfo() const;

    void refresh();
    void setDrawRangeCalculusMethod(DrawRange option);
    void setRange(Range rg);
    bool isPolar();

    void setApproxMethod(ApproxMethod method);
    void setRelativeRangeCoef(double coef);
    void setPolynomialRegressionDegree(int deg);


protected:
    void updateNormalisedData();
    void updateMinMax();
    void updateOrthonormalBasis();
    void updateDrawRange();
    void recalculateOrthonormalBasis();
    void calculateRegressionPolynomials();
    void normaliseData();

    std::weak_ptr<const UserData> userData;

    uint regressionDegree;

    DrawRange rangeOption;
    double rangeCoef;

    Polynomial continuousPol, discretePol;
    ApproxMethod approxMethod;
    double xmin, xmax, xamp, ymin, ymax, yamp; //integration segment for dot product: integrate between min and max
    std::vector<Polynomial> orthonormalBasisDiscrete, orthonormalBasisContinuous;
    std::vector<Point> normalisedData;
};

double discreteScalarProduct(const Polynomial &P, const std::vector<Point> &data);
double continuousScalarProduct(const Polynomial &P, const std::vector<Point> &data);

double continuousNorm(const Polynomial &P, double xmin, double xmax);
double discreteNorm(const Polynomial &P, const std::vector<Point> &data);

#endif // POLYNOMIALREGRESSION_H
