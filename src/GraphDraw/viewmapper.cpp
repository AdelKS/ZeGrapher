#include "viewmapper.h"

namespace zg {

void ZeViewMapper::zoomView(point<u<plane::pixel>> center, double ratio)
{
  x.zoomView(center.x, ratio);
  y.zoomView(center.y, ratio);
}

void ZeViewMapper::translateView(point<u<plane::pixel>> vec)
{
  x.translateView(vec.x);
  y.translateView(vec.y);
}

void ZeViewMapper::setAxesSettings(const ZeAxesSettings &axesSettings)
{
  x.setAxisSettings(axesSettings.x.axisType, axesSettings.x.logSettings.base);
  y.setAxisSettings(axesSettings.y.axisType, axesSettings.y.logSettings.base);
}

ZeViewMapper& ZeViewMapper::setGraphRect(const QRectF &rect)
{
  using pixel_unit = u<plane::pixel>;

  x.setRange(Range1D<pixel_unit>{.min = unit<pixel_unit>{double(rect.left())},
                                 .max = unit<pixel_unit>{double(rect.right())}});

  y.setRange(Range1D<pixel_unit>{.min = unit<pixel_unit>{double(rect.top())},
                                 .max = unit<pixel_unit>{double(rect.bottom())}});

  return *this;
}

QRectF ZeViewMapper::getGraphRect() const
{
  const auto& xr = x.getRange<plane::pixel>();
  const auto& yr = y.getRange<plane::pixel>();
  return QRectF(xr.min.v, yr.min.v, xr.amplitude().v, yr.amplitude().v);
}

void ZeViewMapper::setGraphRange(const real_range2d& range)
{
  x.setRange(range.x);
  y.setRange(range.y);
}

} // namespace zg
