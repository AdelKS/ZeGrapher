#include "themedcolor.h"

#include <QGuiApplication>
#include <QStyleHints>

const QColor& ThemedColor::getCurrent() const
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    return dark;
  else return light;
}

void ThemedColor::setCurrent(QColor c)
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    dark = c;
  else
    light = c;
}
