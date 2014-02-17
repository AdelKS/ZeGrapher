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


#include "valuestable.h"

ValuesTable::ValuesTable(Informations *info, QWidget *parent) :
    QWidget(parent)
{    
    infoClass = info;

    seqTable = NULL;
    parEqTable = NULL;
    funcTable = NULL;

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);

    containerLayout = new QVBoxLayout;
    containerLayout->setMargin(0);

    setFixedWidth(300);

    QHBoxLayout *removeButtonLayout = new QHBoxLayout;

    QPushButton *removeButton = new QPushButton;
    removeButton->setFixedSize(25,25);
    removeButton->setFlat(true);
    removeButton->setIconSize(QSize(25,25));
    removeButton->setIcon(QIcon(":/icons/remove.png"));

    connect(removeButton, SIGNAL(released()), this, SLOT(emitRemoveSignal()));

    removeButtonLayout->addStretch();
    removeButtonLayout->addWidget(removeButton);

    confWidget = new ValuesTableConf(infoClass);
    connect(confWidget, SIGNAL(next(ValuesTableParameters)), this, SLOT(apply(ValuesTableParameters)));    

    containerLayout->addLayout(removeButtonLayout);
    containerLayout->addWidget(confWidget);

    QFrame *frame1 = new QFrame();
    frame1->setFrameShape(QFrame::VLine);
    frame1->setFrameShadow(QFrame::Sunken);

    QFrame *frame2 = new QFrame();
    frame2->setFrameShape(QFrame::VLine);
    frame2->setFrameShadow(QFrame::Sunken);

    layout->addWidget(frame1);
    layout->addLayout(containerLayout);
    layout->addWidget(frame2);

    setLayout(layout);
}

void ValuesTable::emitRemoveSignal()
{
    emit remove(this);
}

void ValuesTable::previous()
{
    if(funcTable != NULL)
        funcTable->close();
    else if(seqTable != NULL)
        seqTable->close();
    else parEqTable->close();

    setFixedWidth(300);

    confWidget->show();
}

void ValuesTable::apply(ValuesTableParameters parameters)
{
    confWidget->close();

    if(parameters.funcType == FUNCTION)
    {
        if(funcTable == NULL)
        {
            funcTable = new FuncTable(infoClass);
            containerLayout->addWidget(funcTable);

            connect(funcTable, SIGNAL(previous()), this, SLOT(previous()));
        }
        setFixedWidth(310);

        funcTable->setTableParameters(parameters);
        funcTable->show();

        if(seqTable != NULL)
        {
            delete seqTable;
            seqTable = NULL;
        }
        else if(parEqTable != NULL)
        {
            delete parEqTable;
            parEqTable = NULL;
        }
    }
    else if(parameters.funcType == SEQUENCE)
    {
        if(seqTable == NULL)
        {
            seqTable = new SeqTable(infoClass);
            containerLayout->addWidget(seqTable);

            connect(seqTable, SIGNAL(previous()), this, SLOT(previous()));
        }

        seqTable->setTableParameters(parameters);
        seqTable->show();

        setFixedWidth(310);

        if(funcTable != NULL)
        {
            delete funcTable;
            funcTable = NULL;
        }
        else if(parEqTable != NULL)
        {
            delete parEqTable;
            parEqTable = NULL;
        }
    }
    else
    {
        if(parEqTable == NULL)
        {
            parEqTable = new ParEqTable(infoClass);
            containerLayout->addWidget(parEqTable);

            connect(parEqTable, SIGNAL(previous()), this, SLOT(previous()));
        }

        parEqTable->setTableParameters(parameters);
        parEqTable->show();

        setFixedWidth(460);

        if(funcTable != NULL)
        {
            delete funcTable;
            funcTable = NULL;
        }
        else if(seqTable != NULL)
        {
            delete seqTable;
            seqTable = NULL;
        }

    }
}

ValuesTable::~ValuesTable()
{

}
