#include "Utils/plotstyle.h"
#include "Utils/utils.h"
#include "globalvars.h"

namespace zg {

PlotStyle::PlotStyle(QObject* parent)
  : QObject(parent)
{
  connect(this, &PlotStyle::visibleChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::colorChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::lineWidthChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::dashPatternChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::drawLineChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::pointWidthChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::pointStyleChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::rangeChanged, this, &PlotStyle::updated);
  connect(this, &PlotStyle::secondColorChanged, this, &PlotStyle::updated);

  connect(this, &PlotStyle::updated, &information, &Information::styleUpdated);
}

QColor PlotStyle::colorLerp(double t) const
{
  const QColor &a = color.getCurrent(), &b = secondColor.getCurrent();
  return QColor::fromRgbF(
      std::lerp(a.redF(), b.redF(), t),
      std::lerp(a.greenF(), b.greenF(), t),
      std::lerp(a.blueF(), b.blueF(), t));
}

void PlotStyle::setVisible(bool v)
{
  if (visible != v)
  {
    visible = v;
    emit visibleChanged();
  }
}

YAML::Emitter& operator << (YAML::Emitter& o, const PlotStyle& s)
{
  static PlotStyle defaultStyle;

  o << YAML::BeginMap;

  if (s.visible != defaultStyle.visible)
  {
    o << YAML::Key << "visible";
    o << YAML::Value << s.visible;
  }

  auto saveColor = [&](const ThemedColor& color, const ThemedColor& defaultColor) {
    o << YAML::BeginMap;
    if (color.dark != defaultColor.dark)
    {
      o << YAML::Value << "dark";
      o << YAML::Key << color.dark.name().toStdString();
    }
    if (color.light != defaultColor.light)
    {
      o << YAML::Value << "light";
      o << YAML::Key << color.light.name().toStdString();
    }
    o << YAML::EndMap;
  };

  if (s.color != defaultStyle.color)
  {
    o << YAML::Key << "color";
    saveColor(s.color, defaultStyle.color);
  }

  if (s.secondColor != defaultStyle.secondColor)
  {
    o << YAML::Key << "second_color";
    saveColor(s.color, defaultStyle.color);
  }

  if (s.lineWidth != defaultStyle.lineWidth)
  {
    o << YAML::Key << "line_width";
    o << YAML::Value << s.lineWidth;
  }

  if (s.drawLine != defaultStyle.drawLine)
  {
    o << YAML::Key << "draw_line";
    o << YAML::Value << s.drawLine;
  }

  if (not s.dashPattern.empty())
  {
    o << YAML::Key << "dash_pattern";
    o << YAML::Value << YAML::BeginSeq << YAML::Flow << std::vector<double>(s.dashPattern.begin(), s.dashPattern.end()) << YAML::EndSeq;
  }

  if (s.pointWidth != defaultStyle.pointWidth)
  {
    o << YAML::Key << "point_width";
    o << YAML::Value << s.pointWidth;
  }

  if (s.pointStyle != defaultStyle.pointStyle)
  {
    o << YAML::Key << "point_style";
    o << YAML::Value << enumValToLowercaseStr(s.pointStyle).toStdString();
  }

  if (s.coordinateSystem != defaultStyle.coordinateSystem)
  {
    o << YAML::Key << "coordinate_system";
    o << YAML::Value << enumValToLowercaseStr(s.coordinateSystem).toStdString();
  }

  o << YAML::EndMap;

  return o;
}

}
