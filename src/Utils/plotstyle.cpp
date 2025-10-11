#include "Utils/plotstyle.h"
#include "globalvars.h"

namespace zg {

PlotStyle::PlotStyle(QObject* parent)
  : QObject(parent),
    start(mathWorld.addAltExprObject()),
    end(mathWorld.addAltExprObject()),
    step(mathWorld.addAltExprObject())
{
  start->setImplicitName("start");
  end->setImplicitName("end");
  step->setImplicitName("step");
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

PlotStyle::~PlotStyle()
{
  mathWorld.removeAltExprObject(start);
  mathWorld.removeAltExprObject(end);
  mathWorld.removeAltExprObject(step);
}

zg::real_unit PlotStyle::getStart() const
{
  return {start->getValue()};
}

zg::real_unit PlotStyle::getEnd() const
{
  return {end->getValue()};
}

zg::real_unit PlotStyle::getStep() const
{
  return {step->getValue()};
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
