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


#ifndef FENETREFONCTIONS_H
#define FENETREFONCTIONS_H

#include "informations.h"
#include "funcwidget.h"
#include "seqwidget.h"
#include "pareqcontroller.h"
#include "datawidget.h"

namespace Ui {
    class FenetreFonctions;
}

class FenetreFonctions : public QWidget
{
    Q_OBJECT

public:
    explicit FenetreFonctions(Informations *info);
     ~FenetreFonctions();  

public slots:
    void validateFunctions();
    void validateSequences();
    void validateLines();
    void validateParametricEquations();

signals:
    void displayKeyboard();

protected slots:
    void setInfoClass(Informations *info);  
    void keyboardButtonClicked();
    void newFuncParametricState();
    void newSeqParametricState();
    void draw();

    void addTangent();   
    void removeTangent(TangentWidget *widget);

    void addStraightline();
    void removeStraightline(StraightLineWidget *widget);

    void addParEq();
    void removeParEq(ParEqWidget *widget);

    void addDataWidget();
    void removeDataWidget(DataWidget *widget);

protected:
    void addFunctions();
    void addSequences();

private:
    Informations *informations;
    Ui::FenetreFonctions *ui;    

    QList <QChar> funcNames;
    QList <QChar> seqNames;

    QList<FuncCalculator*> funcCalcs;
    QList<SeqCalculator*> seqCalcs;

    QList<FuncWidget*> funcWidgets;
    QList<SeqWidget*> seqWidgets;

    QList<QChar> antiderivatives;   
    ParEqController *parEqController;

    QList<TangentWidget*> tangentWidgets;
    QList<StraightLineWidget*> straightlineWidgets;
    QList<ParEqWidget*> parEqWidgets;
    QList<DataWidget*> dataWidgets;
};

#endif // FENETREFONCTIONS_H
