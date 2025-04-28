#include "Utils/plotstyle.h"
#include "information.h"

namespace zg {

PlotStyle::PlotStyle(QObject* parent)
  : QObject(parent)
{
  connect(this, &PlotStyle::visibleChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::colorChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::lineWidthChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::lineStyleChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::pointWidthChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::pointStyleChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::coordinateSystemChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::continuousChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::rangeChanged, this, &PlotStyle::updated);

  connect(this, &PlotStyle::updated, &information, &Information::styleUpdated);
}

double PlotStyle::getStart()
{
  return range.min.v;
}

double PlotStyle::getEnd()
{
  return range.max.v;
}

double PlotStyle::getStep()
{
  return step.v;
}

void PlotStyle::setStart(double v)
{
  range.min.v = v;
}

void PlotStyle::setEnd(double v)
{
  range.max.v = v;
}

void PlotStyle::setStep(double v)
{
  step.v = v;
}

}
