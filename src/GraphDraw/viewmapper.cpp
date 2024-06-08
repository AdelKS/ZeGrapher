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
  x.setAxisSettings(axesSettings.y.axisType, axesSettings.y.logSettings.base);
  orthonormal = axesSettings.orthonormal;
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

void ZeViewMapper::setGraphRange(const GraphRange &range)
{
  using view_unit = u<plane::view>;

  x.setRange(
    Range1D<view_unit>{.min = unit<view_unit>{range.x.min}, .max = unit<view_unit>{range.x.max}});

  y.setRange(
    Range1D<view_unit>{.min = unit<view_unit>{range.y.min}, .max = unit<view_unit>{range.y.max}});
}

void ZeViewMapper::enforceOrthonormality()
{
  double uniteY = std::as_const(y).getRange<plane::pixel>().amplitude().v
                  / std::as_const(y).getRange<plane::view>().amplitude().v;

  double uniteX = std::as_const(x).getRange<plane::pixel>().amplitude().v
                  / std::as_const(x).getRange<plane::view>().amplitude().v;

  double sqrt_ratio = std::sqrt(uniteX / uniteY);
  y.zoomView(sqrt_ratio);
  x.zoomView(1. / sqrt_ratio);
}

} // namespace zg
