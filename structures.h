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

#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <cmath>
#include <QtWidgets>

#define SOFTWARE_VERSION 3.5
#define SOFTWARE_VERSION_STR "v3.5"

// TODO put the following 3 defs into an enum
#define FROM_CURRENT_GRAPHIC 0
#define MANUAL_ENTRY 1
#define PREDEFINED_ENTRY 2

#define MAX_DOUBLE_PREC 13

#define MIN_RANGE 1E-13
#define NUM_PREC 7 // default precision while displaying decimal numbers
#define MAX_NUM_PREC 8 // the maximum precision in decimal come from imprecision on integration and derivation

#define INIT_FREQ 30 //animation update frequency
#define INIT_INCR_PERIOD 100 //animation incremental period
#define PAR_DRAW_LIMIT 500

#define INVALID_COLOR "#FF9980"
#define VALID_COLOR "#B2FFB2"

#define EPSILON 0.001
#define MAX_SAVED_SEQ_VALS 1000000


struct GraphRange
{
    double Xmin, Xmax, Ymin, Ymax;    
    bool orthonormal;

    QRectF getRect() const
    {
        QRectF graphWin;
        graphWin.setBottom(Ymin);
        graphWin.setTop(Ymax);
        graphWin.setLeft(Xmin);
        graphWin.setRight(Xmax);
        return graphWin;
    }

    bool operator==(const GraphRange &other)
    {
        return fabs(Xmin - other.Xmin) < MIN_RANGE && fabs(Xmax - other.Xmax) < MIN_RANGE &&
                fabs(Ymin - other.Ymin) < MIN_RANGE && fabs(Ymax - other.Ymax) < MIN_RANGE;
    }
    bool operator!=(const GraphRange &other)
    {
        return !((*this) == other);
    }
};

enum struct ZeAxisName {X, Y};

struct FastTree
{
    short type;
    double *value;
    FastTree *left;
    FastTree *right;
};

enum struct ZeAxisType
{
    LINEAR, LOG
};

struct ZeCoordinateDisplayFormat
{
    bool decimalGlobalConstant, linDecimalBaseMultiplier, decimalBase;
};

struct ZeAxisSettings
{
    int linearDivider, logDivisions;
    double globalConstant, base;
    QString globalConstantStr, baseStr;
    QColor color;
    int basePowNum, basePowDenom;
    ZeCoordinateDisplayFormat coordinateFormatting;
    ZeAxisType axisType;
};

struct ZeGraphSettings
{
    QColor backgroundColor;
    QColor defaultColor;
    int curvesThickness;
    double distanceBetweenPoints;
    bool smoothing;
    QFont graphFont;
};

struct ZeUnidimGridSettings
{
    bool showGrid, showSubGrid;
    double gridLineWidth, subgridLineWidth;
    QColor gridColor, subgridColor;
    unsigned int subgridSubDivs;
    bool showSubgridRelativeCoordinates;
};

struct ZeGridSettings
{
    ZeUnidimGridSettings alongX, alongY;
};

struct ZeAxesSettings
{
    ZeAxisSettings x, y;
};

struct ZeViewSettings
{
    GraphRange range;
    ZeAxesSettings axes;
    ZeGridSettings grid;
    ZeGraphSettings graph;
};

struct FuncMap
{
    bool funcType;
    short id;
};

struct Range
{
    double start, end, step;
};

struct TangentPoints
{
    QPointF left, center, right;
    //center is the point where the tangent touches the curve, left and right are the edges of the segment
};

struct ParametricInfo
{
    bool isParametric;
    Range range;
    int originalDrawsNum, currentDrawsNum;
};

struct Color
{
    short r, g, b;
};

struct ValuesTableParameters
{
    short funcType, id, entryType, emptyCellsCount;
    Range range;
    QString name;
};

struct ParEqValues
{
    QList<double> tValues, xValues, yValues;
};


enum struct PointStyle { Rhombus, Disc, Square, Triangle, Cross };

struct DataStyle
{
    bool draw, drawLines, drawPoints;
    QColor color;
    PointStyle pointStyle;
    Qt::PenStyle lineStyle;
};

struct SelectorPos
{
    bool inbetween;
    int index;
};

struct Point
{
    double x, y;

    bool operator<(const Point &b) const
    {
        return x < b.x;
    }
};

#endif // STRUCTURES_H
