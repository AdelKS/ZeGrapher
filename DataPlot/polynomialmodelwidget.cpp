#include "polynomialmodelwidget.h"
#include "ui_polynomialmodelwidget.h"

PolynomialModelWidget::PolynomialModelWidget(const QList<Point> &dat, Informations *info, QString xname, QString yname, bool pol, QWidget *parent) :
    QWidget(parent), ui(new Ui::PolynomialModelWidget)
{
    ui->setupUi(this);

    regression = new PolynomialRegression(ui->degree->value(), (ApproxMethod)ui->approachPoints->isChecked(), LimitedToData,
                                          ui->relativeExtrapol->value(), ui->drawModel->isChecked(), pol);
    regression->setData(dat);


    abscissa = xname;
    ordinate = yname; //of course the abscissa and ordinate would mean the polar angle and radius respectively
    updateDescriptionText();

    polar = pol;
    data = dat;
    informations = info;

    informations->addDataRegression(regression);

    addWidgetsToUI();

    connect(ui->drawModel, SIGNAL(toggled(bool)), regression, SLOT(setDrawState(bool)));
    connect(colorButton, SIGNAL(colorChanged(QColor)), regression, SLOT(setColor(QColor)));
    connect(ui->degree, SIGNAL(valueChanged(int)), regression, SLOT(setPolynomialRegressionDegree(int)));
    connect(ui->relativeExtrapol, SIGNAL(valueChanged(double)), regression, SLOT(setRelativeRangeCoef(double)));

    connect(ui->approachPoints, SIGNAL(toggled(bool)), this, SLOT(updateApproxMethod()));
    connect(ui->dataInterval, SIGNAL(clicked(bool)), this, SLOT(updateRangeOption()));
    connect(ui->relativeExtrapolation, SIGNAL(clicked(bool)), this, SLOT(updateRangeOption()));
    connect(ui->manualInterval, SIGNAL(clicked(bool)), this, SLOT(updateRangeOption()));
    connect(ui->alwaysRepresent, SIGNAL(clicked(bool)), this, SLOT(updateRangeOption()));
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

    else regression->setDrawRangeCalculusMethod(DrawAll);
}

void PolynomialModelWidget::addWidgetsToUI()
{
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);

    startVal = new NumberLineEdit(false, informations->getFuncsList());
    startVal->setEnabled(false);
    startVal->setMaximumHeight(22);
    startVal->setMaximumWidth(150);

    endVal = new NumberLineEdit(false, informations->getFuncsList());
    endVal->setEnabled(false);
    endVal->setMaximumHeight(22);
    endVal->setMaximumWidth(150);

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
    ui->drawOptionsLayout->addWidget(colorButton);

    connect(colorButton, SIGNAL(colorChanged(QColor)), this, SIGNAL(regressionEdited()));
}

void PolynomialModelWidget::updateDescriptionText()
{
    ui->description->setText(tr("calcul du meilleur polynôme P approchant l'égalité:") + "\n" + " \" " + ordinate + " = " + "P( " + abscissa + " ) \" ");
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

    informations->removeDataRegression(regression);

    delete regression;
}
