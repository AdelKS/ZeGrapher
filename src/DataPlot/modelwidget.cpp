/****************************************************************************
**  Copyright (c) 2019, Adel Kara Slimane <adel.ks@zegrapher.com>
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


ModelWidget::ModelWidget(const std::weak_ptr<UserData> &userData, Information *info,
              QString xname, QString yname, QWidget *parent):
    QFrame(parent), currentState(ChoiceWidget), information(info), abscissa(xname), ordinate(yname), userData(userData)
{
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    setFrameStyle(QFrame::Raised);
    setFrameShape(QFrame::StyledPanel);

    modelChoice = new ModelChoiceWidget();
    layout->addWidget(modelChoice);

    connect(modelChoice, SIGNAL(modelSelected(ModelType)), this, SLOT(displaySelectedModel(ModelType)));
    connect(modelChoice, SIGNAL(removeMe()), this, SLOT(emitRemoveMeSignal()));
}

void ModelWidget::refreshModel()
{
   if(currentState == PolynomialWidget)
   {
        polynomialModel->refreshModel();
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

void ModelWidget::emitRemoveMeSignal()
{
    emit removeMe(this);
}

void ModelWidget::displaySelectedModel(ModelType model)
{
    modelChoice->hide();

  if(model == PolynomialModel)
  {
        polynomialModel = new PolynomialModelWidget(userData, information, abscissa, ordinate);
        layout->addWidget(polynomialModel);        
        currentState = PolynomialWidget;

        connect(polynomialModel, SIGNAL(removeMe()), this, SLOT(emitRemoveMeSignal()));

    }// other cases will be added when new modeling types would be implemented
}
