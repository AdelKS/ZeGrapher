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
    void updatePolynomialCoefs(QList<double> coefs);

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

    QList<CoefWidgetStruct> coefWidgets;

    PolynomialRegression *regression;
    RegressionValuesSaver *regValSaver;
};

#endif // POLYNOMIALMODELWIDGET_H
