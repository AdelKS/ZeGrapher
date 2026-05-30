#include "Utils/plotstyle.h"
#include "globalvars.h"

namespace zg {

PlotStyle::PlotStyle(QObject* parent)
  : QObject(parent)
{
  connect(this, &PlotStyle::visibleChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::colorChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::lineWidthChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::lineStyleChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::drawLineChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::pointWidthChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::pointStyleChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::rangeChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::secondColorChanged, this, &PlotStyle::updated);

  connect(this, &PlotStyle::updated, &information, &Information::styleUpdated);
}

QList<qreal> PlotStyle::getDashPattern() const
{
  switch (lineStyle)
  {
    case Dash: return {4., 2.};
    case DashDot: return {4., 2., 2., 2.};
    case Dot: return {1., 2.};
    case NoLine: return {};
    case Solid: return {};
  }
  return {};
}

QColor PlotStyle::colorLerp(double t) const
{
  const QColor &a = color.getCurrent(), &b = secondColor.getCurrent();
  return QColor::fromRgbF(
      std::lerp(a.redF(), b.redF(), t),
      std::lerp(a.greenF(), b.greenF(), t),
      std::lerp(a.blueF(), b.blueF(), t));
}

void PlotStyle::setVisible(bool v)
{
  if (visible != v)
  {
    visible = v;
    emit visibleChanged();
  }
}

PlotStyle::LineStyle PlotStyle::defaultLineStyle() const
{
  if (discrete)
    return LineStyle::NoLine;
  else return LineStyle::Solid;
}

PlotStyle::PointStyle PlotStyle::defaultPointStyle() const
{
  if (discrete)
    return PointStyle::Square;
  else return PointStyle::None;
}

void PlotStyle::setDiscrete(bool d)
{
  if (d == discrete) return;
  if (d)
  {
    if (lineStyle != LineStyle::NoLine)
    {
      lineStyle = LineStyle::NoLine;
      emit lineStyleChanged();
    }
    if (pointStyle == PointStyle::None)
    {
      pointStyle = PointStyle::Square;
      emit pointStyleChanged();
    }
  }
  else
  {
    if (lineStyle == LineStyle::NoLine)
    {
      lineStyle = LineStyle::Solid;
      emit lineStyleChanged();
    }
    if (pointStyle != PointStyle::None)
    {
      pointStyle = PointStyle::None;
      emit pointStyleChanged();
    }
  }
  discrete = d;
}

}
