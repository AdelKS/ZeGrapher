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
#include <QtQmlIntegration/qqmlintegration.h>

namespace zg {

/// @brief the words that ask for a donation, and the page that takes it
///
/// The words are the 'donation' of website/content/<lang>/strings.yaml, the
/// same string the download panel of the site shows. The page is the 'donate'
/// link of website/build-config/build.yaml.
class Donation : public QObject
{
  Q_OBJECT
  QML_ELEMENT

  /// @brief QLocale::Language of the interface. English stands in for a
  ///        language that has no translation of the words
  Q_PROPERTY(int language READ getLanguage WRITE setLanguage NOTIFY wordsChanged)

  /// @brief the markdown that asks for a donation
  Q_PROPERTY(QString words READ getWords NOTIFY wordsChanged)

  /// @brief the donation page
  Q_PROPERTY(QUrl page READ getPage CONSTANT)

public:
  explicit Donation(QObject* parent = nullptr): QObject(parent) {}

  int getLanguage() const { return language; }
  QString getWords() const;
  QUrl getPage() const { return QUrl(DONATE_URL); }

  void setLanguage(int lang);

signals:
  void wordsChanged();

private:
  int language = QLocale::English;
};

}
