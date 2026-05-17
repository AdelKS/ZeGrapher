#include "Utils/plotstyle.h"
#include "globalvars.h"

namespace zg {

PlotStyle::PlotStyle(QObject* parent)
  : QObject(parent)
{
  connect(this, &PlotStyle::visibleChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::colorChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::lineWidthChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::dashPatternChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::drawLineChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::pointWidthChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::pointStyleChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::rangeChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::secondColorChanged, this, &PlotStyle::updated);

  connect(this, &PlotStyle::updated, &information, &Information::styleUpdated);
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

}
