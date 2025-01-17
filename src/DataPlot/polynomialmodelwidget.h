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


#ifndef POLYNOMIALMODELWIDGET_H
#define POLYNOMIALMODELWIDGET_H

#include <QWidget>

#include "Widgets/numberlineedit.h"
#include "Widgets/qcolorbutton.h"
#include "Calculus/polynomialregression.h"
#include "Calculus/regressionvaluessaver.h"

namespace Ui {
class PolynomialModelWidget;
}

struct CoefWidgetStruct
{
    QWidget *container;
    QLabel *name;
    QLineEdit *line;
};

class PolynomialModelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PolynomialModelWidget(const std::weak_ptr<const UserData> &userData,
                                   QString xname, QString yname,
                                   QWidget *parent = nullptr);
    void setAbscissaName(QString name);
    void setOrdinateName(QString name);
    void refreshModel();
    ~PolynomialModelWidget();

signals:
    void removeMe();
    void regressionEdited();

protected slots:
    void updateApproxMethod();
    void updateRangeOption();
    void updatePolynomialCoefs(QList<double> coefs);
    void updateManualRangeFields();
    void manualRangeEdited();

protected:
    void addWidgetsToUI();
    void refreshDataPoints();

    Ui::PolynomialModelWidget *ui;
    QString abscissa, ordinate;
    std::weak_ptr<const UserData> userData;
    NumberLineEdit *startVal, *endVal;
    QColorButton *colorButton;

    QList<CoefWidgetStruct> coefWidgets;

    PolynomialRegression *regression;
};

#endif // POLYNOMIALMODELWIDGET_H
