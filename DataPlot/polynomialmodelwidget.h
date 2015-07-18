#ifndef POLYNOMIALMODELWIDGET_H
#define POLYNOMIALMODELWIDGET_H

#include <QWidget>

#include "Structures.h"
#include "informations.h"
#include "Widgets/numberlineedit.h"
#include "Widgets/qcolorbutton.h"
#include "Calculus/polynomialregression.h"
#include "Calculus/regressionvaluessaver.h"

namespace Ui {
class PolynomialModelWidget;
}

class PolynomialModelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PolynomialModelWidget(const QList<Point> &dat, Informations *info, QString xname, QString yname, bool pol, QWidget *parent = 0);
    void setAbscissaName(QString name);
    void setOrdinateName(QString name);
    void setData(const QList<Point> &dat);
    void setPolar(bool pol);
    ~PolynomialModelWidget();

signals:
    void removeMe();
    void regressionEdited();

protected slots:
    void updateApproxMethod();
    void updateRangeOption();
    void updatePolynomialCoefs();

protected:
    void addWidgetsToUI();
    void updateDescriptionText();

    Ui::PolynomialModelWidget *ui;
    QString abscissa, ordinate;
    Informations *informations;
    bool polar;
    QList<Point> data;
    NumberLineEdit *startVal, *endVal;
    QColorButton *colorButton;

    PolynomialRegression *regression;
    RegressionValuesSaver *regValSaver;
};

#endif // POLYNOMIALMODELWIDGET_H
