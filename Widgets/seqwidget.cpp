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



#include "Widgets/seqwidget.h"

SeqWidget::SeqWidget(QChar name, int id, QColor color) : colorSaver(color)
{    
    seqName = name;
    seqNum = id;
    nameLabel->setText(QString(name) + "<sub>n</sub> =");
    calculator = new SeqCalculator(id, "(" + QString(name) + "<sub>n</sub>)", this, errorMessageLabel);
    calculator->setColorSaver(&colorSaver);
    defaultRange.start = 0;
    defaultRange.end = 0.5;
    defaultRange.step = 1;

    connect(&colorSaver, SIGNAL(colorsChanged()), this, SIGNAL(drawStateChanged()));
    connect(drawCheckBox, SIGNAL(released()), this, SIGNAL(drawStateChanged()));
    connect(expressionLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkExprLineEdit()));
    connect(expressionLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkCalledFuncsSeqsParametric()));
    connect(colorButton, SIGNAL(colorChanged(QColor)), &colorSaver, SLOT(setFristColor(QColor)));
    connect(secondColorButton, SIGNAL(colorChanged(QColor)), &colorSaver, SLOT(setLastColor(QColor)));
    connect(drawCheckBox, SIGNAL(toggled(bool)), calculator, SLOT(setDrawState(bool)));

    areFirstValsParametric = areCalledFuncsSeqsParametric = isExprParametric = false;

    addSeqWidgets();
}

void SeqWidget::addSeqWidgets()
{
    QLabel *firstValsLabel = new QLabel(seqName + QString("<sub>0</sub> ; ") + seqName + QString("<sub>1</sub> ; ...  ="));

    firstValsLine = new QLineEdit();
    firstValsLine->setMaximumHeight(25);

    secondContainerLayout->addStretch();
    secondContainerLayout->addWidget(firstValsLabel);
    secondContainerLayout->addWidget(firstValsLine);

    connect(firstValsLine, SIGNAL(textChanged(QString)), this, SLOT(checkFirstValsLineEdit()));
    connect(firstValsLine, SIGNAL(textChanged(QString)), this, SLOT(checkCalledFuncsSeqsParametric()));
    connect(firstValsLine, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));
}

void SeqWidget::checkCalledFuncsSeqsParametric()
{
    bool oldAreCalledFuncsSeqsParametric = areCalledFuncsSeqsParametric;

    areCalledFuncsSeqsParametric = false;

    QList<int> calledFuncs =  treeCreator.getCalledFuncs(expressionLineEdit->text());

    for(int i = 0 ; i < calledFuncs.size() && !areCalledFuncsSeqsParametric ; i++)
        areCalledFuncsSeqsParametric = areCalledFuncsSeqsParametric || funcWidgets[calledFuncs[i]]->isFuncParametric();

    if(!areCalledFuncsSeqsParametric)
    {
        QList<int> calledSeqs = treeCreator.getCalledSeqs(expressionLineEdit->text());
        calledSeqs << treeCreator.getCalledSeqs(firstValsLine->text());

        for(int i = 0 ; i < calledSeqs.size() && !areCalledFuncsSeqsParametric ; i++)
            areCalledFuncsSeqsParametric = areCalledFuncsSeqsParametric || seqWidgets[calledSeqs[i]]->isSeqParametric();
    }

    if(oldAreCalledFuncsSeqsParametric != areCalledFuncsSeqsParametric)
        updateParametricState();
}

SeqCalculator* SeqWidget::getCalculator()
{
    return calculator;
}

void SeqWidget::checkExprLineEdit()
{
    expressionLineEdit->setPalette(neutralPalette);

    bool oldIsExprParametric = isExprParametric;

    isExprParametric = expressionLineEdit->text().contains('k');

    if(oldIsExprParametric != isExprParametric)
        updateParametricState();
}

void SeqWidget::setFuncWidgets(QList<FuncWidget*> widgets)
{
    funcWidgets = widgets;
}

void SeqWidget::updateParametricState()
{
    bool oldParState = isParametric;

    isParametric = isExprParametric || areFirstValsParametric || areCalledFuncsSeqsParametric;

    kConfWidget->setHidden(!isParametric);
    secondColorButton->setHidden(!isParametric);

    if(oldParState != isParametric)
        emit newParametricState();
}

void SeqWidget::checkFirstValsLineEdit()
{
    firstValsLine->setPalette(neutralPalette);

    bool oldAreFirstValsParametric = areFirstValsParametric;
    areFirstValsParametric = firstValsLine->text().contains('k');

    if(oldAreFirstValsParametric != areFirstValsParametric)
        updateParametricState();
}

void SeqWidget::setSeqWidgets(QList<SeqWidget*> widgets)
{
    seqWidgets = widgets;
}

bool SeqWidget::isSeqParametric()
{
    return isParametric;
}

void SeqWidget::firstValidation()
{
    errorMessageLabel->clear();

    if(expressionLineEdit->text().isEmpty())
    {
        isValid = false;
        calculator->setInvalid();
        return;
    }   

    kConfWidget->validate();

    Range range = kConfWidget->getRange();

    calculator->setParametricInfo(isParametric, range);
    colorSaver.setCurvesNum(trunc((range.end - range.start)/range.step) + 1);

    bool firstValsValidated = calculator->validateFirstValsExpr(firstValsLine->text());
    if(firstValsValidated)    
        firstValsLine->setPalette(validPalette);    
    else firstValsLine->setPalette(invalidPalette);

    bool seqExprValidated = calculator->validateSeqExpr(expressionLineEdit->text());
    if(seqExprValidated)
        expressionLineEdit->setPalette(validPalette);
    else expressionLineEdit->setPalette(invalidPalette);

    calculator->setParametricInfo(isParametric, kConfWidget->getRange());

    if(!errorMessageLabel->text().isEmpty())
        errorMessageWidget->show();

    isValid = firstValsValidated && seqExprValidated && (!isParametric || kConfWidget->isValid());
}

void SeqWidget::setFuncsList(QList<FuncCalculator*> list)
{
    kConfWidget->setFuncsList(list);
}

void SeqWidget::secondValidation()
{
    if(!isValid)
        return;

    isValid = calculator->check_called_funcs_and_seqs_validity();
    if(!isValid)
    {
        if(!errorMessageLabel->text().isEmpty())
            errorMessageWidget->show();
        return;
    }

    isValid = calculator->checkByCalculatingFirstValuesTrees();
    if(!isValid)
    {
        if(!errorMessageLabel->text().isEmpty())
            errorMessageWidget->show();
        return;
    }


}

void SeqWidget::thirdValidation()
{
    if(!isValid)
        return;

    isValid = calculator->checkByCalculatingValues();
    if(!isValid)
    {
        if(!errorMessageLabel->text().isEmpty())
            errorMessageWidget->show();
        return;
    }

    isValid = calculator->check_called_funcs_and_seqs_validity();

    if(isValid)
        errorMessageWidget->hide();
    else if(!errorMessageLabel->text().isEmpty())
        errorMessageWidget->show();
}
