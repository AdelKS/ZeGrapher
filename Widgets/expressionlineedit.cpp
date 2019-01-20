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

#include "expressionlineedit.h"

ExpressionLineEdit::ExpressionLineEdit()
{
    validCSS = "padding-left: 5px; border-radius: 5px; background-color: " + QString(VALID_COLOR) + ";";
    invalidCSS = "padding-left: 5px; border-radius: 5px; background-color: " + QString(INVALID_COLOR) + ";";
    neutralCSS = "padding-left: 5px; border-radius: 5px;" ;

    setStyleSheet(neutralCSS);
}

void ExpressionLineEdit::clear()
{
    setStyleSheet(neutralCSS);
    QLineEdit::clear();
}

void ExpressionLineEdit::setValid()
{
    setStyleSheet(validCSS);
}

void ExpressionLineEdit::setInvalid()
{
    setStyleSheet(invalidCSS);
}

void ExpressionLineEdit::setNeutral()
{
    setStyleSheet(neutralCSS);
}




