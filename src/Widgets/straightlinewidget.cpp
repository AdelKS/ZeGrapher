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




#include "Widgets/straightlinewidget.h"
#include "information.h"

StraightLineWidget::StraightLineWidget(int id, QList<Function *> calcsList, QColor col)
{
    lineID = id;
    funcCalculators = calcsList;
    exprCalculator = new ExprCalculator(false, funcCalculators);
    valid = false;

    addWidgets(col);
}

void StraightLineWidget::changeID(int id)
{
    lineID = id;
    nameLabel->setText("(D<sub>" + QString::number(lineID + 1) + "</sub>): ");
}

QColor StraightLineWidget::getColor()
{
    return colorButton->getCurrentColor();
}

void StraightLineWidget::resetPaletteForLineEditA()
{
    a->setPalette(QPalette());
}

void StraightLineWidget::resetPaletteForLineEditB()
{
    b->setPalette(QPalette());
}

void StraightLineWidget::resetPaletteForLineEditC()
{
    c->setPalette(QPalette());
}

void StraightLineWidget::addWidgets(QColor col)
{
    drawCheckBox = new QCheckBox;
    drawCheckBox->setChecked(true);
    drawCheckBox->setMinimumSize(20,20);
    connect(drawCheckBox, SIGNAL(released()), this, SIGNAL(drawStateChanged()));

    nameLabel = new QLabel();
    changeID(lineID);

    a = new QLineEdit;
    a->setMaximumHeight(25);
    a->setFrame(false);
    connect(a, SIGNAL(textChanged(QString)), this, SLOT(resetPaletteForLineEditA()));
    connect(a, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));

    QLabel *x = new QLabel("x +");
    b = new QLineEdit;
    b->setMaximumHeight(25);
    b->setFrame(false);
    connect(b, SIGNAL(textChanged(QString)), this, SLOT(resetPaletteForLineEditB()));
    connect(b, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));

    QLabel *y = new QLabel(" y =");
    c = new QLineEdit;
    c->setMaximumHeight(25);
    c->setFrame(false);
    connect(c, SIGNAL(textChanged(QString)), this, SLOT(resetPaletteForLineEditC()));
    connect(c, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));

    colorButton = new QColorButton(col);
    connect(colorButton, SIGNAL(colorChanged(QColor)), this, SIGNAL(drawStateChanged()));

    QPushButton *removeButton = new QPushButton;
    removeButton->setFixedSize(25,25);
    removeButton->setIconSize(QSize(24,24));
    removeButton->setFlat(true);
    removeButton->setIcon(QIcon(":/icons/remove.png"));

    connect(removeButton, SIGNAL(released()), this, SLOT(emitRemoveMeSignal()));

    QVBoxLayout *layout1 = new QVBoxLayout;
    QHBoxLayout *layout2 = new QHBoxLayout;

    layout2->addWidget(drawCheckBox);
    layout2->addStretch();
    layout2->addWidget(nameLabel);
    layout2->addWidget(a);
    layout2->addWidget(x);
    layout2->addWidget(b);
    layout2->addWidget(y);
    layout2->addWidget(c);
    layout2->addStretch();
    layout2->addWidget(colorButton);
    layout2->addWidget(removeButton);
    layout2->setSpacing(3);
    layout2->setContentsMargins(1, 1, 1, 1);

    QFrame *frame = new QFrame;
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);

    layout1->setSpacing(3);
    layout1->setContentsMargins(1, 1, 1, 1);
    layout1->addLayout(layout2);
    layout1->addWidget(frame);

    setLayout(layout1);
}

void StraightLineWidget::emitRemoveMeSignal()
{
    emit removeMe(this);
}

void StraightLineWidget::validate()
{
    val_a = exprCalculator->calculateExpression(a->text(), valid);
    if(!valid)
    {
        a->setPalette(information.getInvalidSyntaxPalette());
        return;
    }
    else a->setPalette(information.getValidSyntaxPalette());

    val_b = exprCalculator->calculateExpression(b->text(), valid);
    if(!valid)
    {
        b->setPalette(information.getInvalidSyntaxPalette());
        return;
    }
    else b->setPalette(information.getValidSyntaxPalette());

    val_c = exprCalculator->calculateExpression(c->text(), valid);
    if(!valid)
    {
        c->setPalette(information.getInvalidSyntaxPalette());
        return;
    }
    else c->setPalette(information.getValidSyntaxPalette());

    valid = !(val_a == 0 && val_b == 0);
    if(!valid)
    {
        QMessageBox::warning(window(), tr("Error"), tr("Error on straight line") + "D<sub>" + QString::number(lineID)+ "</sub>" +  tr("x and y coefficients can't be simultaneously equal to zero."));
    }
}

double StraightLineWidget::getY(double x)
{
    return (val_c - val_a * x)/val_b;
}

double StraightLineWidget::getVerticalPos()
{
    return val_c / val_a;
}

bool StraightLineWidget::isValid()
{
    return valid && drawCheckBox->isChecked();
}

bool StraightLineWidget::isVertical()
{
    return val_b == 0;
}
