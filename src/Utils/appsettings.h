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

struct ZeAppSettings: QObject
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(bool firstName MEMBER startupUpdateCheck)
  Q_PROPERTY(QLocale::Language language MEMBER language)
  Q_PROPERTY(QFont font MEMBER font)
  Q_PROPERTY(QColor validSyntax READ getValidSyntax WRITE setValidSyntax NOTIFY validSyntaxChanged)
  Q_PROPERTY(QColor invalidSyntax READ getInvalidSyntax WRITE setInvalidSyntax NOTIFY invalidSyntaxChanged)
  Q_PROPERTY(QColor warningSyntax READ getWarningSyntax WRITE setWarningSyntax NOTIFY warningSyntaxChanged)

public:

  explicit ZeAppSettings(QObject* parent = nullptr): QObject(parent) {}

  bool startupUpdateCheck;
  QLocale::Language language;
  QFont font;

  QColor validSyntaxDark = Qt::darkGreen;
  QColor invalidSyntaxDark = Qt::darkRed;
  QColor warningSyntaxDark = Qt::darkYellow;

  QColor validSyntaxLight = Qt::darkGreen;
  QColor invalidSyntaxLight = Qt::darkRed;
  QColor warningSyntaxLight = Qt::darkYellow;

  Q_INVOKABLE QColor getValidSyntax() const;
  Q_INVOKABLE QColor getInvalidSyntax() const;
  Q_INVOKABLE QColor getWarningSyntax() const;

  Q_INVOKABLE void setValidSyntax(QColor);
  Q_INVOKABLE void setInvalidSyntax(QColor);
  Q_INVOKABLE void setWarningSyntax(QColor);


signals:
  void validSyntaxChanged();
  void invalidSyntaxChanged();
  void warningSyntaxChanged();

public slots:
  void colorSchemeChanged();

};
