/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane <adel.ks@zegrapher.com>
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



#ifndef TANGENTWIDGET_H
#define TANGENTWIDGET_H

#include "Structures.h"
#include "Widgets/qcolorbutton.h"
#include "Calculus/funccalculator.h"
#include "Calculus/exprcalculator.h"
#include "Widgets/funcwidget.h"

class TangentWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TangentWidget(int id, QList<FuncCalculator *> calcsList, QList<FuncWidget*> list, QColor col);
    void calculateTangentPoints(double xUnit, double yUnit);
    void resizeTangent(double dx, double side);
    void changeID(int id);
    void move(double newPos);
    void validate();
    QColor getColor();

    bool isTangentValid();
    TangentPoints getCaracteristicPoints();

signals:
    void removeMe(TangentWidget *widget);
    void drawStateChanged();
    void returnPressed();

protected slots:
    void resetPalette();
    void emitRemoveMeSignal();
    void newFuncChoosen(int funcNum);
    void newFuncParState(int funcNum);
    void kValueLineEdited();
    
protected:
    void addWidgets();

    QList<FuncCalculator*> funcCalculators;
    QList<FuncWidget*> funcWidgets;
    ExprCalculator *exprCalculator;

    QPalette validPalette, neutralPalette, invalidPalette;
    QCheckBox *drawCheckBox;
    QLabel *nameLabel, *kTextLabel;
    QComboBox *functionsComboBox;
    QLineEdit *tangentPos, *slopeLineEdit, *ordinateAtOriginLineEdit, *kValueLineEdit;
    QColorButton *colorButton;
    QStringList funcNames;
    TangentPoints tangentPoints;
    double lenght, pos, a, k, raty, ratx;
    int tangentID, funcID;
    bool isValid;
    
};

#endif // TANGENTWIDGET_H
