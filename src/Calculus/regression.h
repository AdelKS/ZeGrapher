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





#ifndef REGRESSION_H
#define REGRESSION_H

#include <QString>
#include <QColor>

#include "structures.h"

// Remove dependence on Qt, to be usable as a standalone library

class Regression : public QObject
{
    Q_OBJECT

public:
    explicit Regression();

    virtual double eval(double x) const = 0;
    virtual QString getInfo() const = 0;

    QString getAbscissaName();
    QString getOrdinateName();
    bool getDrawState();
    int getDataNum();
    virtual bool isPolar() = 0;
    Range getDrawRange();
    QColor getColor();    

signals:
    void regressionModified();
    void rangeUpdated();

public slots:
    void setAbscissaName(QString name);
    void setOrdinateName(QString name);
    void setColor(QColor col);
    void setDrawState(bool state);

protected:

    QColor color;
    Range range;
    QString abscissa, ordinate;
    bool drawState, valid;

};

#endif // REGRESSION_H
