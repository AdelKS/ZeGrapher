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
  connect(this, &PlotStyle::pointWidthChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::pointStyleChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::coordinateSystemChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::rangeChanged, this, &PlotStyle::updated);

  connect(this, &PlotStyle::updated, &information, &Information::styleUpdated);
}

zg::real_unit PlotStyle::getStart() const
{
  if (start)
    return {start->getValue()};
  else return {std::nan("")};
}

zg::real_unit PlotStyle::getEnd() const
{
  if (end)
    return {end->getValue()};
  else return {std::nan("")};
}

zg::real_unit PlotStyle::getStep() const
{
  if (step)
    return {step->getValue()};
  else return {std::nan("")};
}

zg::real_range1d PlotStyle::getRange() const
{
  return real_range1d{.min = getStart(), .max = getEnd()};
}

void PlotStyle::setObjectType(ObjectType newObjectType)
{
  if (objectType != newObjectType)
  {
    objectType = newObjectType;
    emit objectTypeChanged();
  }
}

}
