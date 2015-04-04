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

class Regression
{
public:
    Regression();

    virtual double eval(double x);
    virtual QString getAbscissaName();
    virtual QString getOrdinateName();
    virtual QString getInfo();
    virtual bool getDrawState();


    ~Regression();
};

#endif // REGRESSION_H
