#include "viewmapper.h"

ZeViewMapper::ZeViewMapper(QObject *parent) : QObject(parent)
{
    targetTicksNum = TARGET_TICKS_NUM;
}

void ZeViewMapper::setViewSettings(const ZeViewSettings &viewSettings)
{
    axesSettings = viewSettings.axes;
    gridSettings = viewSettings.grid;

    setGraphRange(viewSettings.range);
}

void ZeViewMapper::setGraphRange(const GraphRange &range)
{
    setXmax(range.x.max);
    setYmax(range.y.max);
    setXmin(range.x.min);
    setYmin(range.y.min);
}

GraphRange ZeViewMapper::getGraphRange()
{
    GraphRange range;

    range.x.min = Xmin;
    range.y.min = Ymin;

    range.x.max = Xmax;
    range.y.max = Ymax;

    return range;
}

ZeViewMapper::ZeViewMapper(const ZeViewMapper &other, QObject *parent) : QObject(parent)
{
    Xmin = other.Xmin;
    Xmax = other.Xmax;
    Ymin = other.Ymin;
    Ymax = other.Ymax;

    lgXmin = other.lgXmin;
    lgXmax = other.lgXmax;
    lgYmin = other.lgYmin;
    lgYmax = other.lgYmax;
}

ZeViewMapper& ZeViewMapper::operator=(const ZeViewMapper &other)
{
    Xmin = other.Xmin;
    Xmax = other.Xmax;
    Ymin = other.Ymin;
    Ymax = other.Ymax;

    lgXmin = other.lgXmin;
    lgXmax = other.lgXmax;
    lgYmin = other.lgYmin;
    lgYmax = other.lgYmax;

    return *this;
}

QRectF ZeViewMapper::getRect() const
{
    QRectF graphWin;
    graphWin.setBottom(Ymin);
    graphWin.setTop(Ymax);
    graphWin.setLeft(Xmin);
    graphWin.setRight(Xmax);
    return graphWin;
}

QRectF ZeViewMapper::getLogRect() const
{
    QRectF graphlLgWin;
    graphlLgWin.setBottom(lgYmin);
    graphlLgWin.setTop(lgYmax);
    graphlLgWin.setLeft(lgXmin);
    graphlLgWin.setRight(lgXmax);
    return graphlLgWin;
}

void ZeViewMapper::setViewRect(QRectF rect)
{
    setViewXmax(rect.right());
    setViewXmin(rect.left());
    setViewYmax(rect.top());
    setViewYmin(rect.bottom());

    verifyOrthonormality();
}

QRectF ZeViewMapper::getViewRect() const
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

void ZeViewMapper::zoomYview(double ratio)
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

void ZeViewMapper::zoomXview(double ratio)
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

void ZeViewMapper::zoomView(QPointF center, double ratio)
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

ZeLinAxisTicks ZeViewMapper::getLinearAxisTicks(double windowWidth,
                                            ZeAxisRange range,
                                            ZeAxisName axisName,
                                            QFontMetrics metrics)
{

    ZeLinAxisTicks axisTicks;
    axisTicks.offset.sumOffset = 0;
    axisTicks.offset.powerOffset = 0;

    ZeLinAxisSettings axisSettings = axisName == ZeAxisName::X ? axesSettings.x.linSettings : axesSettings.y.linSettings;

    const double &constantMultiplier = axisSettings.constantMultiplier;
    double amplitude = range.amplitude();

    double amplitudeLog10 = floor(log10(amplitude));
    double minLog10 = floor(log10(range.min / constantMultiplier));

    if(amplitudeLog10 < minLog10 - axisSettings.maxDigitsNum)
    {
        double digitsNum = minLog10 - axisSettings.maxDigitsNum - amplitudeLog10;

        double tenPower = pow(10, digitsNum);
        axisTicks.offset.sumOffset = trunc(range.min * tenPower) / tenPower;
    }

    double targetPower = floor(log10(amplitude / constantMultiplier / targetTicksNum));

    double baseStep = pow(10, targetPower);
    double realStep = baseStep * constantMultiplier;
    double tenMult = 1;

    ZeLinAxisTick firstTick;
    firstTick.pos = ceil(range.min / realStep) * realStep;
    firstTick.multiplier = firstTick.pos / axisSettings.constantMultiplier - axisTicks.offset.sumOffset;

    axisTicks.ticks << firstTick;

    ZeLinAxisTick tick;

    while(axisTicks.ticks.last().pos < range.max)
    {
        tick.pos = axisTicks.ticks.last().pos + realStep;
        tick.multiplier = axisTicks.ticks.last().multiplier + baseStep;

        axisTicks.ticks << tick;
    }

    axisTicks.ticks.removeLast();

    return axisTicks;
}

void ZeViewMapper::translateView(QPointF vec)
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

double ZeViewMapper::viewToUnitY(double viewY) const // viewY =
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

double ZeViewMapper::unitToViewY(double unitY) const
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

double ZeViewMapper::getXmin()
{
    return Xmin;
}

double ZeViewMapper::getXmax()
{
    return Xmax;
}

double ZeViewMapper::getYmin()
{
    return Ymin;
}

double ZeViewMapper::getYmax()
{
    return Ymax;
}

double ZeViewMapper::viewToUnitX(double viewX) const
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

double ZeViewMapper::unitToViewX(double unitX) const
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

void ZeViewMapper::setViewXmin(double val)
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

void ZeViewMapper::setViewXmax(double val)
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

void ZeViewMapper::setViewYmin(double val)
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

void ZeViewMapper::setViewYmax(double val)
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

void ZeViewMapper::verifyOrthonormality()
{
    // TODO: update with only the Size, and to use with both orthonormal and linear
    if(viewWidget != 0 && viewType == ZeScaleType::LINEAR_ORTHONORMAL)
    {
        double uniteY = viewWidget->height() / getViewRect().height();
        double uniteX = viewWidget->width() / getViewRect().width();

        double ratio =  uniteX / uniteY;
        zoomYview(ratio);
    }
}

void ZeViewMapper::setlgXmin(double val)
{
    lgXmin = val;
    Xmin = pow(xLogBase, lgXmin);
}

void ZeViewMapper::setlgXmax(double val)
{
    lgXmax = val;
    Xmax = pow(xLogBase, lgXmax);
}

void ZeViewMapper::setlgYmin(double val)
{
    lgYmin = val;
    Ymin = pow(yLogBase, lgYmin);
}

void ZeViewMapper::setlgYmax(double val)
{
    lgYmax = val;
    Ymax = pow(yLogBase, lgYmax);
}

void ZeViewMapper::setXmin(double val)
{
    Xmin = val;
    lgXmin = log(Xmin)/log(xLogBase);
}

void ZeViewMapper::setXmax(double val)
{
    Xmax = val;
    lgXmax = log(Xmax)/log(xLogBase);
}

void ZeViewMapper::setYmin(double val)
{
    Ymin = val;
    lgYmin = log(Ymin)/log(yLogBase);
}

void ZeViewMapper::setYmax(double val)
{
    Ymax = val;
    lgYmax = log(Ymax)/log(yLogBase);
}
