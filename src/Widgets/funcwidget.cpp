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




#include "Widgets/funcwidget.h"
#include "information.h"


FuncWidget::FuncWidget(QChar name, int id, QColor color) : AbstractFuncWidget(), colorSaver(color)
{
    funcName = name;
    funcNum = id;
    colorButton->setColor(color);
    secondColorButton->setColor(color);

    isExprParametric = areCalledFuncsParametric = false;

    calculator = new Function(id, QString(name));
    calculator->setColorSaver(&colorSaver);

    connect(&colorSaver, SIGNAL(colorsChanged()), this, SIGNAL(drawStateChanged()));
    connect(drawCheckBox, SIGNAL(released()), this, SIGNAL(drawStateChanged()));

    nameLabel->setText(QString(name) + "(x) =");

    connect(colorButton, SIGNAL(colorChanged(QColor)), &colorSaver, SLOT(setFristColor(QColor)));
    connect(secondColorButton, SIGNAL(colorChanged(QColor)), &colorSaver, SLOT(setLastColor(QColor)));
    connect(expressionLineEdit, SIGNAL(textChanged(QString)), this, SLOT(resetToNeutralState()));
    connect(&colorSaver, SIGNAL(colorsChanged()), &information, SLOT(emitUpdateSignal()));
    connect(drawCheckBox, SIGNAL(toggled(bool)), calculator, SLOT(setDrawState(bool)));
    connect(expressionLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkExprLineEdit()));
    connect(expressionLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkCalledFuncsParametric()));
}

void FuncWidget::resetToNeutralState()
{
    expressionLineEdit->setNeutral();
}

 void FuncWidget::checkCalledFuncsParametric()
 {
     bool oldAreCalledFuncsParametric = areCalledFuncsParametric;
     areCalledFuncsParametric = false;

     QList<int> calledFuncs =  treeCreator.getCalledFuncs(expressionLineEdit->text());

     for(int i = 0 ; i < calledFuncs.size() && !areCalledFuncsParametric ; i++)
         areCalledFuncsParametric = areCalledFuncsParametric || funcWidgets[calledFuncs[i]]->isFuncParametric();

     if(oldAreCalledFuncsParametric != areCalledFuncsParametric)
         updateParametricState();
 }

void FuncWidget::setFuncsCalcsList(QList<Function *> list)
{
    integrationWidget = new IntegrationWidget(funcNum, list);
    connect(expressionLineEdit, SIGNAL(textChanged(QString)), integrationWidget, SLOT(updateWidgetsShownState(QString)));
    connect(integrationWidget, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));
    secondContainerLayout->addWidget(integrationWidget);
    integrationWidget->hide();
    kConfWidget->setFuncsList(list);

    connect(expressionLineEdit, SIGNAL(textChanged(QString)), integrationWidget, SLOT(updateWidgetsShownState(QString)));
}

void FuncWidget::updateParametricState()
{
    bool oldParState = isParametric;

    isParametric = isExprParametric || areCalledFuncsParametric;

    kConfWidget->setHidden(!isParametric);
    secondColorButton->setHidden(!isParametric);

    if(oldParState != isParametric)
        emit newParametricState(funcNum);
}

void FuncWidget::checkExprLineEdit()
{
     expressionLineEdit->setNeutral();

     bool oldIsExprParametric = isExprParametric;

     isExprParametric = expressionLineEdit->text().contains('k');

     if(oldIsExprParametric != isExprParametric)
         updateParametricState();
}

void FuncWidget::setFuncWidgets(QList<FuncWidget*> widgets)
{
    funcWidgets = widgets;
}

bool FuncWidget::isFuncParametric()
{
    return isParametric;
}

Function* FuncWidget::getCalculator()
{
    return calculator;
}

void FuncWidget::firstValidation()
{
    if(expressionLineEdit->text().isEmpty())
    {
        isValid = false;
        errorMessageWidget->hide();
        calculator->setInvalid();
        return;
    }

    isValid = calculator->validateExpression(expressionLineEdit->text());
    calculator->setParametric(isParametric);

    if(kConfWidget->isVisible())
        kConfWidget->validate();

    Range range = kConfWidget->getRange();

    calculator->setParametricRange(range);
    colorSaver.setCurvesNum(trunc((range.end - range.start)/range.step) + 1);

    if(isValid)
        expressionLineEdit->setValid();
    else expressionLineEdit->setInvalid();

    isValid = isValid && validateIntegrationPoints() && (!isParametric || kConfWidget->isValid());
}

bool FuncWidget::validateIntegrationPoints()
{
    bool ok = true;

    calculator->setIntegrationPointsList(integrationWidget->getIntegrationPoints(ok));
    calculator->setIntegrationPointsValidity(ok);

    return ok;
}

void FuncWidget::secondValidation()
{
    if(!isValid)
        return;

    errorMessageLabel->hide();

    std::optional<QString> errorMessage = calculator->checkFuncCallingInclusions();

    if (errorMessage)
    {
        errorMessageLabel->setText(errorMessage.value());
        errorMessageLabel->show();
    }
}

FuncWidget::~FuncWidget()
{
    delete calculator;
}
