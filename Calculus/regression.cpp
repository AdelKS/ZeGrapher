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





#include "regression.h"

Regression::Regression()
{
    polar = false;
    color = Qt::black;
    valid = false;
}

QList<Point> Regression::getData()
{
    return dataPoints;
}

void Regression::setAbscissaName(QString name)
{
    abscissa = name;
}

void Regression::setOrdinateName(QString name)
{
    ordinate = name;
}

QString Regression::getAbscissaName()
{
    return abscissa;
}

QString Regression::getOrdinateName()
{
    return ordinate;
}

void Regression::setPolar(bool state)
{
    polar = state;
    emit regressionModified();
}

void Regression::setDrawState(bool state)
{
    drawState = state;
    emit regressionModified();
}

void Regression::setDataNumber(int num)
{
    dataNum = num;
}

void Regression::setColor(QColor col)
{
    color = col;
    emit regressionModified();
}

bool Regression::isPolar()
{
    return polar;
}

bool Regression::getDrawState()
{
    return drawState && valid;
}

int Regression::getDataNum()
{
    return dataNum;
}

Range Regression::getDrawRange()
{
    return range;
}

QColor Regression::getColor()
{
    return color;
}

Regression::~Regression()
{

}

