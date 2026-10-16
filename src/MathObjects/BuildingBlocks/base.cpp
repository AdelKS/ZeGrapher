#include "base.h"

namespace zg {

Base::Base(DefaultStringRangeProvider p, QObject* parent)
  : QObject(parent), start(this), end(this), defaultStringRangeProvider(p)
{
  start.setImplicitName("zg_internal_start");
  end.setImplicitName("zg_internal_end");
  resetStart();
  resetEnd();
}

StringRange Base::getDefaultStringRange() const
{
  return defaultStringRangeProvider(coordinateSystem, discrete);
}

bool Base::isDefaultStart() const
{
  return start.getExpression() == getDefaultStringRange().start;
}

bool Base::isDefaultEnd() const
{
  return end.getExpression() == getDefaultStringRange().end;
}

void Base::resetStart()
{
  start.setExpression(getDefaultStringRange().start);
}

void Base::resetEnd()
{
  end.setExpression(getDefaultStringRange().end);
}

void Base::setCoordinateSystem(CoordinateSystem s)
{
  if (s == coordinateSystem) return;

  const bool defStart = isDefaultStart();
  const bool defEnd = isDefaultEnd();

  coordinateSystem = s;

  if (defStart) resetStart();
  if (defEnd) resetEnd();

  emit coordinateSystemChanged();

}

void Base::setDiscrete(bool b)
{
  if (discrete == b) return;

  const bool defStart = isDefaultStart();
  const bool defEnd = isDefaultEnd();

  discrete = b;

  if (defStart) resetStart();
  if (defEnd) resetEnd();

  emit discreteChanged();
}

void Base::setSchrodinger(bool s)
{
  if (schrodinger == s) return;

  schrodinger = s;

  emit schrodingerChanged();
}

zg::real_range1d Base::getSnapshot() const
{
  return zg::real_range1d{.min = {start.evaluate()}, .max = {end.evaluate()}};
}

std::optional<zg::real_range1d> Base::getValidSnapshot() const
{
  const zg::real_range1d range = getSnapshot();

  if (not range.is_valid())
    return {};

  return range;
}

} // namespace zg
