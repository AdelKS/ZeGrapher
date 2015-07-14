#ifndef POLYNOMIALMODELWIDGET_H
#define POLYNOMIALMODELWIDGET_H

#include <QWidget>

namespace Ui {
class PolynomialModelWidget;
}

class PolynomialModelWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PolynomialModelWidget(QWidget *parent = 0);
    ~PolynomialModelWidget();

private:
    Ui::PolynomialModelWidget *ui;
};

#endif // POLYNOMIALMODELWIDGET_H
