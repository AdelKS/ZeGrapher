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

#ifndef RANGEADJUSTMENTS_H
#define RANGEADJUSTMENTS_H

#include <QMessageBox>

#include "numberlineedit.h"
#include "information.h"

class RangeAdjustments : public QWidget
{
    Q_OBJECT

public:
    explicit RangeAdjustments(QList<FuncCalculator *> funcsList, QWidget *parent = nullptr);
    GraphRange getRange();
    ~RangeAdjustments();

public slots:
    void resetToStandardView();
    void setGraphRange(const GraphRange &range);
    void disableUserInput(bool disable);
    void setOrthonormal(bool state);
    void processUserInput();
    void apply();

signals:
    void graphRangeChanged(GraphRange range);
    void orthonormalBasisToggled(bool state);
    void resetToDefaultView();

protected:
    void loadDefaults();

    GraphRange graphRange;

    QPushButton *orthonormalButton, *resetButton;
    NumberLineEdit *Xmin, *Xmax, *Ymin, *Ymax;
    QMessageBox *messageBox;
};

#endif // RANGEADJUSTMENTS_H
