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

#include <QMetaType>
#include <QtQml/qqmlregistration.h>

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

#define NUM_PREC_DEC 1E-7
#define MAX_NUM_PREC_DEC 1E-8

#define INIT_FREQ 30 //animation update frequency
#define INIT_INCR_PERIOD 100 //animation incremental period
#define PAR_DRAW_LIMIT 500

#define INVALID_COLOR "#FF9980"
#define VALID_COLOR "#B2FFB2"

#define EPSILON 0.001
#define MAX_SAVED_SEQ_VALS 1000000

#define RELATIVE_MIN_SIZE 0.25 // How much the graph can be smaller than its container sheet/background

#define CM_PER_INCH 2.54

#define ROMBERG_MAX_STEPS 500

// Maximum and minimum relative size between the abscissa text and the horizontal tick spacing
#define MAX_REL_TICK_SPACING 0.8
#define MIN_REL_TICK_SPACING 0.2

// extern definitions for the supported languages
extern const QList<QLocale::Language> supportedLangs;
extern QString langToShortString(QLocale::Language lang);

struct ZeAxisRange
{
  Q_GADGET
  Q_PROPERTY(double min MEMBER min)
  Q_PROPERTY(double max MEMBER max)
  QML_VALUE_TYPE(zeaxisrange)

public:

  double min, max;

  bool operator == (const ZeAxisRange&) const = default;

  double amplitude() const
  {
    return fabs(max - min);
  }

  void operator *=(double scale)
  {
    max *= scale;
    min *= scale;
  }

  void operator /=(double scale)
  {
    max /= scale;
    min /= scale;
  }

  void operator -=(double offset)
  {
    max -= offset;
    min -= offset;
  }

  void operator +=(double offset)
  {
    max += offset;
    min += offset;
  }
};

struct GraphRange
{
  Q_GADGET
  Q_PROPERTY(ZeAxisRange x MEMBER x)
  Q_PROPERTY(ZeAxisRange y MEMBER y)
  QML_VALUE_TYPE(graphrange)

public:

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

  bool operator == (const GraphRange &other) const = default;
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

Q_DECLARE_METATYPE(ZeAxisType);

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

class ZeAxisSettings
{
public:
    ZeAxisSettings() = default;

    bool operator == (const ZeAxisSettings &other) const = default;

    int tickRelSpacing = 0;
    ZeLinAxisSettings linSettings;
    ZeLogAxisSettings logSettings;
    ZeAxisType axisType = ZeAxisType::LINEAR;
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

    bool operator == (const ZeSizeSettings &other) const = default;
};

struct ZeZoomSettings
{
    enum ZoomingType {FITSHEET, CUSTOM};

    ZoomingType zoomingType;
    double zoom;

    bool operator == (const ZeZoomSettings &other) const = default;
};

struct Ze1DGridSettings
{
    bool showGrid = true, showSubGrid = false;
    double gridLineWidth = 0.6, subgridLineWidth = 0.4;
    QColor gridColor = Qt::gray, subgridColor = Qt::gray;
    unsigned int subgridSubDivs = 2;
    bool showSubgridRelativeCoordinates = false;

    bool operator ==(const Ze1DGridSettings &other) const = default;
};

struct ZeGridSettings
{
    Ze1DGridSettings x, y;

    bool operator == (const ZeGridSettings &other) const = default;
};

struct ZeAxesSettings
{
    ZeAxisSettings x, y;
    bool orthonormal = false;
    QColor color = Qt::black;
    double lineWidth = 2;

    bool operator == (const ZeAxesSettings &other) const = default;
};

struct ZeGraphSettings
{
    bool operator == (const ZeGraphSettings &other) const = default;

    QColor backgroundColor;
    QColor defaultColor;
    int curvesThickness;
    double distanceBetweenPoints;
    bool smoothing;
    QFont graphFont;
};

struct ZeAppSettings
{
  Q_GADGET
  Q_PROPERTY(bool firstName MEMBER startupUpdateCheck)
  Q_PROPERTY(QLocale::Language language MEMBER language)
  Q_PROPERTY(QFont font MEMBER font)
  Q_PROPERTY(QColor validSyntax MEMBER validSyntax)
  Q_PROPERTY(QColor invalidSyntax MEMBER invalidSyntax)
  QML_VALUE_TYPE(zeappsettings)

public:

  bool startupUpdateCheck;
  QLocale::Language language;
  QFont font;
  QColor validSyntax, invalidSyntax;
};

struct FuncMap
{
    bool funcType;
    short id;
};

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

bool ptCompY(const Point &pt1, const Point &pt2);

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

// N.B: enum values are used as indexes in a vector!
enum struct PointStyle : unsigned int { Rhombus = 0, Disc, Square, Triangle, Cross };

struct DataStyle
{
    DataStyle(): draw(true), drawLines(false), drawPoints(true),
        color(Qt::black), pointStyle(PointStyle::Square), lineStyle(Qt::SolidLine) {}

    bool draw, drawLines, drawPoints;
    QColor color;
    PointStyle pointStyle;
    Qt::PenStyle lineStyle;
};

struct UserData
{
    UserData() : cartesian(true), style() {}

    std::vector<Point> dataPoints;
    bool cartesian;
    DataStyle style;
};

struct SelectorPos
{
    bool inbetween;
    int index;
};

QPointF toQPointF(const Point &pt);

Point operator*(Point &pt1, const Point &pt2);

template<class Number>
Number int_pow(Number a, int b)
{
    Number res = 1;
    if(b < 0)
    {
        for(int i = 0 ; i > b ; i--)
            res /= a;
    }
    else
    {
        for(int i = 0 ; i < b ; i++)
            res *= a;
    }

    return res;
}

Point operator * (const Point &pt1, const Point &pt2);

#endif // STRUCTURES_H
