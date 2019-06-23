#include "graphview.h"

ZeGraphView::ZeGraphView(const ZeViewSettings &viewSettings, QSize widgetSize, QObject *parent) : QObject(parent)
{
    this->widgetSize = widgetSize;

    setViewSettings(viewSettings);
}

void ZeGraphView::setViewSettings(const ZeViewSettings &viewSettings)
{
    axesSettings = viewSettings.axes;
    xLogBase = pow(axesSettings.x.base, double(axesSettings.x.basePowNum)/double(axesSettings.x.basePowDenom));
    yLogBase = pow(axesSettings.y.base, double(axesSettings.y.basePowNum)/double(axesSettings.y.basePowDenom));

    gridSettings = viewSettings.grid;

    setGraphRange(viewSettings.range);
}

void ZeGraphView::setGraphRange(const GraphRange &range)
{
    setXmax(range.Xmax);
    setYmax(range.Ymax);
    setXmin(range.Xmin);
    setYmin(range.Ymin);
}

ZeGraphView::ZeGraphView(const ZeGraphView &other, QObject *parent) : QObject(parent)
{
    Xmin = other.Xmin;
    Xmax = other.Xmax;
    Ymin = other.Ymin;
    Ymax = other.Ymax;

    lgXmin = other.lgXmin;
    lgXmax = other.lgXmax;
    lgYmin = other.lgYmin;
    lgYmax = other.lgYmax;

    xLogBase = other.xLogBase;
    yLogBase = other.yLogBase;
}

ZeGraphView& ZeGraphView::operator=(const ZeGraphView &other)
{
    Xmin = other.Xmin;
    Xmax = other.Xmax;
    Ymin = other.Ymin;
    Ymax = other.Ymax;

    lgXmin = other.lgXmin;
    lgXmax = other.lgXmax;
    lgYmin = other.lgYmin;
    lgYmax = other.lgYmax;

    xLogBase = other.xLogBase;
    yLogBase = other.yLogBase;

    return *this;
}

QRectF ZeGraphView::rect() const
{
    QRectF graphWin;
    graphWin.setBottom(Ymin);
    graphWin.setTop(Ymax);
    graphWin.setLeft(Xmin);
    graphWin.setRight(Xmax);
    return graphWin;
}

QRectF ZeGraphView::lgRect() const
{
    QRectF graphlLgWin;
    graphlLgWin.setBottom(lgYmin);
    graphlLgWin.setTop(lgYmax);
    graphlLgWin.setLeft(lgXmin);
    graphlLgWin.setRight(lgXmax);
    return graphlLgWin;
}

void ZeGraphView::setViewRect(QRectF rect)
{
    setViewXmax(rect.right());
    setViewXmin(rect.left());
    setViewYmax(rect.top());
    setViewYmin(rect.bottom());

    verifyOrthonormality();
}

QRectF ZeGraphView::viewRect() const
{
    QRectF viewWin;

    if(axesSettings.x.axisType == ZeAxisType::LOG)
    {
        viewWin.setLeft(lgXmin);
        viewWin.setRight(lgXmax);
    }
    else
    {
        viewWin.setLeft(Xmin);
        viewWin.setRight(Xmax);
    }

    if(axesSettings.y.axisType == ZeAxisType::LOG)
    {
        viewWin.setBottom(lgYmin);
        viewWin.setTop(lgYmax);
    }
    else
    {
        viewWin.setBottom(Ymin);
        viewWin.setTop(Ymax);
    }

    return viewWin;
}

void ZeGraphView::zoomYview(double ratio)
{
    if(axesSettings.y.axisType == ZeAxisType::LOG)
    {
        double val = (lgYmax - lgYmin) * ratio;
        setlgYmax(lgYmax + val);
        setlgYmin(lgYmin - val);
    }
    else
    {
        double val = (Ymax - Ymin) * ratio;
        setYmax(Ymax + val);
        setYmin(Ymin - val);
    }

    verifyOrthonormality();
}

void ZeGraphView::zoomXview(double ratio)
{
    if(axesSettings.x.axisType == ZeAxisType::LOG)
    {
        double val = (lgXmax - lgXmin) * ratio;
        setlgXmax(lgXmax + val);
        setlgXmin(lgXmin - val);
    }
    else
    {
        double val = (Xmax - Xmin) * ratio;
        setXmax(Xmax + val);
        setXmin(Xmin - val);
    }

    verifyOrthonormality();
}

void ZeGraphView::zoomView(QPointF center, double ratio)
{
    if((Xmax - Xmin > MIN_RANGE && Ymax - Ymin > MIN_RANGE) || ratio < 0)
    {
        if(axesSettings.x.axisType == ZeAxisType::LOG)
        {
            setlgXmax(lgXmax - (lgXmax - center.x())*ratio);
            setlgXmin(lgXmin - (lgXmin - center.x())*ratio);
        }
        else
        {
            setXmax(Xmax - (Xmax - center.x())*ratio);
            setXmin(Xmin - (Xmin - center.x())*ratio);
        }

        if(axesSettings.x.axisType == ZeAxisType::LOG)
        {
            setlgYmax(lgYmax - (lgYmax - center.y())*ratio);
            setlgYmin(lgYmin - (lgYmin - center.y())*ratio);
        }
        else
        {
            setYmax(Ymax - (Ymax - center.y())*ratio);
            setYmin(Ymin - (Ymin - center.y())*ratio);
        }
    }
}

 ZeGrid ZeGraphView::getGrid()
 {
     ZeGrid grid;

     if(viewWidget == 0)
         return grid;

     if(scaleType == ZeScaleType::LINEAR || scaleType == ZeScaleType::LINEAR_ORTHONORMAL)
     {
         grid.horizontal = getOneDirectionLinearGrid(viewWidget->width(), xGridStep, Xmin, Xmax, gridSettings.horSubGridLineCount,
                                                     30, 150);
         grid.vertical = getOneDirectionLinearGrid(viewWidget->height(), yGridStep, Ymin, Ymax, gridSettings.verSubGridLineCount,
                                                   30, 100);
     }

     return grid;
 }


 ZeAxisTicks ZeGraphView::getOneDirectionLinearGrid(int pxWidth, double &step, double min, double max,
                                                       int subGridlineCount, double minTickDist, double maxTickDist)
 {
     ZeAxisTicks grid;

     ZeMainGridLine gridLine;
     ZeAxisSubTick subGridLine;

     double scaling = pxWidth / (max-min);

     if(scaling * step < minTickDist)
         step *= pow(2, trunc(ln(minTickDist)/ln(step*scaling)) + 1);
     else if(scaling * step > maxTickDist)
         step *= pow(2, trunc(ln(maxTickDist)/ln(step*scaling)));


     gridLine.pos = (trunc(min / step)-1) * step;

     grid.ticks.push_back(pos);

     while(gridLine.pos < max)
     {
         gridLine.pos += step;
         grid.ticks.push_back(pos);
     }

     double count = subGridlineCount;
     double p1, p2;

     for(int i = 0 ; i < grid.ticks.size() - 1 ; i++)
     {
         p1 = grid.ticks[i].pos;
         p2 = grid.ticks[i+1].pos;

         for(double j = 1 ; j < count ; j++)
         {
             subGridLine.pos = j*p2 + (count-j)*p1;
             grid.axisSubticks.push_back(subGridLine);
         }
     }

     while(!grid.ticks.empty() && grid.ticks.front().pos < min) { grid.ticks.pop_front(); }
     while(!grid.axisSubticks.empty() && grid.axisSubticks.front().pos < min) { grid.axisSubticks.pop_front(); }

     while(!grid.ticks.empty() && grid.ticks.back().pos > max) { grid.ticks.pop_back(); }
     while(!grid.axisSubticks.empty() && grid.axisSubticks.back().pos > max) { grid.axisSubticks.pop_back(); }

     return grid;

 }

void ZeGraphView::translateView(QPointF vec)
{
    if(axesSettings.x.axisType == ZeAxisType::LOG)
    {
        setlgXmax(lgXmax + vec.x());
        setlgXmin(lgXmin + vec.x());
    }
    else
    {
        setXmax(Xmax + vec.x());
        setXmin(Xmin + vec.x());
    }

    if(axesSettings.y.axisType == ZeAxisType::LOG)
    {
        setlgYmax(lgYmax + vec.y());
        setlgYmin(lgYmin + vec.y());
    }
    else
    {
        setYmax(Ymax + vec.y());
        setYmin(Ymin + vec.y());
    }
}

double ZeGraphView::viewToUnitY(double viewY) const // viewY =
{
    if(axesSettings.x.axisType == ZeAxisType::LOG)
    {
        return pow(yLogBase, viewY);
    }
    else
    {
        return viewY;
    }
}

double ZeGraphView::unitToViewY(double unitY) const
{
    if(axesSettings.y.axisType == ZeAxisType::LOG)
    {
        return log(unitY)/log(yLogBase);
    }
    else
    {
        return unitY;
    }
}

double ZeGraphView::getXmin()
{
    return Xmin;
}

double ZeGraphView::getXmax()
{
    return Xmax;
}

double ZeGraphView::getYmin()
{
    return Ymin;
}

double ZeGraphView::getYmax()
{
    return Ymax;
}

double ZeGraphView::viewToUnitX(double viewX) const
{
    if(axesSettings.x.axisType == ZeAxisType::LOG)
    {
        return pow(xLogBase, viewX);
    }
    else
    {
        return viewX;
    }
}

double ZeGraphView::unitToViewX(double unitX) const
{
    if(axesSettings.x.axisType == ZeAxisType::LOG)
    {
        return log(unitX)/log(xLogBase);
    }
    else
    {
        return unitX;
    }
}

void ZeGraphView::setViewXmin(double val)
{
    if(axesSettings.x.axisType == ZeAxisType::LOG)
    {
        setlgXmin(val);
    }
    else
    {
        setXmin(val);
    }

    verifyOrthonormality();
}

void ZeGraphView::setViewXmax(double val)
{
    if(axesSettings.x.axisType == ZeAxisType::LOG)
    {
        setlgXmax(val);
    }
    else
    {
        setXmax(val);
    }

    verifyOrthonormality();
}

void ZeGraphView::setViewYmin(double val)
{
    if(axesSettings.y.axisType == ZeAxisType::LOG)
    {
        setlgYmin(val);
    }
    else
    {
        setYmin(val);
    }

    verifyOrthonormality();
}

void ZeGraphView::setViewYmax(double val)
{
    if(axesSettings.y.axisType == ZeAxisType::LOG)
    {
        setlgYmax(val);
    }
    else
    {
        setYmax(val);
    }

    verifyOrthonormality();
}

void ZeGraphView::verifyOrthonormality()
{
    // TODO: update with only the Size, and to use with both orthonormal and linear
    if(viewWidget != 0 && viewType == ZeScaleType::LINEAR_ORTHONORMAL)
    {
        double uniteY = viewWidget->height() / viewRect().height();
        double uniteX = viewWidget->width() / viewRect().width();

        double ratio =  uniteX / uniteY;
        zoomYview(ratio);
    }
}

void ZeGraphView::setlgXmin(double val)
{
    lgXmin = val;
    Xmin = pow(xLogBase, lgXmin);
}

void ZeGraphView::setlgXmax(double val)
{
    lgXmax = val;
    Xmax = pow(xLogBase, lgXmax);
}

void ZeGraphView::setlgYmin(double val)
{
    lgYmin = val;
    Ymin = pow(yLogBase, lgYmin);
}

void ZeGraphView::setlgYmax(double val)
{
    lgYmax = val;
    Ymax = pow(yLogBase, lgYmax);
}

void ZeGraphView::setXmin(double val)
{
    Xmin = val;
    lgXmin = log(Xmin)/log(xLogBase);
}

void ZeGraphView::setXmax(double val)
{
    Xmax = val;
    lgXmax = log(Xmax)/log(xLogBase);
}

void ZeGraphView::setYmin(double val)
{
    Ymin = val;
    lgYmin = log(Ymin)/log(yLogBase);
}

void ZeGraphView::setYmax(double val)
{
    Ymax = val;
    lgYmax = log(Ymax)/log(yLogBase);
}
