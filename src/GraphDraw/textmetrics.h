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
#include <QFontMetricsF>
#include <QHash>
#include <QRectF>
#include <QString>

#include <optional>

/// @brief QFontMetricsF that remembers what it measured
///
/// Each measurement shapes the text with HarfBuzz. The layout measures the
/// same tick labels on every animation frame, so measurements are kept until
/// the font changes.
class TextMetrics
{
public:
  TextMetrics(const QFont& font, const QPaintDevice* device)
    : font(font), device(device), metrics(font, device) {}

  /// @brief keeps the measurements when 'f' is the current font
  void setFont(const QFont& f)
  {
    if (f == font)
      return;
    font = f;
    metrics = QFontMetricsF(font, device);
    cache.clear();
  }

  QRectF boundingRect(const QString& text) const
  {
    return measure(&Entry::bounding, text, [&] { return metrics.boundingRect(text); });
  }

  QRectF tightBoundingRect(const QString& text) const
  {
    return measure(&Entry::tight, text, [&] { return metrics.tightBoundingRect(text); });
  }

  qreal horizontalAdvance(const QString& text) const
  {
    return measure(&Entry::advance, text, [&] { return metrics.horizontalAdvance(text); });
  }

  qreal horizontalAdvance(QChar c) const { return metrics.horizontalAdvance(c); }

  const QFont& getFont() const { return font; }

protected:
  struct Entry
  {
    std::optional<QRectF> bounding, tight;
    std::optional<qreal> advance;
  };

  template <class T>
  T measure(std::optional<T> Entry::* field, const QString& text, auto compute) const
  {
    std::optional<T>& m = entry(text).*field;
    if (not m)
      m = compute();
    return *m;
  }

  Entry& entry(const QString& text) const
  {
    // a long pan or zoom keeps hitting new numbers
    if (cache.size() > 4096)
      cache.clear();
    return cache[text];
  }

  QFont font;
  const QPaintDevice* device;
  QFontMetricsF metrics;
  mutable QHash<QString, Entry> cache;
};
