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



#ifndef ABSTRACTFUNCWIDGET_H
#define ABSTRACTFUNCWIDGET_H

#include "structures.h"
#include "parconfwidget.h"
#include "qcolorbutton.h"
#include "Widgets/expressionlineedit.h"

class AbstractFuncWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractFuncWidget();

signals:
    void returnPressed();

protected:
    void addMainWidgets();

    QCheckBox *drawCheckBox;
    QHBoxLayout *secondContainerLayout;
    QLabel *nameLabel, *errorMessageLabel;
    QWidget *errorMessageWidget;
    ExpressionLineEdit *expressionLineEdit;
    QColorButton *colorButton, *secondColorButton;
    ParConfWidget *kConfWidget;
    TreeCreator treeCreator;

    bool isParametric, isValid;
};

#endif // ABSTRACTFUNCWIDGET_H
