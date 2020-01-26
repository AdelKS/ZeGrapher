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

#define TARGET_TICKS_NUM 20

#include "structures.h"


struct ZeLogAxisTick
{
    double pos;
    QString baseStr, globalConstantStr;
    double base, globalConstant;
    long multiplier, subMultiplier, powerNumerator, powerDenominator;
};

struct ZeLinAxisTick
{
    double pos, multiplier;
};

struct ZeAxisSubTick
{
    double pos;
    int numerator, denominator;
};

struct ZeOffset
{
    double sumOffset, powerOffset;
};

struct ZeLogAxisTicks
{
    QList<ZeLogAxisTick> ticks;
    QList<ZeAxisSubTick> axisSubticks;
};

struct ZeLinAxisTicks
{
    ZeOffset offset;
    QList<ZeLinAxisTick> ticks;
    QList<ZeAxisSubTick> axisSubticks;
};

class ZeViewMapper : public QObject
{
    Q_OBJECT

public:
    explicit ZeViewMapper(QObject *parent = nullptr);

    ZeViewMapper(const ZeViewMapper &other,
                QObject *parent = nullptr);

    ZeViewMapper& operator=(const ZeViewMapper &other);

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
    GraphRange getGraphRange();

    double getXmin();
    double getXmax();
    double getYmin();
    double getYmax();

    double viewToUnitY(double viewY) const;
    double unitToViewY(double unitY) const ;

    double viewToUnitX(double viewX) const ;
    double unitToViewX(double unitX) const ;

    ZeLinAxisTicks getLinearAxisTicks(double pxWidth,
                                   ZeAxisRange range,
                                   ZeAxisName axisName,
                                   QFontMetrics metrics);

    QRectF getRect() const ;
    QRectF getLogRect() const ;
    QRectF getViewRect() const ;
    void setViewRect(QRectF getRect);

signals:

public slots:

protected:

    void verifyOrthonormality();

    double Xmin, Xmax, Ymin, Ymax;
    double lgXmin, lgXmax, lgYmin, lgYmax;

    QSizeF viewPxSize;
    double targetTicksNum;

    ZeAxesSettings axesSettings;
    ZeGridSettings gridSettings;
};

#endif // GRAPHVIEW_H
