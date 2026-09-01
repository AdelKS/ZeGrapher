#pragma once

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

#include <QLocale>
#include <QUrl>
#include <QVariantList>
#include <QtQmlIntegration/qqmlintegration.h>

namespace zg {

/// @brief the manual of website/content/, cut into the blocks the viewer draws
///
/// The viewer never gets the manual as one piece of markdown. A TextEdit in
/// MarkdownText drops the top of a long page that holds pictures once it
/// scrolls out of view, and never draws it again. The pictures thus leave the
/// markdown and become blocks of their own. Every heading also takes a block of
/// its own, which is what a link jumps to, and what the viewer puts space
/// around.
///
/// The manual is one file per language. A link inside it thus points at a
/// heading of that file, or outside the app.
class Documentation : public QObject
{
  Q_OBJECT
  QML_ELEMENT

  /// @brief QLocale::Language of the interface. A language the manual has no translation of falls back to English
  Q_PROPERTY(int language READ getLanguage WRITE setLanguage NOTIFY languageChanged)

  /// @brief the manual, in order:
  ///        {kind: 'text', text, anchor} and {kind: 'image', source, width, height}
  ///        A text block with an anchor holds one heading and nothing else.
  Q_PROPERTY(QVariantList blocks READ getBlocks NOTIFY blocksChanged)

public:
  explicit Documentation(QObject* parent = nullptr);

  int getLanguage() const { return language; }
  QVariantList getBlocks() const { return blocks; }

  void setLanguage(int lang);

  /// @brief the block that starts with that heading, -1 when no block does
  /// @param anchor '#the-size-of-the-graph', with or without the '#'
  Q_INVOKABLE int blockOfAnchor(QString anchor) const;

signals:
  void languageChanged();
  void blocksChanged();

private:
  void load();

  /// @brief cuts the markdown at every heading and at every picture
  QVariantList cut(const QString& body, const QUrl& base) const;

  int language = QLocale::English;
  QVariantList blocks;
};

}
