#include "structures.h"
#include "Utils/yaml.h"

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
    p.width = cmSheetSize.width();
    p.height = cmSheetSize.height();
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
    if (p.width) cmSheetSize.setWidth(*p.width);
    if (p.height) cmSheetSize.setHeight(*p.height);
  }
}
