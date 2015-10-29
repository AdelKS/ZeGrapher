/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "polynomialmodelwidget.h"
#include "ui_polynomialmodelwidget.h"

PolynomialModelWidget::PolynomialModelWidget(const QList<Point> &dat, Informations *info, QString xname, QString yname, bool pol, QWidget *parent) :
    QWidget(parent), ui(new Ui::PolynomialModelWidget)
{
    ui->setupUi(this);

    abscissa = xname;
    ordinate = yname; //of course the abscissa and ordinate would mean the polar angle and radius respectively
    updateDescriptionText();

    polar = pol;
    data = dat;
    informations = info;

    addWidgetsToUI();

    regression = new PolynomialRegression(ui->degree->value(), (ApproxMethod)ui->approachPoints->isChecked(), LimitedToData,
                                          ui->relativeExtrapol->value(), ui->drawModel->isChecked(), pol);
    connect(regression, SIGNAL(rangeUpdated()), this, SLOT(updateManualRangeFields()));

    connect(regression, SIGNAL(coefsUpdated(QList<double>)), this, SLOT(updatePolynomialCoefs(QList<double>)));


    regValSaver = new RegressionValuesSaver(regression, informations->getOptions(), informations->getRange(), informations->getUnits());

    connect(regression, SIGNAL(regressionModified()), regValSaver, SLOT(recalculate()));
    connect(regression, SIGNAL(regressionModified()), informations, SIGNAL(dataUpdated()));

    informations->addDataRegression(regValSaver);

    regression->setData(dat); //

    connect(ui->drawModel, SIGNAL(toggled(bool)), regression, SLOT(setDrawState(bool)));
    connect(colorButton, SIGNAL(colorChanged(QColor)), regression, SLOT(setColor(QColor)));
    connect(ui->degree, SIGNAL(valueChanged(int)), regression, SLOT(setPolynomialRegressionDegree(int)));
    connect(ui->relativeExtrapol, SIGNAL(valueChanged(double)), regression, SLOT(setRelativeRangeCoef(double)));

    connect(ui->approachPoints, SIGNAL(toggled(bool)), this, SLOT(updateApproxMethod()));
    connect(ui->dataInterval, SIGNAL(clicked(bool)), this, SLOT(updateRangeOption()));
    connect(ui->relativeExtrapolation, SIGNAL(clicked(bool)), this, SLOT(updateRangeOption()));
    connect(ui->manualInterval, SIGNAL(clicked(bool)), this, SLOT(updateRangeOption()));
}

void PolynomialModelWidget::manualRangeEdited()
{
    if(startVal->isValid() && endVal->isValid())
    {
        Range range;
        range.step = 1;
        range.start = startVal->getValue();
        range.end = endVal->getValue();

        regression->setRange(range);
    }
}

void PolynomialModelWidget::updatePolynomialCoefs(QList<double> coefs)
{
    for(int pos = 0 ; pos < coefs.size() && pos < coefWidgets.size() ; pos++)
    {
        coefWidgets[pos].container->show();
        coefWidgets[pos].line->setText(QString::number(coefs[pos]));
    }

    for(int pos = coefWidgets.size()-1 ; pos > coefs.size()-1 ; pos--)
    {
        coefWidgets[pos].container->hide();
    }

    for(int pos = coefWidgets.size() ; pos < coefs.size() ; pos++)
    {
        CoefWidgetStruct coefWidget;
        coefWidget.container = new QWidget();
        coefWidget.name = new QLabel("a<sub>" + QString::number(pos) + "</sub> =");
        coefWidget.line = new QLineEdit();

        coefWidget.line->setText(QString::number(coefs[pos]));
        coefWidget.line->setMaximumHeight(25);
        coefWidget.line->setReadOnly(true);
        coefWidget.line->setAlignment(Qt::AlignLeft);

        QHBoxLayout *layout = new QHBoxLayout();
        layout->setMargin(3);

        layout->addWidget(coefWidget.name);
        layout->addWidget(coefWidget.line);

        coefWidget.container->setLayout(layout);

        ui->coefWidgetsLayout_2->addWidget(coefWidget.container);

        coefWidgets << coefWidget;
    }

    QString genericExpr = "P = a<sub>0</sub> + ";
    if(coefs.size() == 2)
        genericExpr += "a<sub>1</sub> X";
    else if(coefs.size() > 1)
        genericExpr += "a<sub>1</sub> X + ";
    if(coefs.size() == 3)
        genericExpr += "a<sub>2</sub> X<sup>2</sup>";
    else if(coefs.size() > 2)
        genericExpr += "a<sub>2</sub> X<sup>2</sup>";
    if(coefs.size() > 3)
    {
        if(coefs.size() > 4)
        {
            genericExpr += "... + ";
            genericExpr += "a<sub>" + QString::number(coefs.size()-2) + "</sub> X<sup>" + QString::number(coefs.size()-2) + "</sup> + ";
        }

        genericExpr += "+ a<sub>" + QString::number(coefs.size()-1) + "</sub> X<sup>" + QString::number(coefs.size()-1) + "</sup>";
    }

    ui->polynomialGenericExpression->setText(genericExpr);

    QString calculableExpr = QString::number(coefs.first(), 'g', 8);
    if(coefs.size() > 1)
    {
        if(coefs[1] < 0)
            calculableExpr += " " + QString::number(coefs[1], 'g', 8) + "*x";
        else calculableExpr += " + " + QString::number(coefs[1], 'g', 8) + "*x";
    }

    for(int coef = 2 ; coef < coefs.size() ; coef++)
    {
        if(coefs[coef] < 0)
            calculableExpr += " " + QString::number(coefs[coef], 'g', 8) + "*x^" + QString::number(coef);
        else calculableExpr += " + " + QString::number(coefs[coef], 'g', 8) + "*x^" + QString::number(coef);
    }

    ui->expressionLineEdit->setText(calculableExpr);
}

void PolynomialModelWidget::updateManualRangeFields()
{
    startVal->setNumber(regression->getDrawRange().start);
    endVal->setNumber(regression->getDrawRange().end);
}

void PolynomialModelWidget::updateApproxMethod()
{
    if(ui->approachPoints->isChecked())
        regression->setApproxMethod(ApproachPoints);
    else regression->setApproxMethod(ApproachSegments);
}

void PolynomialModelWidget::updateRangeOption()
{
    if(ui->dataInterval->isChecked())
        regression->setDrawRangeCalculusMethod(LimitedToData);

    else if(ui->relativeExtrapolation->isChecked())
        regression->setDrawRangeCalculusMethod(RelativeExtrapolation);

    else if(ui->manualInterval->isChecked())
        regression->setDrawRangeCalculusMethod(Manual);
}

void PolynomialModelWidget::addWidgetsToUI()
{
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);

    startVal = new NumberLineEdit(false, informations->getFuncsList());
    startVal->setEnabled(false);
    startVal->setMaximumHeight(22);
    startVal->setMaximumWidth(150);

    connect(startVal, SIGNAL(newVal(double)), this, SLOT(manualRangeEdited()));

    endVal = new NumberLineEdit(false, informations->getFuncsList());
    endVal->setEnabled(false);
    endVal->setMaximumHeight(22);
    endVal->setMaximumWidth(150);

    connect(endVal, SIGNAL(newVal(double)), this, SLOT(manualRangeEdited()));

    QLabel *start = new QLabel(tr("Début:"));
    start->setEnabled(false);

    QLabel *end = new QLabel(tr("Fin:"));
    end->setEnabled(false);

    layout->addSpacing(30);
    layout->addWidget(start);
    layout->addWidget(startVal);
    layout->addSpacing(10);
    layout->addWidget(end);
    layout->addWidget(endVal);
    layout->addStretch();

    ui->manualEntryLayout->addLayout(layout);

    connect(ui->manualInterval, SIGNAL(toggled(bool)), start, SLOT(setEnabled(bool)));
    connect(ui->manualInterval, SIGNAL(toggled(bool)), end, SLOT(setEnabled(bool)));
    connect(ui->manualInterval, SIGNAL(toggled(bool)), startVal, SLOT(setEnabled(bool)));
    connect(ui->manualInterval, SIGNAL(toggled(bool)), endVal, SLOT(setEnabled(bool)));

    colorButton = new QColorButton();
    QLabel *colorLabel = new QLabel(tr("Couleur:"));
    ui->drawOptionsLayout->addWidget(colorLabel);
    ui->drawOptionsLayout->addWidget(colorButton);
    ui->drawOptionsLayout->addStretch();

    connect(colorButton, SIGNAL(colorChanged(QColor)), this, SIGNAL(regressionEdited()));

    QPushButton *removeButton = new QPushButton();
    removeButton->setIcon(QIcon(":/icons/remove.png"));
    removeButton->setFlat(true);

    ui->valuesTab->setCornerWidget(removeButton);

    connect(removeButton, SIGNAL(released()), this, SIGNAL(removeMe()));
}

void PolynomialModelWidget::updateDescriptionText()
{
    ui->description->setText(tr("Calcul du meilleur polynôme P approchant l'égalité:") + "\n" + " \" " + ordinate + " = " + "P( " + abscissa + " ) \" ");
}

void PolynomialModelWidget::setAbscissaName(QString name)
{
    abscissa = name;
    regression->setAbscissaName(name);
    updateDescriptionText();
}

void PolynomialModelWidget::setOrdinateName(QString name)
{
    ordinate = name;
    regression->setOrdinateName(name);
    updateDescriptionText();
}

void PolynomialModelWidget::setData(const QList<Point> &dat)
{    
    regression->setData(dat);
}

void PolynomialModelWidget::setPolar(bool pol)
{   
    polar = pol;
    regression->setPolar(pol);
}

PolynomialModelWidget::~PolynomialModelWidget()
{
    delete ui;

    informations->removeDataRegression(regValSaver);

    delete regression;
    delete regValSaver;
}
