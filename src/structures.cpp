#include "structures.h"
#include "Utils/yaml.h"

Point operator*(Point &pt1, const Point &pt2)
{
  return {pt1.x * pt2.x, pt1.y * pt2.y};
}

QPointF toQPointF(const Point &pt)
{
  return QPointF(pt.x, pt.y);
}

bool ptCompY(const Point &pt1, const Point &pt2)
{
  return pt1.y < pt2.y;
}

Point operator * (const Point &pt1, const Point &pt2)
{
  return Point {.x = pt1.x * pt2.x, .y = pt1.y * pt2.y };
}

std::optional<ZeSizeSettings::POD> ZeSizeSettings::exportPod() const
{
  ZeSizeSettings::POD p;

  p.scaling = zg::yml::not_default(scalingFactor, 1.0);

  if (not sheetFillsWindow)
  {
    p.unit = sizeUnit;
    if (sizeUnit == SizeUnit::PIXEL)
    {
      p.width = pxSheetSize.width();
      p.height = pxSheetSize.height();
    }
    else
    {
      p.width = cmSheetSize.width();
      p.height = cmSheetSize.height();
    }
  }

  if (p)
    return p;
  else return {};
}

void ZeSizeSettings::importPod(POD p)
{
  if (p.scaling)
    scalingFactor = *p.scaling;

  if (p.width or p.height)
  {
    sheetFillsWindow = false;
    if (p.unit)
      sizeUnit = *p.unit;
    else sizeUnit = SizeUnit::PIXEL;

    if (sizeUnit == SizeUnit::PIXEL)
    {
      if (p.width) pxSheetSize.setWidth(*p.width);
      if (p.height) pxSheetSize.setHeight(*p.height);
    }
    else
    {
      if (p.width) cmSheetSize.setWidth(*p.width);
      if (p.height) cmSheetSize.setHeight(*p.height);
    }
  }

  if (p.unit)
    sizeUnit = *p.unit;
}
