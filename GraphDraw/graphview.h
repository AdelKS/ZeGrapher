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

#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QObject>
#include <QWidget>
#include <QRectF>
#include <QPair>

#include "structures.h"

struct ZeAxisTick
{
    double pos;
    QString baseStr, globalConstantStr;
    double base, globalConstant;
    long multiplier, subMultiplier, powerNumerator, powerDenominator;
};

struct ZeAxisSubTick
{
    double pos;
    int numerator, denominator;
};

struct ZeOffset
{
    long sumOffset, powerOffset;
};

struct ZeAxisTicks
{
    ZeOffset offset;
    QList<ZeAxisTick> ticks;
    QList<ZeAxisSubTick> axisSubticks;
};

struct ZeAxesTicks
{
    ZeAxisTicks x, y;
};


class ZeGraphView : public QObject
{
    Q_OBJECT

public:
    explicit ZeGraphView(const ZeViewSettings &viewSettings, QSize viewPxSize, QObject *parent = nullptr);
    ZeGraphView(const ZeGraphView &other, QObject *parent = nullptr);

    ZeGraphView& operator=(const ZeGraphView &other);

    void zoomYview(double ratio);
    void zoomXview(double ratio);
    void zoomView(QPointF center, double ratio);
    void translateView(QPointF vec);

    void setViewSettings(const ZeViewSettings &viewSettings);
    
    void setViewXmin(double val);
    void setViewXmax(double val);
    void setViewYmin(double val);
    void setViewYmax(double val);

    void setlgXmin(double val);
    void setlgXmax(double val);
    void setlgYmin(double val);
    void setlgYmax(double val);

    void setXmin(double val);
    void setXmax(double val);
    void setYmin(double val);
    void setYmax(double val);

    void setGraphRange(const GraphRange &range);

    double getXmin();
    double getXmax();
    double getYmin();
    double getYmax();

    double viewToUnitY(double viewY) const;
    double unitToViewY(double unitY) const ;

    double viewToUnitX(double viewX) const ;
    double unitToViewX(double unitX) const ;

    ZeAxesTicks getAxesTicks();

    QRectF rect() const ;
    QRectF lgRect() const ;
    QRectF viewRect() const ;
    void setViewRect(QRectF rect);

signals:

public slots:

protected:
    ZeAxisTicks getLinearAxisTicks(int pxWidth, ZeAxisRange range, ZeAxisSettings &axisSettings, QFontMetrics metrics);

    void verifyOrthonormality();

    double Xmin, Xmax, Ymin, Ymax;
    double lgXmin, lgXmax, lgYmin, lgYmax;

    double xLogBase, yLogBase;    
    double xGridStep, yGridStep;

    QSizeF viewPxSize;

    ZeAxesSettings axesSettings;
    ZeGridSettings gridSettings;
};

#endif // GRAPHVIEW_H
