#ifndef POLYNOMIALMODELWIDGET_H
#define POLYNOMIALMODELWIDGET_H

#include <QWidget>

#include "Structures.h"
#include "informations.h"

namespace Ui {
class PolynomialModelWidget;
}

class PolynomialModelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PolynomialModelWidget(const QList<Point> &dat, Informations *info, QString xname, QString yname, QWidget *parent = 0);
    void setAbscissaName(QString name);
    void setOrdinateName(QString name);
    void setData(const QList<Point> &dat);
    void setPolar(bool pol);
    ~PolynomialModelWidget();

signals:
    void removeMe();

protected:
    void updateDescriptionText();

    Ui::PolynomialModelWidget *ui;
    QString abscissa, ordinate;
    Informations *informations;
    bool polar;
    QList<Point> data;
};

#endif // POLYNOMIALMODELWIDGET_H
