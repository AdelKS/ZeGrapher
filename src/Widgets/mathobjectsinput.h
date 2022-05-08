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

#ifndef MATHOBJECTSINPUT_H
#define MATHOBJECTSINPUT_H

#include "information.h"
#include "Widgets/funcwidget.h"
#include "Widgets/seqwidget.h"
#include "Widgets/pareqcontroller.h"
#include "Widgets/datawidget.h"

namespace Ui {
    class MathObjectsInput;
}

class MathObjectsInput : public QWidget
{
    Q_OBJECT

public:
    explicit MathObjectsInput(Information *info, QWidget *parent = 0);
    ~MathObjectsInput();

    void closeAllOpenedWindows();

public slots:
    void validateFunctions();
    void validateSequences();
    void validateLines();
    void validateParametricEquations();

signals:
    void displayKeyboard();
    void showHelpWindow();

protected slots:
    void setInfoClass(Information *info);  
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

    void showDataHelpWindow();

protected:
    void addFunctions();
    void addSequences();    
    void saveColors();

private:
    Information *information;
    Ui::MathObjectsInput *ui;

    QList <QChar> funcNames;
    QList <QChar> seqNames;
    QTextBrowser helpWindow;

    QList<FuncCalculator*> funcCalcs;
    QList<SeqCalculator*> seqCalcs;

    QList<FuncWidget*> funcWidgets;
    QList<SeqWidget*> seqWidgets;

    QList<QChar> antiderivatives;

    QList<TangentWidget*> tangentWidgets;
    QList<StraightLineWidget*> straightlineWidgets;
    QList<ParEqWidget*> parEqWidgets;
    QList<DataWidget*> dataWidgets;    

    ParEqController parEqController;
};

#endif // MATHOBJECTSINPUT_H
