/****************************************************************************
**  Copyright (c) 2016, Adel Kara Slimane <adel.ks@zegrapher.com>
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






#ifndef SEQCALCULATOR_H
#define SEQCALCULATOR_H

#include "Structures.h"
#include "treecreator.h"
#include "funccalculator.h"
#include "colorsaver.h"

class SeqCalculator : public QObject
{
    Q_OBJECT

public:
    SeqCalculator(int id, QString name, QLabel *errorLabel);

    bool validateFirstValsExpr(QString expr);
    bool validateSeqExpr(QString expr);
    bool check_called_funcs_and_seqs_validity();
    bool checkByCalculatingFirstValuesTrees();
    bool checkByCalculatingValues();
    bool canBeCalled();  
    bool isSeqValid();
    bool isSeqParametric();
    bool getDrawState();

    void setFuncsPointers(QList<FuncCalculator*> otherFuncs);
    void setSeqsPointers(QList<SeqCalculator*> otherSeqs);   
    void setParametricInfo(bool parametric, Range parRange);
    void setInvalid();

    void setColorSaver(ColorSaver *colsaver);
    ColorSaver* getColorSaver();

    int getDrawsNum();
    int get_nMin();    

    Range getKRange();
    double getSeqValue(double n, bool &ok, int index_k = 0);
    double getCustomSeqValue(double n, bool &ok, double k_value);

public slots:
    void set_nMin(int val);
    void setDrawState(bool draw);

signals:
    void colorChanged(int id);

protected:

    void addRefFuncsPointers(); 
    void deleteFirstValsTrees();
    bool checkCalledFuncsValidity(QString str);
    bool checkCalledSeqsValidity(QString str);
    bool calculateAndSaveFirstValuesTrees();
    void updateSeqValuesSize();

    double calculateFromTree(FastTree *tree, double n, bool &ok);

    bool validateSeqFirstValsTrees();
    bool saveSeqValues(double nMax);
    bool saveCustomSeqValues(double nMax);
    bool verifyAskedTerm(double n);
    bool verifyOtherSeqAskedTerm(double n, int id);

    QLabel *errorMessageLabel;

    int seqNum, kPos, nMin, drawsNum;
    bool isExprValidated, areFirstValsValidated, isParametric, isValid, blockCalculatingFromTree, drawState, isKRangeValid;
    double custom_k, k;
    ColorSaver *colorSaver;
    Range kRange;
    TreeCreator treeCreator, firstValsTreeCreator;
    FastTree *seqTree;
    QString expression, firstValsExpr, seqName;
    QStringList seqsNames;
    QList<FuncCalculator*> funcCalculatorsList;
    QList<SeqCalculator*> seqCalculatorsList;
    QList<double (*)(double)> refFuncs;

    QList<FastTree*> firstValsTrees;
    QList< QList<double> > seqValues;    
};

#endif // SEQCALCULATOR_H
