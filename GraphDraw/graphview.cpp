#include "graphview.h"

ZeGraphView::ZeGraphView(QObject *parent) : QObject(parent)
{
    Xmin = Ymin = -10;
    Xmax = Ymax = 10;
    lgXmin = lgYmin = -1;
    lgXmax = lgYmax = 1;
    xLogBase = yLogBase = 10;
    m_viewType = ZeScaleType::LINEAR;
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

    m_viewType = other.m_viewType;
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

    m_viewType = other.m_viewType;
}

QRectF ZeGraphView::rect()
{
    QRectF graphWin;
    graphWin.setBottom(Ymin);
    graphWin.setTop(Ymax);
    graphWin.setLeft(Xmin);
    graphWin.setRight(Xmax);
    return graphWin;
}

QRectF ZeGraphView::lgRect()
{
    QRectF graphlLgWin;
    graphlLgWin.setBottom(lgYmin);
    graphlLgWin.setTop(lgYmax);
    graphlLgWin.setLeft(lgXmin);
    graphlLgWin.setRight(lgXmax);
    return graphlLgWin;
}

QRectF ZeGraphView::viewRect()
{
    QRect viewWin;

    if(viewType == ZeScaleType::X_LOG || viewType == ZeScaleType::XY_LOG)
    {
        viewWin.setLeft(lgXmin);
        viewWind.setRight(lgXmax);
    }
    else
    {
        viewWin.setLeft(Xmin);
        viewWind.setRight(Xmax);
    }

    if(viewType == ZeScaleType::Y_LOG || viewType == ZeScaleType::XY_LOG)
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
    if(viewType == ZeScaleType::Y_LOG || viewType == ZeScaleType::XY_LOG)
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
}

void ZeGraphView::zoomXview(double ratio)
{
    if(viewType == ZeScaleType::X_LOG || viewType == ZeScaleType::XY_LOG)
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
}

void ZeGraphView::zoomView(QPointF center, double ratio)
{
    if((Xmax - Xmin > MIN_RANGE && Ymax - Ymin > MIN_RANGE) || ratio < 0)
    {
        if(viewType == ZeScaleType::X_LOG || viewType == ZeScaleType::XY_LOG)
        {
            setlgXmax(lgXmax - (lgXmax - center.x())*ratio);
            setlgXmin(lgXmin - (lgXmin - center.x())*ratio);
        }
        else
        {
            setXmax(Xmax - (Xmax - center.x())*ratio);
            setXmin(Xmin - (Xmin - center.x())*ratio);
        }

        if(viewType == ZeScaleType::Y_LOG || viewType == ZeScaleType::XY_LOG)
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

 ZeGrid ZeGraphView::getGrid(QRect graphRectPx)
 {
     ZeGrid grid;

     if(scaleType == ZeScaleType::LINEAR || scaleType == ZeScaleType::LINEAR_ORTHONORMAL)
     {
         grid.horizontal = getOneDirectionLinearGrid(graphRectPx.width(), xGridStep, Xmin, Xmax, gridSettings.horSubGridLineCount,
                                                     30, 150);
         grid.vertical = getOneDirectionLinearGrid(graphRectPx.height(), yGridStep, Ymin, Ymax, gridSettings.verSubGridLineCount,
                                                   30, 100);
     }

     return grid;
 }

 ZeOneDirectionGrid ZeGraphView::getOneDirectionLinearGrid(int pxWidth, double &step, double min, double max,
                                                       int subGridlineCount, double minTickDist, double maxTickDist)
 {
     ZeOneDirectionGrid grid;

     ZeMainGridLine gridLine;
     ZeSubGridLine subGridLine;

     double scaling = pxWidth / (max-min);

     if(scaling * step < minTickDist)
         step *= pow(2, trunc(ln(minTickDist)/ln(step*scaling)) + 1);
     else if(scaling * step > maxTickDist)
         step *= pow(2, trunc(ln(maxTickDist)/ln(step*scaling)));


     gridLine.pos = (trunc(min / step)-1) * step;

     grid.mainGrid.push_back(pos);

     while(gridLine.pos < max)
     {
         gridLine.pos += step;
         grid.mainGrid.push_back(pos);
     }

     double count = subGridlineCount;
     double p1, p2;

     for(int i = 0 ; i < grid.mainGrid.size() - 1 ; i++)
     {
         p1 = grid.mainGrid[i].pos;
         p2 = grid.mainGrid[i+1].pos;

         for(double j = 1 ; j < count ; j++)
         {
             subGridLine.pos = j*p2 + (count-j)*p1;
             grid.subGrid.push_back(subGridLine);
         }
     }

     while(!grid.mainGrid.empty() && grid.mainGrid.front().pos < min) { grid.mainGrid.pop_front(); }
     while(!grid.subGrid.empty() && grid.subGrid.front().pos < min) { grid.subGrid.pop_front(); }

     while(!grid.mainGrid.empty() && grid.mainGrid.back().pos > max) { grid.mainGrid.pop_back(); }
     while(!grid.subGrid.empty() && grid.subGrid.back().pos > max) { grid.subGrid.pop_back(); }

     return grid;

 }

void ZeGraphView::translateView(QPointF vec)
{
    if(viewType == ZeScaleType::X_LOG || viewType == ZeScaleType::XY_LOG)
    {
        setlgXmax(lgXmax + vec.x());
        setlgXmin(lgXmin + vec.x());
    }
    else
    {
        setXmax(Xmax + vec.x());
        setXmin(Xmin + vec.x());
    }

    if(viewType == ZeScaleType::Y_LOG || viewType == ZeScaleType::XY_LOG)
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

void ZeGraphView::viewToUnit_y(double viewY) // viewY =
{
    if(viewType == ZeScaleType::Y_LOG || viewType == ZeScaleType::XY_LOG)
    {
        return pow(yLogBase, viewY);
    }
    else
    {
        return viewY;
    }
}

const double ZeGraphView::unitToView_y(double unitY)
{
    if(viewType == ZeScaleType::Y_LOG || viewType == ZeScaleType::XY_LOG)
    {
        return ln(unitY)/ln(yLogBase);
    }
    else
    {
        return unitY;
    }
}

const double ZeGraphView::viewToUnit_x(double viewX)
{
    if(viewType == ZeScaleType::X_LOG || viewType == ZeScaleType::XY_LOG)
    {
        return pow(xLogBase, viewX);
    }
    else
    {
        return viewX;
    }
}

const double ZeGraphView::unitToView_x(double unitX)
{
    if(viewType == ZeScaleType::X_LOG || viewType == ZeScaleType::XY_LOG)
    {
        return ln(unitX)/ln(xLogBase);
    }
    else
    {
        return unitX;
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

ZeScaleType ZeGraphView::viewType() const
{
    return m_viewType;
}
