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


#include "ValuesTable/valuestableconf.h"

ValuesTableConf::ValuesTableConf(Informations *info, QWidget *parent) :
    QWidget(parent)
{
    infoClass = info;

    funcs = info->getFuncsList();
    seqs = info->getSeqsList();
    parEqs = info->getParEqsList();

    exprCalc = new ExprCalculator(false, funcs);

    connect(infoClass, SIGNAL(updateOccured()), this, SLOT(updateNameCombo()));

    functions << "f" << "g" << "h" << "p" << "r" << "m";
    sequences << "u" << "v" << "l" << "w" << "q" << "z";

    QVBoxLayout *principalLayout = new QVBoxLayout();
    principalLayout->addStretch();

    QFormLayout *typeLayout = new QFormLayout();

    typeCombo = new QComboBox();
    typeCombo->addItem("");
    typeCombo->addItem(tr("Fonction"));
    typeCombo->addItem(tr("Suite"));
    typeCombo->addItem(tr("Equation paramétrique"));
    typeCombo->setCurrentIndex(0);

    connect(typeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateNameCombo()));

    nameCombo = new QComboBox();
    nameCombo->setEnabled(false);

    typeLayout->addRow(tr("Type :"), typeCombo);
    typeLayout->addRow(tr("Nom :"), nameCombo);

    principalLayout->addLayout(typeLayout);

    QGroupBox *entryValues = new QGroupBox(tr("Valeurs d'entrée :"));

    QVBoxLayout *entryValuesLayout = new QVBoxLayout();

    //------------- From current graphic

    fromCurrentGraphic = new QRadioButton(tr("Valeurs du graphique."));
    fromCurrentGraphic->setChecked(true);

    //--------------- Manual Entry

    QVBoxLayout *manualEntryLayout = new QVBoxLayout();
    manualEntryLayout->setSpacing(3);

    manualEntry = new QRadioButton(tr("Manuellement."));

    QHBoxLayout *emptyCellsNumLayout = new QHBoxLayout();
    emptyCellsNumLayout->setMargin(0);
    emptyCellsNumLayout->setSpacing(6);

    QLabel *emptyCellsNumText = new QLabel(tr("Cases vides :"));
    emptyCellsNumText->setEnabled(false);

    connect(manualEntry, SIGNAL(toggled(bool)), emptyCellsNumText, SLOT(setEnabled(bool)));

    emptyCellsNum = new QSpinBox();
    emptyCellsNum->setValue(10);
    emptyCellsNum->setMinimum(10);
    emptyCellsNum->setMaximum(100);
    emptyCellsNum->setMaximumHeight(25);
    emptyCellsNum->setEnabled(false);

    connect(manualEntry, SIGNAL(toggled(bool)), emptyCellsNum, SLOT(setEnabled(bool)));

    emptyCellsNumLayout->addSpacing(15);
    emptyCellsNumLayout->addWidget(emptyCellsNumText);
    emptyCellsNumLayout->addWidget(emptyCellsNum);
    emptyCellsNumLayout->addStretch();

    manualEntryLayout->addWidget(manualEntry);
    manualEntryLayout->addLayout(emptyCellsNumLayout);
    //------------------- Predefined Entry

    QVBoxLayout *predefinedEntryLayout = new QVBoxLayout();
    predefinedEntryLayout->setSpacing(3);

    predefined = new QRadioButton(tr("Prédéfinis"));

    QHBoxLayout *start_step_Layout = new QHBoxLayout();
    start_step_Layout->setSpacing(6);

    QLabel *startText = new QLabel(tr("Début:"));
    startText->setEnabled(false);

    connect(predefined, SIGNAL(toggled(bool)), startText, SLOT(setEnabled(bool)));

    startValue = new QLineEdit();
    startValue->setMaximumHeight(25);
    startValue->setEnabled(false);

    connect(predefined, SIGNAL(toggled(bool)), startValue, SLOT(setEnabled(bool)));

    QLabel *stepText = new QLabel(tr("Pas:"));
    stepText->setEnabled(false);

    connect(predefined, SIGNAL(toggled(bool)), stepText, SLOT(setEnabled(bool)));

    stepValue = new QLineEdit();
    stepValue->setMaximumHeight(25);
    stepValue->setEnabled(false);

    connect(predefined, SIGNAL(toggled(bool)), stepValue, SLOT(setEnabled(bool)));

    start_step_Layout->addSpacing(15);
    start_step_Layout->addWidget(startText);
    start_step_Layout->addWidget(startValue);
    start_step_Layout->addWidget(stepText);
    start_step_Layout->addWidget(stepValue);
    start_step_Layout->addStretch();

    QHBoxLayout *cellsNumLayout = new QHBoxLayout();
    cellsNumLayout->setSpacing(6);

    QLabel *cellsNumText = new QLabel(tr("Cases :"));
    cellsNumText->setEnabled(false);

    connect(predefined, SIGNAL(toggled(bool)), cellsNumText, SLOT(setEnabled(bool)));

    cellsNum = new QSpinBox();
    cellsNum->setMaximumHeight(25);
    cellsNum->setValue(10);
    cellsNum->setMinimum(10);
    cellsNum->setMaximum(200);
    cellsNum->setEnabled(false);

    connect(predefined, SIGNAL(toggled(bool)), cellsNum, SLOT(setEnabled(bool)));

    cellsNumLayout->addSpacing(15);
    cellsNumLayout->addWidget(cellsNumText);
    cellsNumLayout->addWidget(cellsNum);
    cellsNumLayout->addStretch();

    predefinedEntryLayout->addWidget(predefined);
    predefinedEntryLayout->addLayout(start_step_Layout);
    predefinedEntryLayout->addLayout(cellsNumLayout);

    //---------------------------------------- Add them to the layout for QGroupBox

    entryValuesLayout->addWidget(fromCurrentGraphic);
    entryValuesLayout->addLayout(manualEntryLayout);
    entryValuesLayout->addLayout(predefinedEntryLayout);

    entryValues->setLayout(entryValuesLayout);

    //----------------------------------------- Add QGroupBox to the principal Layout and a "apply" button

    principalLayout->addWidget(entryValues);

    QHBoxLayout *applyButtonLayout = new QHBoxLayout();
    QPushButton *applyButton = new QPushButton(tr("Appliquer"));

    connect(applyButton, SIGNAL(released()), this, SLOT(apply()));

    applyButtonLayout->addStretch();
    applyButtonLayout->addWidget(applyButton);
    applyButtonLayout->addStretch();

    principalLayout->addLayout(applyButtonLayout);
    principalLayout->addStretch();

    setLayout(principalLayout);
}

void ValuesTableConf::updateNameCombo()
{
    int index = typeCombo->currentIndex();

    nameCombo->clear();
    nameCombo->addItem("");
    typesNameMap.clear();
    typesNameMap << 0;

    if(index == FUNCTION)// FUNCTION
    {
        nameCombo->setEnabled(true);

         for(short i = 0; i < funcs.size(); i++)
         {
             if(funcs[i]->isFuncValid())
             {
                 typesNameMap << i;
                 nameCombo->addItem(functions[i]);
             }
         }

    }
    else if(index == SEQUENCE)// SEQUENCE in the combo is 2, and also the case in the defines in "structures.h"
    {
         nameCombo->setEnabled(true);       

         for(short i = 0; i < seqs.size(); i++)
         {
             if(seqs[i]->isSeqValid())
             {
                 typesNameMap << i;
                 nameCombo->addItem(sequences[i]);
             }
         }

    }
    else if(index == PARAMETRIC_EQ)
    {
         nameCombo->setEnabled(true);        

         for(short i = 0; i < parEqs->size(); i++)
         {
             if(parEqs->at(i)->isValid())
             {
                 typesNameMap << i;
                 nameCombo->addItem("(P" + QString::number(i+1) + ")");
             }
         }        
    }
    else nameCombo->setEnabled(false);

    nameCombo->setCurrentIndex(0);
}

static double fipart(double x)
{
    if(x < 0)
    {
        return ceil(x);
    }
    else
    {
        return floor(x);
    }
}

bool ValuesTableConf::verifySeqInfo(double start, double step)
{
    if(start != fipart(start))
    {
        QMessageBox::warning(window(), tr("Erreur"), tr("La valeur de départ doit être un entier."));
        return false;
    }
    if(start < seqs[0]->get_nMin())
    {
        QMessageBox::warning(window(), tr("Erreur"), tr("La valeur de départ doit être supérieure à n<sub>min</sub>."));
        return false;
    }
    if(step != fipart(step) || step < 0)
    {
        QMessageBox::warning(window(), tr("Erreur"), tr("La valeur du pas doit être un entier positif."));
        return false;
    }

    return true;
}

void ValuesTableConf::apply()
{
    if(typeCombo->currentIndex() == 0)
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez selectionner un type"));
        return;
    }
    else if(nameCombo->currentIndex() == 0)
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez selectionner un nom"));
        return;
    }

    ValuesTableParameters parameters;

    if(fromCurrentGraphic->isChecked())
    {
        parameters.entryType = FROM_CURRENT_GRAPHIC;
    }
    else if(manualEntry->isChecked())
    {
        parameters.entryType = MANUAL_ENTRY;
        parameters.emptyCellsCount = emptyCellsNum->value();
    }
    else //predefined entry
    {
        parameters.entryType = PREDEFINED_ENTRY;

        bool ok = true;

        parameters.range.start = exprCalc->calculateExpression(startValue->text(), ok);
        if(!ok)
        {
            QMessageBox::warning(this, tr("Erreur"), tr("Erreur de saisie dans la valeur de départ."));
            return;
        }

        parameters.range.step = exprCalc->calculateExpression(stepValue->text(), ok);
        if(!ok)
        {
            QMessageBox::warning(this, tr("Erreur"), tr("Erreur de saisie dans la valeur du pas."));
            return;
        }

        if(typeCombo->currentIndex() == SEQUENCE)
            if(!verifySeqInfo(parameters.range.start, parameters.range.step))
                return;

        parameters.range.end = parameters.range.start + parameters.range.step * (double)cellsNum->value();
    }

    parameters.funcType = typeCombo->currentIndex();
    parameters.id = typesNameMap[nameCombo->currentIndex()];
    parameters.name = nameCombo->currentText();

    emit next(parameters);
}

ValuesTableConf::~ValuesTableConf()
{
    delete exprCalc;
}
