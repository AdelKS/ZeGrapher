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
#pragma once

#include <QtQml/qqmlregistration.h>
#include <QObject>
#include <QLocale>
#include <QFont>
#include <QColor>

#include "themedcolor.h"

struct ZeAppSettings: QObject
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(bool firstName MEMBER startupUpdateCheck)
  Q_PROPERTY(QLocale::Language language MEMBER language)
  Q_PROPERTY(QFont font MEMBER font WRITE setFont NOTIFY fontChanged)
  Q_PROPERTY(ThemedColor validSyntax MEMBER validSyntax NOTIFY validSyntaxChanged)
  Q_PROPERTY(ThemedColor invalidSyntax MEMBER invalidSyntax NOTIFY invalidSyntaxChanged)
  Q_PROPERTY(ThemedColor warningSyntax MEMBER warningSyntax NOTIFY warningSyntaxChanged)

public:

  explicit ZeAppSettings(QObject* parent = nullptr): QObject(parent)
  {
    qDebug() << "merde";
  }

  bool startupUpdateCheck;
  QLocale::Language language;
  QFont font;

  ThemedColor validSyntax = {.dark = Qt::darkGreen, .light = Qt::darkGreen};
  ThemedColor invalidSyntax = {.dark = Qt::darkRed, .light = Qt::darkRed};
  ThemedColor warningSyntax = {.dark = Qt::darkYellow, .light = Qt::darkYellow};

  Q_INVOKABLE void setFont(QFont);

signals:
  void validSyntaxChanged();
  void invalidSyntaxChanged();
  void warningSyntaxChanged();
  void fontChanged();

};
