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
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#pragma once

#include "zecalculator/parsing/types.h"
#include <cfloat>
#include <QtWidgets>

#include <QMetaType>
#include <QtQml/qqmlregistration.h>

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

#define MAX_NUM_PREC_DEC 1E-8

#define RELATIVE_MIN_SIZE 0.25 // How much the graph can be smaller than its container sheet/background

#define CM_PER_INCH 2.54

/// @brief type of math objects to use in ZeCalculator
inline constexpr zc::parsing::Type zc_t = zc::parsing::Type::RPN;

// extern definitions for the supported languages
inline const QList<QLocale::Language> supportedLangs = { QLocale::English, QLocale::French, QLocale::German, QLocale::Chinese};
inline QString langToShortString(QLocale::Language lang)
{
    if(lang == QLocale::French)
        return "fr";
    else if(lang == QLocale::German)
        return "de";
    else if(lang == QLocale::Chinese)
        return "zh";
    else return "en";
}

enum struct ZeAxisName {X, Y};

enum struct ZeViewType
{
    LINEAR, LOG
};

Q_DECLARE_METATYPE(ZeViewType);

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
    ZeViewType axisType = ZeViewType::LINEAR;
};

struct SizeUnit: QObject
{
  Q_OBJECT
  QML_ELEMENT

public:
  SizeUnit(QObject *parent = nullptr): QObject(parent) {}

  enum Unit {PIXEL, CENTIMETER};
  Q_ENUM(Unit)
};

struct ZeSizeSettings
{
  Q_GADGET

  Q_PROPERTY(SizeUnit::Unit sizeUnit MEMBER sizeUnit)
  Q_PROPERTY(bool sheetFillsWindow MEMBER sheetFillsWindow)
  Q_PROPERTY(bool figureFillsSheet MEMBER figureFillsSheet)
  Q_PROPERTY(double scalingFactor MEMBER scalingFactor)
  Q_PROPERTY(QSize pxFigureSize MEMBER pxFigureSize)
  Q_PROPERTY(QSize pxSheetSize MEMBER pxSheetSize)
  Q_PROPERTY(int pxMargins MEMBER pxMargins)
  Q_PROPERTY(QSizeF cmFigureSize MEMBER cmFigureSize)
  Q_PROPERTY(QSizeF cmSheetSize MEMBER cmSheetSize)
  Q_PROPERTY(double cmMargins MEMBER cmMargins)

public:

  SizeUnit::Unit sizeUnit = SizeUnit::PIXEL;
  bool sheetFillsWindow = true, figureFillsSheet = true;

  double scalingFactor = 1.0;

  QSize pxFigureSize, pxSheetSize;
  int pxMargins;

  QSizeF cmFigureSize, cmSheetSize;
  double cmMargins;

  bool operator == (const ZeSizeSettings &other) const = default;
};

struct ZoomingType: QObject
{
  Q_OBJECT
  QML_ELEMENT

public:
  ZoomingType(QObject *parent = nullptr): QObject(parent) {}

  enum Type {FITSHEET, CUSTOM};
  Q_ENUM(Type)
};

struct ZeZoomSettings
{
  Q_GADGET
  Q_PROPERTY(ZoomingType::Type zoomingType MEMBER zoomingType)
  Q_PROPERTY(double zoom MEMBER zoom)
  QML_VALUE_TYPE(ZeZoomSettings)

public:

  ZoomingType::Type zoomingType;
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

    QColor backgroundColor = Qt::white;
    QColor defaultColor = Qt::black;
    int curvesThickness = 3;
    double distanceBetweenPoints = 1;
    bool smoothing = true;
    QFont graphFont = {};
};

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
