#include "axissettings.h"
#include "Utils/yaml.h"

std::optional<ZeAxisSettings::POD> ZeAxisSettings::exportPod() const
{
  POD p {
    .tick_spacing = zg::yml::not_default(tickRelSpacing, 0),
    .max_digits = zg::yml::not_default(linSettings.maxDigitsNum, 3),
    .multiplier = zg::yml::not_default(linSettings.constantMultiplierStr)
  };

  if (p)
    return p;
  else return {};
}

void ZeAxisSettings::importPod(ZeAxisSettings::POD p)
{
  if (p.tick_spacing)
    tickRelSpacing = *p.tick_spacing;

  if (p.multiplier)
    linSettings.constantMultiplierStr = QString::fromStdString(*p.multiplier);

  if (p.max_digits)
    linSettings.maxDigitsNum = *p.max_digits;
}

std::optional<ZeAxesSettings::POD> ZeAxesSettings::exportPod() const
{
  POD p {
    .color = color.exportPod(defaultColor),
    .line_width = zg::yml::not_default(lineWidth, defaultLineWith),
    .x = x.exportPod(),
    .y = y.exportPod()
  };

  if (p)
    return p;
  else return {};
}

void ZeAxesSettings::importPod(ZeAxesSettings::POD p)
{
  if (p.color)
    color.importPod(*p.color);

  if (p.line_width)
    lineWidth = *p.line_width;

  if (p.x)
    x.importPod(std::move(*p.x));

  if (p.y)
    y.importPod(std::move(*p.y));
}
