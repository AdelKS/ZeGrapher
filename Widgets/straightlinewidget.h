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




#ifndef STRAIGHTLINEWIDGET_H
#define STRAIGHTLINEWIDGET_H

#include "structures.h"
#include "Calculus/funccalculator.h"
#include "Widgets/qcolorbutton.h"
#include "Calculus/exprcalculator.h"

class StraightLineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StraightLineWidget(int id, QList<FuncCalculator *> calcsList, QColor col);
    void validate();
    void changeID(int id);

    double getY(double x);
    double getVerticalPos();

    QColor getColor();

    bool isValid();
    bool isVertical();

signals:
    void removeMe(StraightLineWidget *widget);
    void drawStateChanged();
    void returnPressed();

protected slots:
    void resetPaletteForLineEditA();
    void resetPaletteForLineEditB();
    void resetPaletteForLineEditC();

    void emitRemoveMeSignal();
    
protected:
    void addWidgets(QColor col);

    QList<FuncCalculator*> funcCalculators;
    ExprCalculator *exprCalculator;

    QCheckBox *drawCheckBox;
    QLabel *nameLabel;
    QLineEdit *a, *b, *c;
    QColorButton *colorButton;
    QPalette validPalette, invalidPalette, neutralPalette;

    int lineID;
    bool valid;
    double val_a, val_b, val_c;
    
};

#endif // STRAIGHTLINEWIDGET_H
