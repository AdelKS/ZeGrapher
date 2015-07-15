#include "polynomialmodelwidget.h"
#include "ui_polynomialmodelwidget.h"

PolynomialModelWidget::PolynomialModelWidget(const QList<Point> &dat, Informations *info, QString xname, QString yname, QWidget *parent) :
    QWidget(parent), ui(new Ui::PolynomialModelWidget)
{
    ui->setupUi(this);

    abscissa = xname;
    ordinate = yname; //of course the abscissa and ordinate would mean the polar angle and radius respectively
    updateDescriptionText();

    data = dat;
    informations = info;
}

void PolynomialModelWidget::updateDescriptionText()
{
    ui->description->setText(tr("calcul du meilleur polynôme P approchant l'égalité:") + "\n" + " \" " + ordinate + " = " + "P( " + abscissa + " ) \" ");
}

void PolynomialModelWidget::setAbscissaName(QString name)
{
    abscissa = name;
    updateDescriptionText();
}

void PolynomialModelWidget::setOrdinateName(QString name)
{
    ordinate = name;
    updateDescriptionText();
}

void PolynomialModelWidget::setData(const QList<Point> &dat)
{
    data = dat;
}

void PolynomialModelWidget::setPolar(bool pol)
{
    polar = pol;
}

PolynomialModelWidget::~PolynomialModelWidget()
{
    delete ui;
}
