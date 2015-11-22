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



#include "ValuesTable/pareqtable.h"

ParEqTable::ParEqTable(Information *info) : AbstractTable()
{
    information = info;
    exprCalc = new ExprCalculator(false, info->getFuncsList());

    k = 0;

    updateTimer->setInterval(2000);
    updateTimer->setSingleShot(true);

    QColor color;
    color.setNamedColor(VALID_COLOR);
    validPalette.setColor(QPalette::Base, color);

    color.setNamedColor(INVALID_COLOR);
    invalidPalette.setColor(QPalette::Base, color);

    connect(information, SIGNAL(updateOccured()), updateTimer, SLOT(start()));
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTable()));
    connect(precision, SIGNAL(valueChanged(int)), this, SLOT(precisionEdited()));
    connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(cellEdited(QStandardItem*)));

}

void ParEqTable::setTableParameters(ValuesTableParameters par)
{
    parameters = par;
    parEq = information->getParEqsList()->at(parameters.id);
    connect(parEq, SIGNAL(destroyed()), this, SIGNAL(previous()));

    title->setText(tr("Parametric equation: ") + parameters.name);

    if(parEq->isParEqParametric())
    {
        k = parEq->getKRange().start;
        k_parameter_widget->show();
        k_value->setText(QString::number(k, 'g', precision->value()));

        connect(k_value, SIGNAL(returnPressed()), this, SLOT(kValueEdited()));
    }

    updateTable();
}


void ParEqTable::precisionEdited()
{
    disableCellEdit = true;

    for(int i = 0; i < parEqValues.tValues.size(); i++)
    {
        if(!std::isnan(parEqValues.tValues[i]))
        {
            model->item(i+1, 0)->setText(QString::number(parEqValues.tValues[i], 'g', precision->value()));
            model->item(i+1, 1)->setText(QString::number(parEqValues.xValues[i], 'g', precision->value()));
            model->item(i+1, 2)->setText(QString::number(parEqValues.yValues[i], 'g', precision->value()));
        }
    }

    disableCellEdit = false;
}

void ParEqTable::kValueEdited()
{
    bool ok = true;
    double temp = exprCalc->calculateExpression(k_value->text(), ok);

    if(!ok)
        k_value->setPalette(invalidPalette);
    else k_value->setPalette(validPalette);

    k = temp;
    updateTable();
}

void ParEqTable::updateTable()
{
    model->clear();

    parEqValues.tValues.clear();
    parEqValues.xValues.clear();
    parEqValues.yValues.clear();

    if(!parEq->isValid())
        return;

    if(parEq->isParEqParametric() && k_parameter_widget->isHidden())
    {
        k = parEq->getKRange().start;
        k_parameter_widget->show();

        k_value->setText(QString::number(k, 'g', precision->value()));

        connect(k_value, SIGNAL(returnPressed()), this, SLOT(kValueEdited()));
    }

    if(parameters.entryType == FROM_CURRENT_GRAPHIC || parameters.entryType == PREDEFINED_ENTRY)
        fillFromRange();

    else emptyCellsFill();

    tableView->setModel(model);

    for(short i = 0; i < model->columnCount(); i++)
        tableView->setColumnWidth(i, 140);
}

void ParEqTable::fillFromRange()
{
    if(parameters.entryType == FROM_CURRENT_GRAPHIC)
        parameters.range = parEq->getTRange(k);

    fillWithValues();

    tableView->setModel(model);
}

void ParEqTable::cellEdited(QStandardItem *item)
{
    if(item->text().isEmpty() || item->column() != 0 || disableCellEdit)
        return;

    bool ok = true;
    double t = exprCalc->calculateExpression(item->text(), ok);
    if(!ok)
    {
         QMessageBox::warning(this, tr("Error"), tr("Syntax error in this entry"));
         return;
    }

    Point point = parEq->getPoint(t, k);

    parEqValues.tValues[item->row()-1] = t; // row-1 because there is a "x" cell on the first cell of the first column
    parEqValues.xValues[item->row()-1] = point.x;
    parEqValues.yValues[item->row()-1] = point.y;

    model->item(item->row(), 1)->setText(QString::number(point.x, 'g', precision->value()));
    model->item(item->row(), 2)->setText(QString::number(point.y, 'g', precision->value()));
}

void ParEqTable::fillWithValues()
{
    parEqValues = parEq->getParEqValues(parameters.range, k);

    QList<QStandardItem*> xlist, ylist, tlist;

    QStandardItem *item1 = new QStandardItem("t");
    item1->setFont(boldFont);
    item1->setEditable(false);
    tlist.append(item1);

    QStandardItem *item2 = new QStandardItem("x");
    item2->setFont(boldFont);
    item2->setEditable(false);
    xlist.append(item2);

    QStandardItem *item3 = new QStandardItem("y");
    item3->setFont(boldFont);
    item3->setEditable(false);
    ylist.append(item3);

    for(int i = 0 ; i < parEqValues.tValues.size(); i++)
    {
        QStandardItem *t = new QStandardItem();
        t->setEditable(true);
        t->setText(QString::number(parEqValues.tValues[i], 'g', precision->value()));
        tlist.append(t);

        QStandardItem *x = new QStandardItem();
        x->setEditable(false);
        x->setText(QString::number(parEqValues.xValues[i], 'g', precision->value()));
        xlist.append(x);

        QStandardItem *y = new QStandardItem();
        y->setEditable(false);
        y->setText(QString::number(parEqValues.yValues[i], 'g', precision->value()));
        ylist.append(y);
    }

    model->appendColumn(tlist);
    model->appendColumn(xlist);
    model->appendColumn(ylist);
}

void ParEqTable::emptyCellsFill()
{
    QList<QStandardItem*> xlist, ylist, tlist;

    QStandardItem *item1 = new QStandardItem("t");
    item1->setFont(boldFont);
    item1->setEditable(false);
    tlist.append(item1);

    QStandardItem *item2 = new QStandardItem("x");
    item2->setFont(boldFont);
    item2->setEditable(false);
    xlist.append(item2);

    QStandardItem *item3 = new QStandardItem("y");
    item2->setFont(boldFont);
    item2->setEditable(false);
    ylist.append(item3);

    for(int i = 0 ; i < parEqValues.tValues.size(); i++)
    {
        QStandardItem *t = new QStandardItem();
        t->setEditable(true);
        tlist.append(t);
        parEqValues.tValues << NAN;

        QStandardItem *x = new QStandardItem();
        x->setEditable(false);
        xlist.append(x);
        parEqValues.xValues << NAN;

        QStandardItem *y = new QStandardItem();
        y->setEditable(false);
        ylist.append(y);
        parEqValues.yValues << NAN;
    }

    model->appendColumn(tlist);
    model->appendColumn(xlist);
    model->appendColumn(ylist);
}
