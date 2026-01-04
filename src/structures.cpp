#include "structures.h"

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

QColor ZeAppSettings::getValidSyntax() const
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    return validSyntaxDark;
  else return validSyntaxLight;
}

void ZeAppSettings::colorSchemeChanged()
{
  if (invalidSyntaxDark != invalidSyntaxLight)
    emit invalidSyntaxChanged();
  if (validSyntaxDark != validSyntaxLight)
    emit validSyntaxChanged();
  if (warningSyntaxDark != warningSyntaxLight)
    emit warningSyntaxChanged();
}

QColor ZeAppSettings::getInvalidSyntax() const
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    return invalidSyntaxDark;
  else return invalidSyntaxLight;
}

QColor ZeAppSettings::getWarningSyntax() const
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    return warningSyntaxDark;
  else return warningSyntaxLight;
}

void ZeAppSettings::setValidSyntax(QColor c)
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    validSyntaxDark = c;
  else validSyntaxLight = c;
  emit validSyntaxChanged();
}

void ZeAppSettings::setInvalidSyntax(QColor c)
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    invalidSyntaxDark = c;
  else invalidSyntaxLight = c;
  emit invalidSyntaxChanged();
}

void ZeAppSettings::setWarningSyntax(QColor c)
{
  if (qGuiApp->styleHints()->colorScheme() == Qt::ColorScheme::Dark)
    warningSyntaxDark = c;
  else warningSyntaxLight = c;
  emit warningSyntaxChanged();
}
