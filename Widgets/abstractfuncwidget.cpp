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



#include "Widgets/abstractfuncwidget.h"

AbstractFuncWidget::AbstractFuncWidget() : treeCreator(FUNCTION)
{
    isParametric = isValid = false;

    QColor color;
    color.setNamedColor(VALID_COLOR);
     validPalette.setColor(QPalette::Base, color);
    validPalette.setColor(QPalette::Text, Qt::black);

    color.setNamedColor(INVALID_COLOR);
    invalidPalette.setColor(QPalette::Base, color);
    invalidPalette.setColor(QPalette::Text, Qt::black);

    addMainWidgets();

}

void AbstractFuncWidget::addMainWidgets()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QHBoxLayout *firstContainerLayout = new QHBoxLayout;
    firstContainerLayout->setMargin(0);
    firstContainerLayout->setSpacing(4);

    drawCheckBox = new QCheckBox();
    drawCheckBox->setChecked(true);

    nameLabel = new QLabel;

    expressionLineEdit = new QLineEdit;
    expressionLineEdit->setMaximumHeight(25);

    connect(expressionLineEdit, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));   

    colorButton = new QColorButton;
    secondColorButton = new QColorButton;

    firstContainerLayout->addWidget(drawCheckBox);
    firstContainerLayout->addWidget(nameLabel);
    firstContainerLayout->addWidget(expressionLineEdit);
    firstContainerLayout->addWidget(colorButton);
    firstContainerLayout->addWidget(secondColorButton);

    secondColorButton->hide();

    mainLayout->addLayout(firstContainerLayout);

    secondContainerLayout = new QHBoxLayout;
    secondContainerLayout->setMargin(0);
    secondContainerLayout->setSpacing(4);   

    mainLayout->addLayout(secondContainerLayout);

    kConfWidget = new ParConfWidget('k');
    kConfWidget->hide();

    connect(kConfWidget, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));
    mainLayout->addWidget(kConfWidget);

    errorMessageWidget = new QWidget();
    QHBoxLayout *hlayout = new QHBoxLayout(errorMessageWidget);
    hlayout->setMargin(0);
    hlayout->setSpacing(3);

    QLabel *errorIcon = new QLabel();
    errorIcon->setPixmap(QPixmap(":/icons/error.png"));
    errorIcon->setScaledContents(true);
    errorIcon->setMaximumSize(25,25);
    hlayout->addWidget(errorIcon);

    errorMessageLabel = new QLabel();
    errorMessageLabel->setWordWrap(true);
    hlayout->addWidget(errorMessageLabel);

    mainLayout->addWidget(errorMessageWidget);
    errorMessageWidget->hide();


    setLayout(mainLayout);
}
