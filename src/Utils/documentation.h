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

/// @brief the manual of one language, cut into the blocks that DocPane draws
///
/// The viewer never receives the manual as one piece of markdown. A TextEdit in
/// MarkdownText loses the top of a long page that holds pictures once it
/// scrolls out of view, and never draws it again. So the pictures leave the
/// markdown and become blocks of their own. Each heading also becomes a block,
/// which is what a link jumps to, and what the viewer puts space around.
///
/// The manual is one file per language, so a link inside it points at a heading
/// of the same file, or out of the app.
class Documentation : public QObject
{
  Q_OBJECT
  QML_ELEMENT

  /// @brief QLocale::Language of the interface. English stands in for a
  ///        language the manual has no translation of
  Q_PROPERTY(int language READ getLanguage WRITE setLanguage NOTIFY languageChanged)

  /// @brief the manual in order, one map per block:
  ///        {kind: 'text', text, anchor} or {kind: 'image', source, width, height}
  ///        A text block that carries an anchor holds one heading and nothing else.
  Q_PROPERTY(QVariantList blocks READ getBlocks NOTIFY blocksChanged)

public:
  explicit Documentation(QObject* parent = nullptr);

  int getLanguage() const { return language; }
  QVariantList getBlocks() const { return blocks; }

  void setLanguage(int lang);

  /// @brief the index of the block that holds that heading, -1 when no block does
  /// @param anchor '#the-size-of-the-graph', with or without the '#'
  Q_INVOKABLE int blockOfAnchor(QString anchor) const;

signals:
  void languageChanged();
  void blocksChanged();

private:
  void load();

  /// @brief cuts the markdown at every heading and at every picture
  QVariantList cut(const QString& text, const QUrl& base) const;

  int language = QLocale::English;
  QVariantList blocks;
};

}
