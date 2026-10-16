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

  // a sheet without a positive size fills the window
  if (p.width.value_or(0) > 0 and p.height.value_or(0) > 0)
  {
    sheetFillsWindow = false;
    cmSheetSize = QSizeF(*p.width, *p.height);
  }
}
