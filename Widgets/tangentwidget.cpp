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


#include "Widgets/tangentwidget.h"

TangentWidget::TangentWidget(int id, QList<FuncCalculator *> calcsList, QList<FuncWidget*> list)
{
   tangentID = id;
   funcNames << "f" << "g" << "h" << "p" << "r" << "m";
   isValid = false;
   funcCalculators = calcsList;

   funcWidgets = list;
   for(int i = 0 ; i < funcWidgets.size(); i++)
       connect(funcWidgets[i], SIGNAL(newParametricState(int)), this, SLOT(newFuncParState(int)));

   exprCalculator = new ExprCalculator(false, funcCalculators);
   lenght = 3;

   QColor color;
   color.setNamedColor(VALID_COLOR);
   validPalette.setColor(QPalette::Base, color);

   color.setNamedColor(INVALID_COLOR);
   invalidPalette.setColor(QPalette::Base, color);

   addWidgets();
}

void TangentWidget::newFuncChoosen(int funcNum)
{
    kTextLabel->setHidden(!funcWidgets[funcNum]->isFuncParametric());
    kValueLineEdit->setHidden(!funcWidgets[funcNum]->isFuncParametric());
}

void TangentWidget::newFuncParState(int funcNum)
{
    if(funcNum != functionsComboBox->currentIndex())
        return;

    kTextLabel->setHidden(!funcWidgets[funcNum]->isFuncParametric());
    kValueLineEdit->setHidden(!funcWidgets[funcNum]->isFuncParametric());
}

void TangentWidget::changeID(int id)
{
    tangentID = id;
    nameLabel->setText("(T<sub>" + QString::number(tangentID + 1) + "</sub>) " + tr("tangent to :"));
}

QColor TangentWidget::getColor()
{
    return colorButton->getCurrentColor();
}

void TangentWidget::addWidgets()
{
    drawCheckBox = new QCheckBox;
    drawCheckBox->setChecked(true);
    drawCheckBox->setMaximumSize(20,20);
    connect(drawCheckBox, SIGNAL(released()), this, SIGNAL(drawStateChanged()));

    nameLabel = new QLabel();
    changeID(tangentID);

    functionsComboBox = new QComboBox;
    functionsComboBox->addItems(funcNames);
    functionsComboBox->setFixedSize(40,25);
    connect(functionsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(newFuncChoosen(int)));

    for(int i = 0 ; i < funcNames.size(); i++){functionsComboBox->addItem(funcNames[i]);}

    QLabel *label2 = new QLabel(tr("at:  x ="));

    tangentPos = new QLineEdit;
    tangentPos->setMaximumSize(80,25);
    connect(tangentPos, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));

    colorButton = new QColorButton(Qt::black);
    connect(colorButton, SIGNAL(colorChanged(QColor)), this, SIGNAL(drawStateChanged()));

    QPushButton *removeButton = new QPushButton;
    removeButton->setFixedSize(25,25);
    removeButton->setIconSize(QSize(25,25));
    removeButton->setFlat(true);
    removeButton->setIcon(QIcon(":/icons/remove.png"));

    connect(removeButton, SIGNAL(released()), this, SLOT(emitRemoveMeSignal()));  

    QVBoxLayout *layout1 = new QVBoxLayout;
    QHBoxLayout *layout2 = new QHBoxLayout;
    QHBoxLayout *layout3 = new QHBoxLayout;
    QHBoxLayout *layout4 = new QHBoxLayout;

    layout2->addWidget(drawCheckBox);
    layout2->addWidget(nameLabel);
    layout2->addWidget(functionsComboBox);
    layout2->addWidget(label2);
    layout2->addWidget(tangentPos);
    layout2->addStretch();
    layout2->addWidget(colorButton);
    layout2->addWidget(removeButton);
    layout2->setSpacing(3);
    layout2->setMargin(1);

    QLabel *yLabel = new QLabel("y = ");
    yLabel->setMaximumHeight(25);

    QLabel *xLabel = new QLabel("x + ");
    xLabel->setMaximumHeight(25);

    slopeLineEdit = new QLineEdit();
    slopeLineEdit->setMaximumHeight(25);
    slopeLineEdit->setFrame(false);
    slopeLineEdit->setReadOnly(true);

    ordinateAtOriginLineEdit = new QLineEdit();
    ordinateAtOriginLineEdit->setMaximumHeight(25);
    ordinateAtOriginLineEdit->setFrame(false);
    ordinateAtOriginLineEdit->setReadOnly(true);

    layout3->addSpacing(25);
    layout3->addWidget(yLabel);
    layout3->addWidget(slopeLineEdit);
    layout3->addWidget(xLabel);
    layout3->addWidget(ordinateAtOriginLineEdit);
    layout3->addSpacing(5);

    kValueLineEdit = new QLineEdit;
    kValueLineEdit->setMaximumHeight(25);
    kValueLineEdit->setHidden(!funcWidgets[0]->isFuncParametric());
    connect(kValueLineEdit, SIGNAL(textChanged(QString)), this, SLOT(kValueLineEdited()));
    connect(kValueLineEdit, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));

    kTextLabel = new QLabel(tr("with k ="));
    kTextLabel->setMaximumHeight(25);
    kTextLabel->setHidden(!funcWidgets[0]->isFuncParametric());

    layout4->addStretch();
    layout4->addWidget(kTextLabel);
    layout4->addWidget(kValueLineEdit);
    layout4->addStretch();

    QFrame *frame = new QFrame;
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);

    layout1->setSpacing(3);
    layout1->setMargin(1);
    layout1->addLayout(layout2);
    layout1->addLayout(layout3);
    layout1->addLayout(layout4);
    layout1->addWidget(frame);

    setLayout(layout1);
}

void TangentWidget::kValueLineEdited()
{
    kValueLineEdit->setPalette(neutralPalette);
}

void TangentWidget::emitRemoveMeSignal()
{
    emit removeMe(this);
}

void TangentWidget::resetPalette()
{
    tangentPos->setPalette(neutralPalette);
}

void TangentWidget::validate()
{
    funcID = functionsComboBox->currentIndex();

    isValid = funcCalculators[funcID]->isFuncValid();
    if(!isValid)
    {
        slopeLineEdit->setText(tr("Choosen Function"));
        ordinateAtOriginLineEdit->setText(tr("Invalid"));
        return;
    }

    pos = exprCalculator->calculateExpression(tangentPos->text(), isValid);
    if(!isValid)
    {
       tangentPos->setPalette(invalidPalette);
       ordinateAtOriginLineEdit->clear();
       slopeLineEdit->clear();
       return;
    }
    else tangentPos->setPalette(validPalette);

    if(kValueLineEdit->isVisible())
    {
        k = exprCalculator->calculateExpression(kValueLineEdit->text(), isValid);
        if(!isValid)
        {
           kValueLineEdit->setPalette(invalidPalette);
           ordinateAtOriginLineEdit->clear();
           slopeLineEdit->clear();
           return;
        }
        else kValueLineEdit->setPalette(validPalette);
    }
    else k = 0;
}

void TangentWidget::calculateTangentPoints(double xUnit, double yUnit)
{
    if(!isValid)
        return;

    raty = (yUnit/xUnit+1)/2;
    ratx = (xUnit/yUnit+1)/2;

    a = funcCalculators[funcID]->getDerivativeValue(pos, k);
    double b = -pos*a + funcCalculators[funcID]->getFuncValue(pos, k);

    slopeLineEdit->setText(QString::number(a, 'g', NUM_PREC));
    ordinateAtOriginLineEdit->setText(QString::number(b, 'g', NUM_PREC));

    double d = 0.5 * lenght * sqrt(1/(a*a*raty*raty + ratx*ratx));

    tangentPoints.center.x = pos;
    tangentPoints.center.y = funcCalculators[funcID]->getFuncValue(pos, k);

    tangentPoints.right.x = pos + d;
    tangentPoints.right.y = a*tangentPoints.right.x + b;

    tangentPoints.left.x = pos - d;
    tangentPoints.left.y = a*tangentPoints.left.x + b;
}

bool TangentWidget::isTangentValid()
{
    return isValid && drawCheckBox->isChecked();
}

void TangentWidget::move(double newPos)
{
    pos = newPos;
    tangentPos->setText(QString::number(pos, 'g', NUM_PREC));
}

TangentPoints TangentWidget::getCaracteristicPoints()
{
    return tangentPoints;
}

void TangentWidget::resizeTangent(double dx, double side) // side = -1 for resize with left point, +1 for right point
{
    lenght += 2 * side * dx * sqrt(a*a*raty*raty + ratx*ratx);
}
