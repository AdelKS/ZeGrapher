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
#include <QFont>
#include <QColor>

struct ZeGraphSettings: QObject
{
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QFont font WRITE setFont MEMBER font NOTIFY fontChanged)
  QML_VALUE_TYPE(ZeGraphSettings)

public:
    explicit ZeGraphSettings(QObject* parent = nullptr): QObject(parent) {}

    QColor backgroundColor = Qt::white;
    double distanceBetweenPoints = 1;
    bool smoothing = true;

    QFont getFont() const { return font; };
    Q_INVOKABLE void setFont(QFont);

signals:
  void fontChanged();

protected:
    QFont font = {};
};
