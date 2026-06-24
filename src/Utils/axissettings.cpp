#include "axissettings.h"
#include "Utils/yaml.h"

std::optional<ZeAxisSettings::POD> ZeAxisSettings::exportPod() const
{
  POD p {
    .title = zg::yml::not_default(title),
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

  if (p.title)
    title = QString::fromStdString(*p.title);
}

std::optional<ZeAxesSettings::POD> ZeAxesSettings::exportPod() const
{
  POD p {
    .color = color.exportPod(defaultColor),
    .line_width = zg::yml::not_default(lineWidth, defaultLineWith),
    .x = x.exportPod(),
    .y = y.exportPod(),
    .title_font = zg::yml::QFontPOD::from(titleFont, defaultTitleFont)
  };

  if (p)
    return p;
  else return {};
}

void ZeAxesSettings::setDefaultTitleFont(QFont f)
{
  if (titleFont == defaultTitleFont)
    titleFont = f;

  defaultTitleFont = f;
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

  if (p.title_font)
  {
    if (p.title_font->name)
      titleFont.setFamily(QString::fromStdString(*p.title_font->name));
    if (p.title_font->weight)
      titleFont.setWeight(*p.title_font->weight);
    if (p.title_font->size)
      titleFont.setPointSize(*p.title_font->size);
  }
}
