#include "themedcolor.h"
#include "Utils/yaml.h"

#include <QGuiApplication>
#include <QStyleHints>

bool isDarkTheme()
{
  return qGuiApp->styleHints()->colorScheme() != Qt::ColorScheme::Light;
}

const QColor& ThemedColor::getCurrent() const
{
  if (isDarkTheme())
    return dark;
  else return light;
}

void ThemedColor::setCurrent(QColor c)
{
  if (isDarkTheme())
    dark = c;
  else
    light = c;
}

std::optional<ThemedColor::POD> ThemedColor::exportPod(ThemedColor defaultColors) const
{
  auto pod = POD {
    .dark = zg::yml::not_default(dark, defaultColors.dark).transform([](QColor c){ return c.name().toStdString(); }),
    .light = zg::yml::not_default(light, defaultColors.light).transform([](QColor c){ return c.name().toStdString(); }),
  };

  if (pod)
    return pod;
  else return {};
}

void ThemedColor::importPod(POD p)
{
  if (p.dark)
    dark = QColor::fromString(QString::fromStdString(*p.dark));

  if (p.light)
    light = QColor::fromString(QString::fromStdString(*p.light));
}
