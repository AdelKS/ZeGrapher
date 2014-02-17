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


#ifndef PARCONFWIDGET_H
#define PARCONFWIDGET_H

#include "Structures.h"
#include "treecreator.h"
#include "exprcalculator.h"

class ParConfWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ParConfWidget(QChar parName, bool withAnimateButton = false, bool withKeepTracksButton = false);
    ~ParConfWidget();


    void validate();
    void setAnimationEnabled(bool enabled);
    void setAnimationChecked(bool checked);
    void setFuncsList(QList<FuncCalculator *> list);

    bool isValid();
    bool doesKeepTracks();
    bool isAnimateChecked();

    Range getRange(double k = 0);


signals:
    void k_typed(bool is_k_present);
    void animateToggled(bool toggled);
    void returnPressed();

protected slots:
    void checkExprForK(QString expr);
    void resetPaletteForStartLineEdit();
    void resetPaletteForStepLineEdit();
    void resetPaletteForEndLineEdit();

protected:
    void addConfWidgets(QHBoxLayout *layout);
    void updateTreeWithExpr(QString &lastExpr, QLineEdit *line, FastTree **treePointerAdress, bool &isExprGood);

    QLineEdit *start, *step, *end;
    QCheckBox *animate, *keepTracks;
    TreeCreator treeCreator;
    ExprCalculator *calculator;
    FastTree *startTree, *stepTree, *endTree;
    QString lastStartExpr, lastStepExpr, lastEndExpr;
    QPalette validPalette, invalidPalette, neutralPalette;
    Range defaultRange;

    bool kState, valid, keepTracksButtonAvailable, isStepGood, isEndGood, isStartGood;

    
};

#endif // PARCONFWIDGET_H
