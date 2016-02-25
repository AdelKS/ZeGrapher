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



#include "Widgets/integrationwidget.h"

IntegrationWidget::IntegrationWidget(int funcNum, QList<FuncCalculator *> funcsList) : exprCalc(true, funcsList)
{
    setTitle(tr("Initial conditions:"));

    Func = funcNum;

    antiderivatives << "F(" << "G(" << "H(" << "P(" << "R(" << "M(";  

    centralLayout = new QVBoxLayout();
    centralLayout->setSpacing(2);
    centralLayout->setMargin(0);

    centralLayout->addSpacing(5);

    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(assignNeutralPalette(QWidget*)));

    for(short i = 0; i < 6 ; i++)
        addWidgetToList(antiderivatives[i], i);

    setLayout(centralLayout);

    QColor color;
    color.setNamedColor(VALID_COLOR);
    validPalette.setColor(QPalette::Base, color);
    validPalette.setColor(QPalette::Text, Qt::black);

    color.setNamedColor(INVALID_COLOR);
    invalidPalette.setColor(QPalette::Base, color);
    invalidPalette.setColor(QPalette::Text, Qt::black);
}

void IntegrationWidget::addWidgetToList(QString name, int funcId)
{
    QWidget *widget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(2);
    layout->setMargin(0);

    QLineEdit *xLineEdit = new QLineEdit();
    xLineEdit->setFrame(false);
    xLineEdit->setMaximumHeight(25);

    connect(xLineEdit, SIGNAL(textChanged(QString)), signalMapper, SLOT(map()));
    signalMapper->setMapping(xLineEdit, xLineEdit);

    QLineEdit *yLineEdit = new QLineEdit;
    yLineEdit->setFrame(false);
    yLineEdit->setMaximumHeight(25);

    connect(yLineEdit, SIGNAL(textChanged(QString)), signalMapper, SLOT(map()));
    signalMapper->setMapping(yLineEdit, yLineEdit);

    QLabel *label1 = new QLabel(name);
    QLabel *label2 = new QLabel(") = ");

    layout->addWidget(label1);
    layout->addWidget(xLineEdit);
    layout->addWidget(label2);
    layout->addWidget(yLineEdit);

    connect(xLineEdit, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));
    connect(yLineEdit, SIGNAL(returnPressed()), this, SIGNAL(returnPressed()));

    widget->setLayout(layout);
    widget->hide();

    centralLayout->addWidget(widget);

    containerWidgetsList << widget;
    funcIds << funcId;
    xList << xLineEdit;
    yList << yLineEdit;

}

void IntegrationWidget::assignNeutralPalette(QWidget *lineEdit)
{
    lineEdit->setPalette(neutralPalette);
}

void IntegrationWidget::updateWidgetsShownState(QString expr)
{
    bool isOneShown = false, doesContain = false;

    for(int i = 0 ; i < containerWidgetsList.size(); i++)
    {
        if(i == Func)
            continue;

        doesContain = expr.contains(antiderivatives[i]);
        containerWidgetsList[i]->setHidden(!doesContain);
        isOneShown = isOneShown || doesContain;
    }

    setHidden(!isOneShown);
}

QList<Point> IntegrationWidget::getIntegrationPoints(bool &ok)
{
    ok = true;
    Point point = {0,0};
    QList<Point> list;

    int max = 0;
    for(int id: funcIds)
    {
        if(id > max)
            max = id;
    }

    for(int i = 0 ; i <= max; i++)
        list << point;


    if(isHidden())
        return list;

    for(int i = 0 ; i < containerWidgetsList.size() && ok ; i++)
    {
        if(containerWidgetsList[i]->isHidden())
            continue;

        point.x = exprCalc.calculateExpression(xList[i]->text(), ok);

        if(ok)
            xList[i]->setPalette(validPalette);
        else xList[i]->setPalette(invalidPalette);

        point.y = exprCalc.calculateExpression(yList[i]->text(), ok);

        if(ok)
            yList[i]->setPalette(validPalette);
        else yList[i]->setPalette(invalidPalette);

        if(ok)
            list[funcIds[i]] = point;
    }

    return list;
}
