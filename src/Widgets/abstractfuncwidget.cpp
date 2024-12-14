/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
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

AbstractFuncWidget::AbstractFuncWidget() :
    drawCheckBox(new QCheckBox()),
    secondContainerLayout(new QHBoxLayout()),
    nameLabel(new QLabel()),
    errorMessageLabel(new QLabel()),
    errorMessageWidget(new QWidget()),
    expressionLineEdit(new ExpressionLineEdit()),
    colorButton(new QColorButton()),
    secondColorButton(new QColorButton()),
    kConfWidget(new ParConfWidget('k')),
    treeCreator(ObjectType::FUNCTION),
    isParametric(false),
    isValid(false)
{
    addMainWidgets();
}

void AbstractFuncWidget::addMainWidgets()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();

    QHBoxLayout *firstContainerLayout = new QHBoxLayout();
    firstContainerLayout->setContentsMargins(0, 0, 0, 0);

    drawCheckBox->setChecked(true);

    expressionLineEdit->setMaximumHeight(25);

    connect(expressionLineEdit, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));

    firstContainerLayout->addWidget(drawCheckBox);
    firstContainerLayout->addWidget(nameLabel);
    firstContainerLayout->addWidget(expressionLineEdit);
    firstContainerLayout->addWidget(colorButton);
    firstContainerLayout->addWidget(secondColorButton);

    secondColorButton->hide();

    mainLayout->addLayout(firstContainerLayout);

    secondContainerLayout->setContentsMargins(0, 0, 0, 0);
    secondContainerLayout->setSpacing(4);

    mainLayout->addLayout(secondContainerLayout);

    kConfWidget->hide();

    connect(kConfWidget, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));
    mainLayout->addWidget(kConfWidget);

    QHBoxLayout *hlayout = new QHBoxLayout(errorMessageWidget);
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->setSpacing(3);

    QLabel *errorIcon = new QLabel();
    errorIcon->setPixmap(QPixmap(":/icons/error.png"));
    errorIcon->setScaledContents(true);
    errorIcon->setMaximumSize(25,25);
    hlayout->addWidget(errorIcon);

    errorMessageLabel->setWordWrap(true);
    hlayout->addWidget(errorMessageLabel);

    mainLayout->addWidget(errorMessageWidget);
    errorMessageWidget->hide();


    setLayout(mainLayout);
}
