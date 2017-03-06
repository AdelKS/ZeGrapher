#include "graphview.h"

GraphView::GraphView(QObject *parent) : QObject(parent)
{
    Xmin = Ymin = -10;
    Xmax = Ymax = 10;
    lgXmin = lgYmin = -1;
    lgXmax = lgYmax = 1;
    xLogBase = yLogBase = 10;
    m_viewType = ScaleType::LINEAR;
}

GraphView::GraphView(const GraphView &other, QObject *parent) : QObject(parent)
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

GraphView& GraphView::operator=(const GraphView &other)
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

QRectF GraphView::rect()
{
    QRectF graphWin;
    graphWin.setBottom(Ymin);
    graphWin.setTop(Ymax);
    graphWin.setLeft(Xmin);
    graphWin.setRight(Xmax);
    return graphWin;
}

QRectF GraphView::lgRect()
{
    QRectF graphlLgWin;
    graphlLgWin.setBottom(lgYmin);
    graphlLgWin.setTop(lgYmax);
    graphlLgWin.setLeft(lgXmin);
    graphlLgWin.setRight(lgXmax);
    return graphlLgWin;
}

QRectF GraphView::viewRect()
{
    QRect viewWin;

    if(viewType == ScaleType::X_LOG || viewType == ScaleType::XY_LOG)
    {
        viewWin.setLeft(lgXmin);
        viewWind.setRight(lgXmax);
    }
    else
    {
        viewWin.setLeft(Xmin);
        viewWind.setRight(Xmax);
    }

    if(viewType == ScaleType::Y_LOG || viewType == ScaleType::XY_LOG)
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

void GraphView::zoomYview(double ratio)
{
    if(viewType == ScaleType::Y_LOG || viewType == ScaleType::XY_LOG)
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

void GraphView::zoomXview(double ratio)
{
    if(viewType == ScaleType::X_LOG || viewType == ScaleType::XY_LOG)
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

void GraphView::zoomView(QPointF center, double ratio)
{
    if((Xmax - Xmin > MIN_RANGE && Ymax - Ymin > MIN_RANGE) || ratio < 0)
    {
        if(viewType == ScaleType::X_LOG || viewType == ScaleType::XY_LOG)
        {
            setlgXmax(lgXmax - (lgXmax - center.x())*ratio);
            setlgXmin(lgXmin - (lgXmin - center.x())*ratio);
        }
        else
        {
            setXmax(Xmax - (Xmax - center.x())*ratio);
            setXmin(Xmin - (Xmin - center.x())*ratio);
        }

        if(viewType == ScaleType::Y_LOG || viewType == ScaleType::XY_LOG)
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

 ZeGridDescription getGrid(QRect graphRectPx)
 {

 }

void GraphView::translateView(QPointF vec)
{
    if(viewType == ScaleType::X_LOG || viewType == ScaleType::XY_LOG)
    {
        setlgXmax(lgXmax + vec.x());
        setlgXmin(lgXmin + vec.x());
    }
    else
    {
        setXmax(Xmax + vec.x());
        setXmin(Xmin + vec.x());
    }

    if(viewType == ScaleType::Y_LOG || viewType == ScaleType::XY_LOG)
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

void GraphView::realYvalueFromView(double viewY) // viewY =
{
    if(viewType == ScaleType::Y_LOG || viewType == ScaleType::XY_LOG)
    {
        return pow(yLogBase, viewY);
    }
    else
    {
        return viewY;
    }
}

void GraphView::realXvalueFromView(double viewX)
{
    if(viewType == ScaleType::X_LOG || viewType == ScaleType::XY_LOG)
    {
        pow(xLogBase, viewX);
    }
    else
    {
        return viewX;
    }
}

void GraphView::setlgXmin(double val)
{
    lgXmin = val;
    Xmin = pow(xLogBase, lgXmin);
}

void GraphView::setlgXmax(double val)
{
    lgXmax = val;
    Xmax = pow(xLogBase, lgXmax);
}

void GraphView::setlgYmin(double val)
{
    lgYmin = val;
    Ymin = pow(yLogBase, lgYmin);
}

void GraphView::setlgYmax(double val)
{
    lgYmax = val;
    Ymax = pow(yLogBase, lgYmax);
}

void GraphView::setXmin(double val)
{
    Xmin = val;
    lgXmin = log(Xmin)/log(xLogBase);
}

void GraphView::setXmax(double val)
{
    Xmax = val;
    lgXmax = log(Xmax)/log(xLogBase);
}

void GraphView::setYmin(double val)
{
    Ymin = val;
    lgYmin = log(Ymin)/log(yLogBase);
}

void GraphView::setYmax(double val)
{
    Ymax = val;
    lgYmax = log(Ymax)/log(yLogBase);
}

ScaleType GraphView::viewType() const
{
    return m_viewType;
}
