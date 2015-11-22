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



#ifndef WINDOWBOUNDARY_H
#define WINDOWBOUNDARY_H

#include "Structures.h"
#include "information.h"
#include "Calculus/exprcalculator.h"


namespace Ui {
    class WindowBoundary;
}

class WindowBoundary : public QWidget
{
    Q_OBJECT

public:
    explicit WindowBoundary(Information *info);
    ~WindowBoundary();

public slots:
    void resetToStandardView();
    void updateWidgets();


private slots:
    void standardView();
    void orthonormal(bool state);
    void apply();

signals:
    void newWindow(GraphRange window);//FIXME never emitted
    void orthonormalBasis(bool state);

private:

    Information *information;
    Ui::WindowBoundary *ui;
    QMessageBox *messageBox;    
    ExprCalculator *calculator;
};

#endif // WINDOWBOUNDARY_H
