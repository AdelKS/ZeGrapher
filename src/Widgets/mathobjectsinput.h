/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
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
    explicit MathObjectsInput(QWidget *parent = 0);
    ~MathObjectsInput();

    void closeAllOpenedWindows();

public slots:
    void validateFunctions();
    void validateSequences();
    void validateParametricEquations();

signals:
    void displayKeyboard();
    void showHelpWindow();

protected slots:
    void setInfoClass();
    void keyboardButtonClicked();
    void newFuncParametricState();
    void newSeqParametricState();
    void draw();

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
    Ui::MathObjectsInput *ui;

    QList <QChar> funcNames;
    QList <QChar> seqNames;
    QTextBrowser helpWindow;

    QList<Function*> funcCalcs;
    QList<SeqCalculator*> seqCalcs;

    QList<FuncWidget*> funcWidgets;
    QList<SeqWidget*> seqWidgets;

    QList<QChar> antiderivatives;

    QList<ParEqWidget*> parEqWidgets;
    QList<DataWidget*> dataWidgets;

    ParEqController parEqController;
};

#endif // MATHOBJECTSINPUT_H
