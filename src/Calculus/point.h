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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#pragma once

#include <QPointF>

struct Range
{
    Range() : start(0), end(0), step(1) {}
    double start, end, step;
};

struct Point
{
    double x, y;

    bool operator<(const Point &b) const
    {
        return x < b.x;
    }

    operator QPointF() const
    {
        return QPointF(x, y);
    }
};

inline bool ptCompY(const Point &pt1, const Point &pt2)
{
  return pt1.y < pt2.y;
}

inline Point operator * (const Point &pt1, const Point &pt2)
{
  return Point {.x = pt1.x * pt2.x, .y = pt1.y * pt2.y };
}
