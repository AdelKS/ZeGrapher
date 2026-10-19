/****************************************************************************
**  Copyright (c) 2026, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU Affero General Public License as published by the
**  Free Software Foundation, either version 3 of the License, or (at your
**  option) any later version.
**
**  This file is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#pragma once

#include <QFont>
#include <QPointF>
#include <QRectF>
#include <QString>

#include <vector>

#include "textmetrics.h"

enum class TextRole { Tick, Exponent, Title };

/// @brief a text whose baseline starts at 'origin', turned by 'angle' degrees
///        around it
struct Label
{
  QString text;
  TextRole role = TextRole::Tick;
  QPointF origin;
  double angle = 0;

  bool operator==(const Label&) const = default;
};

/// @brief a tick label: 'base', then 'exponent' raised in a smaller font, then
///        'tail'
/// @note the graph font has no superscript digits, and a fallback font would
///       change the layout from one system to another
struct MathText
{
  QString base = {}, exponent = {}, tail = {};

  bool operator==(const MathText&) const = default;
};

/// @brief the minus sign, which is wider than the hyphen that QString::number()
///        writes
inline constexpr QChar minusSign = QChar(0x2212);

/// @brief 'v' with 'precision' significant digits, a minus sign, and '×10'
///        with a raised exponent instead of an 'e'
inline MathText formatNumber(double v, int precision)
{
  const QString digits = QString::number(v, 'g', precision);

  MathText text;
  if (const qsizetype e = digits.indexOf('e'); e >= 0)
  {
    text.base = digits.first(e) + "×10";
    text.exponent = QString::number(digits.sliced(e + 1).toInt());
  }
  else text.base = digits;

  text.base.replace('-', minusSign);
  text.exponent.replace('-', minusSign);
  return text;
}

/// @brief measures and places MathText, with one TextMetrics per font
class MathTextMetrics
{
public:
  MathTextMetrics(const QFont& font, const QPaintDevice* device)
    : base(font, device), exponent(exponentFont(font), device) {}

  /// @brief the font of an exponent, smaller than the font of its text
  static QFont exponentFont(QFont font)
  {
    font.setPointSizeF(font.pointSizeF() * 0.7);
    return font;
  }

  void setFont(const QFont& font)
  {
    base.setFont(font);
    exponent.setFont(exponentFont(font));
  }

  qreal horizontalAdvance(const QString& text) const { return base.horizontalAdvance(text); }
  qreal horizontalAdvance(QChar c) const { return base.horizontalAdvance(c); }

  qreal horizontalAdvance(const MathText& text) const
  {
    return parts(text)[2].offset.x() + base.horizontalAdvance(text.tail);
  }

  QRectF boundingRect(const MathText& text) const
  {
    return united(text, [](const TextMetrics& m, const QString& s) { return m.boundingRect(s); });
  }

  QRectF tightBoundingRect(const MathText& text) const
  {
    return united(text, [](const TextMetrics& m, const QString& s) { return m.tightBoundingRect(s); });
  }

  /// @brief the labels that draw 'text', its baseline starting at 'origin'
  std::vector<Label> labels(const MathText& text, QPointF origin) const
  {
    std::vector<Label> found;
    for (const Part& part : parts(text))
      if (not part.text.isEmpty())
        found.push_back(Label{.text = part.text, .role = part.role, .origin = origin + part.offset});
    return found;
  }

protected:
  struct Part
  {
    QString text;
    TextRole role;
    const TextMetrics& metrics;
    QPointF offset;
  };

  /// @brief the three parts of 'text', each with its offset from the origin
  std::vector<Part> parts(const MathText& text) const
  {
    // an exponent's baseline sits a bit under the height of a digit
    const qreal raise = 0.45 * base.getFont().pointSizeF();
    const qreal afterBase = base.horizontalAdvance(text.base) + (text.exponent.isEmpty() ? 0 : 1);
    const qreal afterExponent = afterBase + exponent.horizontalAdvance(text.exponent);

    return {{text.base, TextRole::Tick, base, QPointF()},
            {text.exponent, TextRole::Exponent, exponent, QPointF(afterBase, -raise)},
            {text.tail, TextRole::Tick, base, QPointF(afterExponent, 0)}};
  }

  QRectF united(const MathText& text, auto measure) const
  {
    QRectF rect;
    for (const Part& part : parts(text))
      if (not part.text.isEmpty())
        rect = rect.united(measure(part.metrics, part.text).translated(part.offset));
    return rect;
  }

  TextMetrics base, exponent;
};
