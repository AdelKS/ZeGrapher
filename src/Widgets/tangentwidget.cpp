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



#include "Widgets/tangentwidget.h"
#include "information.h"

TangentWidget::TangentWidget(int id, QList<Function *> calcsList, QList<FuncWidget*> list, QColor col)
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

   addWidgets();
   colorButton->setColor(col);
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
    layout2->setContentsMargins(1, 1, 1, 1);

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
    layout1->setContentsMargins(1, 1, 1, 1);
    layout1->addLayout(layout2);
    layout1->addLayout(layout3);
    layout1->addLayout(layout4);
    layout1->addWidget(frame);

    setLayout(layout1);
}

void TangentWidget::kValueLineEdited()
{
    kValueLineEdit->setPalette(QPalette());
}

void TangentWidget::emitRemoveMeSignal()
{
    emit removeMe(this);
}

void TangentWidget::resetPalette()
{
    tangentPos->setPalette(QPalette());
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
       tangentPos->setPalette(information.getInvalidSyntaxPalette());
       ordinateAtOriginLineEdit->clear();
       slopeLineEdit->clear();
       return;
    }
    else tangentPos->setPalette(information.getValidSyntaxPalette());

    if(kValueLineEdit->isVisible())
    {
        k = exprCalculator->calculateExpression(kValueLineEdit->text(), isValid);
        if(!isValid)
        {
           kValueLineEdit->setPalette(information.getInvalidSyntaxPalette());
           ordinateAtOriginLineEdit->clear();
           slopeLineEdit->clear();
           return;
        }
        else kValueLineEdit->setPalette(information.getValidSyntaxPalette());
    }
    else k = 0;
}

void TangentWidget::calculateTangentPoints()
{
    if(!isValid)
        return;

    // value of the function on the tangent point
    const double fval = funcCalculators[funcID]->getFuncValue(pos, k);

    // write the tangent as y = a*x + b
    a = funcCalculators[funcID]->getDerivativeValue(pos, k);
    const double b = -pos*a + fval;

    slopeLineEdit->setText(QString::number(a, 'g', NUM_PREC));
    ordinateAtOriginLineEdit->setText(QString::number(b, 'g', NUM_PREC));

    const double d = 0.5 * lenght * sqrt(1/(a*a + 1));

    tangentPoints.center = Point {.x = pos,     .y = fval};
    tangentPoints.right  = Point {.x = pos + d, .y = a*(pos + d) + b};
    tangentPoints.left   = Point {.x = pos - d, .y = a*(pos - d) + b};
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
