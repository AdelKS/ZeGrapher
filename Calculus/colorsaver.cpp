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


#include "Calculus/colorsaver.h"

ColorSaver::ColorSaver(QColor firstANDlast)
{
    curvesNum = 1;
    firstColor = firstANDlast;
    lastColor = firstANDlast;
    colorsList << firstANDlast;
}

QColor ColorSaver::getColor(int index)
{
    return colorsList[index];
}

void ColorSaver::setCurvesNum(double num)
{
    if(curvesNum != num)
    {
        curvesNum = num;
        saveColors();
    }
}

void ColorSaver::setFristColor(QColor col)
{
    firstColor = col;
    saveColors();
    emit colorsChanged();
}

void ColorSaver::setLastColor(QColor col)
{
    lastColor = col;
    saveColors();
    emit colorsChanged();
}

void ColorSaver::saveColors()
{
    colorsList.clear();

    QColor color;
    double dr = lastColor.redF() - firstColor.redF();
    double dg = lastColor.greenF() - firstColor.greenF();
    double db = lastColor.blueF() - firstColor.blueF();

    colorsList << firstColor;

    for(double i = 1 ; i < curvesNum - 1 ; i++)
    {
        color.setRedF(dr * i / curvesNum + firstColor.redF());
        color.setGreenF(dg * i / curvesNum + firstColor.greenF());
        color.setBlueF(db * i / curvesNum + firstColor.blueF());

        colorsList << color;
    }

    colorsList << lastColor;
}

