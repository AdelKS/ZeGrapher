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


#include "Widgets/qcolorbutton.h"

QColorButton::QColorButton(QWidget *parent) : QPushButton(parent)
{
    buttonSize = 20;
    setFixedSize(buttonSize, buttonSize);

    setColor(Qt::black);
    setFocusPolicy(Qt::NoFocus);

    connect(this, SIGNAL(released()), this, SLOT(openColorDialog()));
}

QColorButton::QColorButton(QColor color, QWidget *parent) : QColorButton(parent)
{
    setColor(color);
}

void QColorButton::openColorDialog()
{
    setColor(QColorDialog::getColor(currentColor, window()));
}

void QColorButton::setColor(QColor color)
{
    currentColor = color;
    int h1, s1, v1;
    color.getHsl(&h1, &s1, &v1);

    int h2 = h1, s2 = s1, v2 = v1;

    int DeltaL = 100;

    if(v1 > 255 - DeltaL)
    {
        v2 = 255;
        v1 -= v1 - (255 - DeltaL);
    }
    else v2 = v1 + DeltaL;

    QString gradientText = "background: qradialgradient(cx:0.4, cy:0.4, radius: 0.4, fx:0.4, fy:0.4, "
                           "stop:0 hsv(" + QString::number(h2) + "," + QString::number(s2) + "," + QString::number(v2) + "), "
                           "stop:1 hsv(" + QString::number(h1) + "," + QString::number(s1) + "," + QString::number(v1) + ")) ; ";


    /**QString gradientText2 = "background: qradialgradient(cx:0.4, cy:0.4, radius: 0.7, fx:0.4, fy:0.4, "
                           "stop:0 hsv(" + QString::number(h1) + "," + QString::number(s1) + "," + QString::number(v1) + "), "
                           "stop:0.4 hsv(" + QString::number(h1) + "," + QString::number(s1) + "," + QString::number(v1) + "), "
                           "stop:1 hsv(" + QString::number(h2) + "," + QString::number(s2) + "," + QString::number(v2) + ")) ; ";**/
    setStyleSheet(gradientText + "; border-radius:" + QString::number(buttonSize/2) + "px");

    emit colorChanged(currentColor);
}

QColor QColorButton::getCurrentColor()
{
    return currentColor;
}
