/****************************************************************************
**  Copyright (c) 2024, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
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
#include "information.h"

ExpressionLineEdit::ExpressionLineEdit(QWidget *parent) : QLineEdit(parent)
{
    updateBackground();
    connect(&information, SIGNAL(appSettingsChanged()), this, SLOT(updateBackground()));
}

void ExpressionLineEdit::updateBackground()
{
    validCSS = "padding-left: 5px; border-radius: 5px; background-color: " + information.getAppSettings().validSyntax.name() + ";";
    invalidCSS = "padding-left: 5px; border-radius: 5px; background-color: " + information.getAppSettings().invalidSyntax.name() + ";";
    neutralCSS = "padding-left: 5px; border-radius: 5px;";

    switch(state)
    {
    case State::INVALID:
        setStyleSheet(invalidCSS);
        break;
    case State::VALID:
        setStyleSheet(validCSS);
        break;
    case State::NEUTRAL:
        setStyleSheet(neutralCSS);
        break;
    }
}

void ExpressionLineEdit::clear()
{
    state = State::NEUTRAL;
    QLineEdit::clear();
    updateBackground();
}

void ExpressionLineEdit::setValid()
{
    state = State::VALID;
    updateBackground();
}

void ExpressionLineEdit::setInvalid()
{
    state = State::INVALID;
    updateBackground();
}

void ExpressionLineEdit::setNeutral()
{
    state = State::NEUTRAL;
    updateBackground();
}
