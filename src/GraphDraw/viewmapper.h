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

    GraphRange getGraphRange();

    double getXmin();
    double getXmax();
    double getYmin();
    double getYmax();

    QPointF toView(const QPointF &unitPt) const;
    QPointF toUnit(const QPointF &viewPt) const;

    Point toView(const Point &unitPt) const;
    Point toUnit(const Point &viewPt) const;

    double toUnitY(double viewY) const;
    double toViewY(double unitY) const ;

    double toUnitX(double viewX) const ;
    double toViewX(double unitX) const ;



    QRectF getRect() const ;
    QRectF getLogRect() const ;
    QRectF getViewRect() const ;
    void setViewRect(QRectF getRect);

signals:

public slots:
    void setGraphRange(const GraphRange &range);

protected:

    void verifyOrthonormality();

    double Xmin, Xmax, Ymin, Ymax;
    double lgXmin, lgXmax, lgYmin, lgYmax;

    QSizeF viewPxSize;

    ZeAxesSettings axesSettings;
    ZeGridSettings gridSettings;
};

#endif // GRAPHVIEW_H
