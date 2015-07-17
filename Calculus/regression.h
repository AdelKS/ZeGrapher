/****************************************************************************
**  Copyright (c) 2015, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.1.
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

#include "Structures.h"

class Regression : public QObject
{
    Q_OBJECT

public:
    explicit Regression();
    ~Regression();

    virtual double eval(double x) const = 0;
    virtual QString getInfo() const = 0;

    QString getAbscissaName();
    QString getOrdinateName();
    bool getDrawState();
    int getDataNum();
    bool isPolar();
    Range getDrawRange();
    QColor getColor();    

signals:
    void regressionModified();

public slots:
    void setAbscissaName(QString name);
    void setOrdinateName(QString name);
    void setColor(QColor col);
    void setDrawState(bool state);
    void setDataNumber(int num);
    void setPolar(bool state);

protected:

    QColor color;
    int dataNum;
    Range range;
    QString abscissa, ordinate;
    bool drawState, polar, valid;

};

#endif // REGRESSION_H
