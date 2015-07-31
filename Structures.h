/****************************************************************************
**  Copyright (c) 2013, Adel Kara Slimane, the ZeGrapher project <contact@zegrapher.com>
**
**  This file is part of the ZeGrapher project, version 2.0.
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


#define FROM_CURRENT_GRAPHIC 0
#define MANUAL_ENTRY 1
#define PREDEFINED_ENTRY 2


#define NORMAL 0
#define ZOOMER 1
#define DEZOOMER 2
#define ZOOMBOX 3
#define DEPLACER 4
#define NO_CURSOR 5

#define FUNCTION 1
#define SEQUENCE 2
#define PARAMETRIC_EQ 3
#define NORMAL_EXPR 4
#define DATA_TABLE_EXPR 5 // expression to apply to a column: example: x' = 2 * x multiplies every column's cell value by 2.
#define MAX_DOUBLE_PREC 15

#define MIN_RANGE 0.000001
#define NUM_PREC 7 // default precision while displaying decimal numbers
#define MAX_NUM_PREC 8 // the maximum precision in decimal come from imprecision on integration and derivation

#define INIT_FREQ 20 //animation update ferquence
#define INIT_INCR_PERIOD 100 //animation incremental period
#define PAR_DRAW_LIMIT 100

#define INVALID_COLOR "#FF9980"
#define VALID_COLOR "#B2FFB2"

#define EPSILON 0.001


struct GraphRange
{
    double Xmin, Xmax, Ymin, Ymax, Xscale, Yscale;
};
struct Point
{
    double x, y;

    bool operator<(const Point &b) const
    {
        return x < b.x;
    }
};

struct Rectangle
{
    Point pt1, pt2;
};

struct FastTree
{
    short type;
    double *value;
    FastTree *left;
    FastTree *right;
};

struct Options
{
    QColor couleurDesAxes;
    QColor couleurDuFond;
    QColor couleurQuadrillage;
    short epaisseurDesCourbes;
    short numSize;
    double distanceEntrePoints;
    bool lissage;

};

struct CurveSelection
{
    bool tangentSelection;
    bool isSomethingSelected;
    bool isParametric;
    int funcType, tangentPtSelection, id, kPos;

};

struct FuncMap
{
    bool funcType;
    short id;
};

struct MouseState
{
    bool tangentHovering;
    bool hovering;
    bool isParametric;
    short tangentPtSelection;
    short funcType;
    short kPos;
    short id;
};

struct Range
{
    double start, end, step;
};

struct TangentPoints
{
    Point left, center, right; //the "center" point is the tangent point between the curve and the tangent
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


enum PointStyle { Rhombus, Disc, Square, Triangle, Cross };

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




#endif // STRUCTURES_H
