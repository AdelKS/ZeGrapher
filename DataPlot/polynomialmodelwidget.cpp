#include "polynomialmodelwidget.h"
#include "ui_polynomialmodelwidget.h"

PolynomialModelWidget::PolynomialModelWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PolynomialModelWidget)
{
    ui->setupUi(this);
}

PolynomialModelWidget::~PolynomialModelWidget()
{
    delete ui;
}
