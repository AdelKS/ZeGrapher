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




#include "Widgets/qcolorbutton.h"

QColorButton::QColorButton(QColor color)
{
    setColor(color);
    setFixedSize(18, 18);
    setFocusPolicy(Qt::NoFocus);

    connect(this, SIGNAL(released()), this, SLOT(openColorDialog()));
}

void QColorButton::openColorDialog()
{
    setColor(QColorDialog::getColor(currentColor, window()));
}

void QColorButton::setColor(QColor color)
{
    currentColor = color;
    setStyleSheet("background-color:"+color.name()+"; border-width: 1px; border-color: #787878 ; border-style: solid; "
                                                   "border-top-right-radius: 8; border-bottom-left-radius: 8;"
                                                   "border-top-left-radius: 2; border-bottom-right-radius: 2;");

    emit colorChanged(currentColor);
}

QColor QColorButton::getCurrentColor()
{
    return currentColor;
}
