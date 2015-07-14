#include "modelwidget.h"


ModelWidget::ModelWidget(const QList<Point> &dat, Informations *info, QWidget *parent) : QWidget(parent)
{
    layout = new QVBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);

    data = dat;
    informations = info;

    modelChoice = new ModelChoiceWidget();
    layout->addWidget(modelChoice);
    currentState = State::Choice;

    connect(modelChoice, SIGNAL(modelSelected(ModelType)), this, SLOT(displaySelectedModel(ModelType)));
}

void ModelWidget::displaySelectedModel(ModelType type)
{
    modelChoice->hide();

    switch(type)
    {
    case ModelType::Polynomial:

        polynomialModel = new PolynomialModelWidget(data, informations);
        layout->addWidget(polynomialModel);
        currentState = State::Polynomial;

        break;
    }// other cases will be added when new modelisation types would be implemented
}

ModelWidget::~ModelWidget()
{
    delete modelChoice;
}
