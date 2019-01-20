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




#ifndef RANGEADJUSTMENTS_H
#define RANGEADJUSTMENTS_H

#include "structures.h"
#include "information.h"
#include "Calculus/exprcalculator.h"
#include "Widgets/numberlineedit.h"


namespace Ui {
    class RangeAdjustments;
}

class RangeAdjustments : public QWidget
{
    Q_OBJECT

public:
    explicit RangeAdjustments(QList<FuncCalculator *> funcsList, GraphRange graphRange, GraphTickIntervals tickIntervals, QWidget *parent = nullptr);
    ~RangeAdjustments();

    void setMargin(int margin);

public slots:
    void resetToStandardView();   
    void setGraphRange(GraphRange range);
    void setGraphTickIntervals(GraphTickIntervals tickIntervals);
    void disableRangeWidgets(bool disable);
    void hideViewOptions(bool hide);

protected slots:
    void onRangeChange();
    void onTickIntervalChange();

    void standardView();
    void setOrthonormal(bool state);

signals:
    void graphRangeChanged(GraphRange range);
    void graphTickIntervalsChanged(GraphTickIntervals tickIntervals);
    void orthonormalBasisToggled(bool state);

private:
    GraphRange graphRange;
    GraphTickIntervals tickIntervals;
    NumberLineEdit *Xmin, *Xmax, *Xstep, *Ymin, *Ymax, *Ystep;
    Ui::RangeAdjustments *ui;
    QMessageBox *messageBox;    
    ExprCalculator *calculator;
};

#endif // RANGEADJUSTMENTS_H
