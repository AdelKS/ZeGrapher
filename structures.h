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
#include <cfloat>
#include <QtWidgets>

#define SOFTWARE_VERSION 3.5
#define SOFTWARE_VERSION_STR "v3.5"

// TODO put the following 3 defs into an enum
#define FROM_CURRENT_GRAPHIC 0
#define MANUAL_ENTRY 1
#define PREDEFINED_ENTRY 2

#define MAX_DOUBLE_PREC 13

#define NORMAL 0
#define ZOOMER 1
#define DEZOOMER 2
#define ZOOMBOX 3
#define DEPLACER 4
#define NO_CURSOR 5

#define MIN_AMPLITUDE 100 * DBL_EPSILON
#define MAX_AMPLITUDE DBL_MAX / 100

#define NUM_PREC 7 // default precision while displaying decimal numbers
#define MAX_NUM_PREC 8 // the maximum precision in decimal come from imprecision on integration and derivation

#define INIT_FREQ 30 //animation update frequency
#define INIT_INCR_PERIOD 100 //animation incremental period
#define PAR_DRAW_LIMIT 500

#define INVALID_COLOR "#FF9980"
#define VALID_COLOR "#B2FFB2"

#define EPSILON 0.001
#define MAX_SAVED_SEQ_VALS 1000000

#define RELATIVE_MIN_SIZE 0.25 // How much the graph can be smaller than its container sheet/background

#define CM_PER_INCH 2.54

// Maximum and minimum relative size between the abscissa text and the horizontal tick spacing
#define MAX_REL_TICK_SPACING 0.8
#define MIN_REL_TICK_SPACING 0.2

// extern definitions for the supported languages
extern const QList<QLocale::Language> supportedLangs;
extern QString langToShortString(QLocale::Language lang);

struct ZeAxisRange
{
    double min, max;

    double amplitude() const
    {
        return fabs(max - min);
    }
};

struct GraphRange
{
    ZeAxisRange x, y;

    QRectF getRect() const
    {
        QRectF graphWin;
        graphWin.setBottom(y.min);
        graphWin.setTop(y.max);
        graphWin.setLeft(x.min);
        graphWin.setRight(x.max);
        return graphWin;
    }

    bool operator==(const GraphRange &other)
    {
        return fabs(x.min - other.x.min) < MIN_AMPLITUDE && fabs(x.max - other.x.max) < MIN_AMPLITUDE &&
                fabs(y.min - other.y.min) < MIN_AMPLITUDE && fabs(y.max - other.y.max) < MIN_AMPLITUDE;
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
    bool decimalGlobalConstant, decimalBase;

    bool operator !=(const ZeCoordinateDisplayFormat &other)
    {
        return decimalGlobalConstant != other.decimalGlobalConstant or
                decimalBase != other.decimalBase;
    }
};

struct ZeLogAxisSettings
{
    double constantMultiplier, base;
    QString constantMultiplierStr, baseStr;

    bool operator !=(const ZeLogAxisSettings &other)
    {
        return constantMultiplier != other.constantMultiplier or
                base != other.base or
                constantMultiplierStr != other.constantMultiplierStr or
                baseStr != other.baseStr;
    }
};

struct ZeLinAxisSettings
{
    double constantMultiplier;
    QString constantMultiplierStr;
    int maxDigitsNum;

    bool operator !=(const ZeLinAxisSettings &other)
    {
        return constantMultiplier != other.constantMultiplier or
                constantMultiplierStr != other.constantMultiplierStr or
                maxDigitsNum != other.maxDigitsNum;
    }
};

struct ZeAxisSettings
{
    QColor color;
    double lineWidth;
    int tickRelSpacing;
    ZeLinAxisSettings linSettings;
    ZeLogAxisSettings logSettings;
    ZeAxisType axisType;

    bool operator !=(const ZeAxisSettings &other)
    {
        return color != other.color or
                lineWidth != other.lineWidth or
                tickRelSpacing != other.tickRelSpacing or
                linSettings != other.linSettings or
                logSettings != other.logSettings or
                axisType != other.axisType;
    }
};

struct ZeSizeSettings
{
    enum SizeUnit {PIXEL, CENTIMETER};

    SizeUnit sizeUnit;
    bool sheetFillsWindow, figureFillsSheet;

    double scalingFactor;

    QSize pxFigureSize, pxSheetSize;
    int pxMargins;

    QSizeF cmFigureSize, cmSheetSize;
    double cmMargins;

    bool operator !=(const ZeSizeSettings &other)
    {
        return sizeUnit != other.sizeUnit or
                sheetFillsWindow != other.sheetFillsWindow or
                scalingFactor != other.scalingFactor or
                pxFigureSize != other.pxFigureSize or
                pxSheetSize != other.pxSheetSize or
                pxMargins != other.pxMargins or
                cmFigureSize != other.cmFigureSize or
                cmSheetSize != other.cmSheetSize or
                cmMargins != other.cmMargins;
    }
};

struct ZeZoomSettings
{
    enum ZoomingType {FITSHEET, CUSTOM};

    ZoomingType zoomingType;
    double zoom;

    bool operator !=(const ZeZoomSettings &other)
    {
        return zoomingType != other.zoomingType or
                zoom != other.zoom;
    }
};

struct ZeEstheticSettings
{
    QColor backgroundColor;
    QColor defaultColor;
    int curvesThickness;
    double distanceBetweenPoints;
    bool smoothing;
    QFont graphFont;

    bool operator !=(const ZeEstheticSettings &other)
    {
        return  backgroundColor != other.backgroundColor or
                defaultColor != other.defaultColor or
                curvesThickness != other.curvesThickness or
                distanceBetweenPoints != other.distanceBetweenPoints or
                smoothing != other.smoothing or
                graphFont != other.graphFont;
    }
};

struct ZeGraphSettings
{
    ZeZoomSettings zoomSettings;
    ZeSizeSettings sizeSettings;
    ZeEstheticSettings estheticSettings;

    bool operator !=(const ZeGraphSettings &other)
    {
        return  zoomSettings != other.zoomSettings or
                sizeSettings != other.sizeSettings or
                estheticSettings != other.estheticSettings;
    }
};

struct Ze1DGridSettings
{
    bool showGrid, showSubGrid;
    double gridLineWidth, subgridLineWidth;
    QColor gridColor, subgridColor;
    unsigned int subgridSubDivs;
    bool showSubgridRelativeCoordinates;

    bool operator !=(const Ze1DGridSettings &other)
    {
        return showGrid != other.showGrid or
                showSubGrid != other.showSubGrid or
                gridLineWidth != other.gridLineWidth or
                subgridLineWidth != other.subgridLineWidth or
                gridColor != other.gridColor or
                subgridColor != other.subgridColor or
                subgridSubDivs != other.subgridSubDivs or
                showSubgridRelativeCoordinates != other.showSubgridRelativeCoordinates;
    }
};

struct ZeGridSettings
{
    Ze1DGridSettings x, y;

    bool operator !=(const ZeGridSettings &other)
    {
        return x != other.x or y != other.y;
    }
};

struct ZeAxesSettings
{
    ZeAxisSettings x, y;    
    bool orthonormal;

    bool operator !=(const ZeAxesSettings &other)
    {
        return orthonormal != other.orthonormal or x != other.x or y != other.y;
    }
};

struct ZeViewSettings
{
    GraphRange range;
    ZeAxesSettings axes;
    ZeGridSettings grid;
    ZeGraphSettings graph;
};

struct ZeAppSettings
{
    bool startupUpdateCheck;
    QLocale::Language language;
    QFont font;
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

struct Point
{
    double x, y;

    bool operator<(const Point &b) const
    {
        return x < b.x;
    }
};

struct TangentPoints
{
    Point left, center, right;
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



#endif // STRUCTURES_H
