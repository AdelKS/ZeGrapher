#include "gridsettings.h"
#include "yaml.h"

std::optional<Ze1DSubgridSettings::POD> Ze1DSubgridSettings::exportPod() const
{
  POD p {
    .show = zg::yml::not_default(show, defaultShow),
    .line_width = zg::yml::not_default(lineWidth, defaultLineWidth),
    .color = color.exportPod(defaultColor),
    .subdivs = zg::yml::not_default(subdivs, defaultSubdivs)
  };

  if (p)
    return p;
  else return {};
}

void Ze1DSubgridSettings::importPod(POD p)
{
  if (p.show)
    show = *p.show;

  if (p.line_width)
    lineWidth = *p.line_width;

  if (p.color)
    color.importPod(std::move(*p.color));

  if (p.subdivs and *p.subdivs >= 1)
    subdivs = *p.subdivs;
}

std::optional<Ze1DGridCommonSettings::POD> Ze1DGridCommonSettings::exportPod() const
{
  POD p {
    .show = zg::yml::not_default(show, defaultShow),
    .line_width = zg::yml::not_default(lineWidth, defaultLineWidth),
    .color = color.exportPod(defaultColor),
  };

  if (p)
    return p;
  else return {};
}

void Ze1DGridCommonSettings::importPod(POD p)
{
  if (p.show)
    show = *p.show;

  if (p.line_width)
    lineWidth = *p.line_width;

  if (p.color)
    color.importPod(std::move(*p.color));
}

std::optional<ZeGridSettings::POD> ZeGridSettings::exportPod() const
{
  POD p {
    .x = x.exportPod(),
    .y = y.exportPod()
  };

  if (p)
    return p;
  else return {};
}

void ZeGridSettings::importPod(POD p)
{
  if (p.x)
    x.importPod(std::move(*p.x));

  if (p.y)
    y.importPod(std::move(*p.y));
}


std::optional<ZeSubgridSettings::POD> ZeSubgridSettings::exportPod() const
{
  POD p {
    .x = x.exportPod(),
    .y = y.exportPod()
  };

  if (p)
    return p;
  else return {};
}

void ZeSubgridSettings::importPod(POD p)
{
  if (p.x)
    x.importPod(std::move(*p.x));

  if (p.y)
    y.importPod(std::move(*p.y));
}
