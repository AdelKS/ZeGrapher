#include "modelwidget.h"


ModelWidget::ModelWidget(const QList<Point> &dat, Informations *info, bool isPolar, QString xname, QString yname, QWidget *parent) : QFrame(parent)
{
    layout = new QVBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    abscissa = xname;
    ordinate = yname;
    polar = isPolar;

    setFrameStyle(QFrame::Raised);
    setFrameShape(QFrame::StyledPanel);

    data = dat;
    informations = info;

    modelChoice = new ModelChoiceWidget();
    layout->addWidget(modelChoice);
    currentState = ChoiceWidget;

    connect(modelChoice, SIGNAL(modelSelected(ModelType)), this, SLOT(displaySelectedModel(ModelType)));
    connect(modelChoice, SIGNAL(removeMe()), this, SLOT(emitRemoveMeSignal()));
}

void ModelWidget::setData(const QList<Point> &dat)
{
    data = dat;

   if(currentState == PolynomialWidget)
   {
        polynomialModel->setData(data);
   }
}

void ModelWidget::setAbscissaName(QString name)
{
    abscissa = name;

    if(currentState == PolynomialWidget)
        polynomialModel->setAbscissaName(name);
}

void ModelWidget::setOrdinateName(QString name)
{
    ordinate = name;

    if(currentState == PolynomialWidget)
        polynomialModel->setOrdinateName(name);
}

void ModelWidget::setPolar(bool state)
{
    polar = state;

    if(currentState == PolynomialWidget)
    {
        polynomialModel->setPolar(state);
    }
}

void ModelWidget::emitRemoveMeSignal()
{
    emit removeMe(this);
}

void ModelWidget::displaySelectedModel(ModelType model)
{
    modelChoice->hide();

  if(model == PolynomialModel)
  {
        polynomialModel = new PolynomialModelWidget(data, informations, abscissa, ordinate, polar);
        layout->addWidget(polynomialModel);
        currentState = PolynomialWidget;

        connect(polynomialModel, SIGNAL(removeMe()), this, SLOT(emitRemoveMeSignal()));

    }// other cases will be added when new modelisation types would be implemented
}

ModelWidget::~ModelWidget()
{

}
