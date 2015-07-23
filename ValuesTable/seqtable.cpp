/****************************************************************************
**  Copyright (c) 2013, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.0.
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


#include "ValuesTable/seqtable.h"

SeqTable::SeqTable(Informations *info) : AbstractTable(), exprCalculator(false, info->getFuncsList())
{
    informations = info;    

    updateTimer->setInterval(2000);
    updateTimer->setSingleShot(true);

    QColor color;
    color.setNamedColor(VALID_COLOR);
    validPalette.setColor(QPalette::Base, color);

    color.setNamedColor(INVALID_COLOR);
    invalidPalette.setColor(QPalette::Base, color);

    connect(informations, SIGNAL(updateOccured()), updateTimer, SLOT(start()));
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTable()));
    connect(precision, SIGNAL(valueChanged(int)), this, SLOT(precisionEdited()));
    connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(cellEdited(QStandardItem*)));
}

void SeqTable::setTableParameters(ValuesTableParameters par)
{
    parameters = par;
    title->setText(tr("Suite: ") + "(" + parameters.name + "<sub>n</sub>)");

    seq = informations->getSeqsList()[parameters.id];

    if(parameters.entryType == FROM_CURRENT_GRAPHIC)
        connect(informations, SIGNAL(updateOccured()), updateTimer, SLOT(start()));

    if(seq->isSeqParametric())
    {
        k = seq->getKRange().start;
        k_parameter_widget->show();
        k_value->setText(QString::number(k, 'g', precision->value()));

        connect(k_value, SIGNAL(returnPressed()), this, SLOT(kValueEdited()));
    }

    updateTable();
}

void SeqTable::precisionEdited()
{
    disableCellEdit = true;

    for(int i = 0; i < xValues.size(); i++)
    {
        if(!std::isnan(xValues[i]))
        {
            model->item(i+1, 0)->setText(QString::number(xValues[i], 'g', precision->value()));
            model->item(i+1, 1)->setText(QString::number(yValues[i], 'g', precision->value()));
        }
    }

    disableCellEdit = false;
}

void SeqTable::kValueEdited()
{
    bool ok = true;
    double temp = exprCalculator.calculateExpression(k_value->text(), ok);

    if(!ok)
        k_value->setPalette(invalidPalette);
    else k_value->setPalette(validPalette);

    k = temp;
    updateTable();
}

void SeqTable::updateTable()
{
    model->clear();
    xValues.clear();
    yValues.clear();

    if(!seq->isSeqValid())
        return;

    if(seq->isSeqParametric() && k_parameter_widget->isHidden())
    {
        k = seq->getKRange().start;
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

void SeqTable::fillFromRange()
{
    if(parameters.entryType == FROM_CURRENT_GRAPHIC)
    {
        GraphRange range = informations->getRange();

        parameters.range.start = trunc(range.Xmin / range.Xscale) * range.Xscale;
        parameters.range.step = range.Xscale;
        parameters.range.end = range.Xmax;

        if(seq->get_nMin() > parameters.range.end)
            return;

        if(seq->get_nMin() > parameters.range.start)
            parameters.range.start = seq->get_nMin();

        if(parameters.range.step != ceil(parameters.range.step))
            parameters.range.step = ceil(parameters.range.step);
    }

    add_x_values();

    if(seq->isSeqParametric())
        add_parSeq_y_values();
    else add_y_values();

    tableView->setModel(model);
}

void SeqTable::cellEdited(QStandardItem *item)
{
    if(item->text().isEmpty() || item->column() != 0 || disableCellEdit)
        return;

    bool ok = true;
    double x = exprCalculator.calculateExpression(item->text(), ok), y;
    if(!ok)
    {
         QMessageBox::warning(this, tr("Erreur"), tr("Erreur de syntaxe dans cette entrée"));
         return;
    }

    if(x != floor(x) || x < seq->get_nMin())
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Vous devez saisir un nombre entier supérieur à n<sub>min</sub>"));
        return;
    }

    if(seq->isSeqParametric())
    {        
        y = seq->getCustomSeqValue(x, ok, k);
        if(!ok)
        {
               QMessageBox::critical(this, tr("Erreur"), tr("Erreur dans le calcul des termes de cette suite paramètrique, changer la valeur du paramètre pourrait résoudre le problème."));
               model->item(item->row(), 1)->setText("");
               return;
        }     
    }
    else y = seq->getSeqValue(x, ok);

    if(!ok)
        return;

    xValues[item->row()-1] = x; // row-1 because there is a "x" cell on the first cell of the first column
    yValues[item->row()-1] = y;

    model->item(item->row(), 1)->setText(QString::number(y, 'g', precision->value()));

}

void SeqTable::add_x_values()
{
    QList<QStandardItem*> liste;
    liste.reserve(100);

    QStandardItem *item2 = new QStandardItem("n");
    item2->setFont(boldFont);
    item2->setEditable(false);
    liste.append(item2);

    double x = parameters.range.start;

    while(x <= parameters.range.end)
    {
        QStandardItem *item = new QStandardItem(QString::number(x, 'g', precision->value()));
        liste.append(item);
        xValues << x;
        x += parameters.range.step;
    }

    model->appendColumn(liste);
}

void SeqTable::add_y_values()
{
    QList<QStandardItem*> liste;

    double x = parameters.range.start, y;
    bool ok = true;

    QStandardItem *funcNameCell = new QStandardItem(parameters.name.toUpper() + "n");
    funcNameCell->setFont(boldFont);
    funcNameCell->setEditable(false);

    liste.append(funcNameCell);

    while(x <= parameters.range.end)
    {
        y = seq->getSeqValue(x, ok);

        if(!ok)
            return;

        yValues << y;

        QStandardItem *item = new QStandardItem();
        item->setEditable(false);
        item->setText(QString::number(y, 'g', precision->value()));
        liste.append(item);

        x += parameters.range.step;
    }

    model->appendColumn(liste);
}

void SeqTable::add_parSeq_y_values()
{
    QList<QStandardItem*> liste;

    double x = parameters.range.start, y;
    bool ok = true;

    QStandardItem *funcNameCell = new QStandardItem(parameters.name.toUpper() + "n");
    funcNameCell->setFont(boldFont);
    funcNameCell->setEditable(false);

    liste.append(funcNameCell);

    while(x <= parameters.range.end)
    {
        y = seq->getCustomSeqValue(x, ok, k);

        if(!ok)
        {
            QMessageBox::critical(this, tr("Erreur"), tr("Erreur dans le calcul des termes de cette suite paramètrique, changer la valeur du paramètre pourrait résoudre le problème."));
            model->clear();
            return;
        }

        yValues << y;

        QStandardItem *item = new QStandardItem();
        item->setEditable(false);
        item->setText(QString::number(y, 'g', precision->value()));
        liste.append(item);

        x += parameters.range.step;
    }

    model->appendColumn(liste);
}

void SeqTable::emptyCellsFill()
{
    QList<QStandardItem*>liste;
    liste.reserve(100);

    QStandardItem *item2 = new QStandardItem("x");
    item2->setFont(boldFont);
    item2->setEditable(false);
    liste.append(item2);

    for(int i = 0 ; i < parameters.emptyCellsCount; i++)
    {
        QStandardItem *item = new QStandardItem();
        liste << item;
        xValues << NAN;
        yValues << NAN;
    }

    model->appendColumn(liste);

    liste.clear();

    QStandardItem *item4 = new QStandardItem(parameters.name + "(x)");
    item4->setFont(boldFont);
    item4->setEditable(false);

    liste.append(item4);

    for(int i = 0 ; i < parameters.emptyCellsCount; i++)
    {
        QStandardItem *item = new QStandardItem();
        item->setEditable(false);
        liste << item;
    }

    model->appendColumn(liste);
}
