/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
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

#include "appsettings.h"

#include <QGuiApplication>
#include <QStyleHints>

QColor ZeAppSettings::getValidSyntax() const
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    return validSyntaxDark;
  else return validSyntaxLight;
}

void ZeAppSettings::colorSchemeChanged()
{
  if (invalidSyntaxDark != invalidSyntaxLight)
    emit invalidSyntaxChanged();
  if (validSyntaxDark != validSyntaxLight)
    emit validSyntaxChanged();
  if (warningSyntaxDark != warningSyntaxLight)
    emit warningSyntaxChanged();
}

QColor ZeAppSettings::getInvalidSyntax() const
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    return invalidSyntaxDark;
  else return invalidSyntaxLight;
}

QColor ZeAppSettings::getWarningSyntax() const
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    return warningSyntaxDark;
  else return warningSyntaxLight;
}

void ZeAppSettings::setValidSyntax(QColor c)
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    validSyntaxDark = c;
  else
    validSyntaxLight = c;

  emit validSyntaxChanged();
}

void ZeAppSettings::setInvalidSyntax(QColor c)
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    invalidSyntaxDark = c;
  else
    invalidSyntaxLight = c;

  emit invalidSyntaxChanged();
}

void ZeAppSettings::setWarningSyntax(QColor c)
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    warningSyntaxDark = c;
  else
    warningSyntaxLight = c;

  emit warningSyntaxChanged();
}
