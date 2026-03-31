#include "themedcolor.h"

#include <QGuiApplication>
#include <QStyleHints>

const QColor& ThemedColor::getCurrent()
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    return dark;
  else return light;
}

void ThemedColor::setCurrent(QColor c)
{
  const QColor hsv = c.toHsv();
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
  {
    dark = c;
    light.setHsv(hsv.hue(), hsv.saturation(), 255 - hsv.value());
  }
  else
  {
    light = c;
    dark.setHsv(hsv.hue(), hsv.saturation(), 255 - hsv.value());
  }
}
