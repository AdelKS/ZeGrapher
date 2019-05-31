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




#include "ValuesTable/valuestable.h"

ValuesTable::ValuesTable(Information *info, QWidget *parent) :
    QWidget(parent)
{    
    infoClass = info;

    seqTable = nullptr;
    parEqTable = nullptr;
    funcTable = nullptr;

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);

    containerLayout = new QVBoxLayout;
    containerLayout->setMargin(3);

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
    if(funcTable != nullptr)
        funcTable->close();
    else if(seqTable != nullptr)
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
        if(funcTable == nullptr)
        {
            funcTable = new FuncTable(infoClass);
            containerLayout->addWidget(funcTable);

            connect(funcTable, SIGNAL(previous()), this, SLOT(previous()));
        }
        setFixedWidth(310);

        funcTable->setTableParameters(parameters);
        funcTable->show();

        if(seqTable != nullptr)
        {
            delete seqTable;
            seqTable = nullptr;
        }
        else if(parEqTable != nullptr)
        {
            delete parEqTable;
            parEqTable = nullptr;
        }
    }
    else if(parameters.funcType == SEQUENCE)
    {
        if(seqTable == nullptr)
        {
            seqTable = new SeqTable(infoClass);
            containerLayout->addWidget(seqTable);

            connect(seqTable, SIGNAL(previous()), this, SLOT(previous()));
        }

        seqTable->setTableParameters(parameters);
        seqTable->show();

        setFixedWidth(310);

        if(funcTable != nullptr)
        {
            delete funcTable;
            funcTable = nullptr;
        }
        else if(parEqTable != nullptr)
        {
            delete parEqTable;
            parEqTable = nullptr;
        }
    }
    else
    {
        if(parEqTable == nullptr)
        {
            parEqTable = new ParEqTable(infoClass);
            containerLayout->addWidget(parEqTable);

            connect(parEqTable, SIGNAL(previous()), this, SLOT(previous()));
        }

        parEqTable->setTableParameters(parameters);
        parEqTable->show();

        setFixedWidth(460);

        if(funcTable != nullptr)
        {
            delete funcTable;
            funcTable = nullptr;
        }
        else if(seqTable != nullptr)
        {
            delete seqTable;
            seqTable = nullptr;
        }

    }
}

ValuesTable::~ValuesTable()
{

}
