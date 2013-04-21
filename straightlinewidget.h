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


#ifndef STRAIGHTLINEWIDGET_H
#define STRAIGHTLINEWIDGET_H

#include "Structures.h"
#include "funccalculator.h"
#include "qcolorbutton.h"
#include "exprcalculator.h"

class StraightLineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StraightLineWidget(int id, QList<FuncCalculator *> calcsList);
    void validate();
    void changeID(int id);

    double getOrdinate(double abscissa);
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
    void addWidgets();

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
