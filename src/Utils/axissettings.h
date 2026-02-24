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

#include <QString>
#include <QColor>

struct ZeLogAxisSettings
{
    ZeLogAxisSettings() = default;

    double constantMultiplier = 1, base = 10;
    QString constantMultiplierStr, baseStr = "10";

    bool operator == (const ZeLogAxisSettings &other) const = default;
};

struct ZeLinAxisSettings
{
    ZeLinAxisSettings() = default;

    bool operator == (const ZeLinAxisSettings &other) const = default;

    double constantMultiplier = 1;
    QString constantMultiplierStr;
    int maxDigitsNum = 3;
};

enum struct ZeViewType
{
    LINEAR, LOG
};

class ZeAxisSettings
{
public:
    ZeAxisSettings() = default;

    bool operator == (const ZeAxisSettings &other) const = default;

    int tickRelSpacing = 0;
    ZeLinAxisSettings linSettings;
    ZeLogAxisSettings logSettings;
    ZeViewType axisType = ZeViewType::LINEAR;
};

struct ZeAxesSettings
{
    ZeAxisSettings x, y;
    bool orthonormal = false;
    QColor color = Qt::black;
    double lineWidth = 2;

    bool operator == (const ZeAxesSettings &other) const = default;
};
