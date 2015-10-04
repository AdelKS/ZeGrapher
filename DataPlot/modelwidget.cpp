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
