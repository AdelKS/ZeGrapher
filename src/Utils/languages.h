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
**  Affero General Public License for more details.
**
**  You should have received a copy of the GNU Affero General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QList>
#include <QLocale>
#include <QString>

/// @brief every language the app can show, English first
/// @note the list comes from the translations the build embeds, so adding a
///       language only takes a new .ts file
inline const QList<QLocale::Language>& supportedLangs()
{
  static const QList<QLocale::Language> langs = []
  {
    // English needs no translation file: it's the source language
    QList<QLocale::Language> found = {QLocale::English};

    for (const QString& file: QDir(":/translations").entryList({"ZeGrapher_*.qm"}, QDir::Files))
    {
      const QString code = QFileInfo(file).completeBaseName().section('_', 1);
      if (const auto lang = QLocale::codeToLanguage(code); lang != QLocale::AnyLanguage)
        found.append(lang);
    }

    return found;
  }();

  return langs;
}

/// @brief the first language of the user's list that the app can show,
///        English otherwise
/// @note QLocale::system().language() follows the regional format on Windows,
///       and the app bundle's localizations on macOS
inline QLocale::Language systemLanguage()
{
  for (const QString& name: QLocale::system().uiLanguages())
    if (const auto lang = QLocale(name).language(); supportedLangs().contains(lang))
      return lang;

  return QLocale::English;
}

/// @brief a language's name in that language, "Français" for French
inline QString langToNativeName(QLocale::Language lang)
{
  // QLocale can't name a language alone: it picks a territory first, and the
  // resulting locale name can include that territory. English comes out as
  // "American English" and Spanish as "español de España". These two are the
  // languages where that happens.
  static const QHash<QLocale::Language, QString> withoutTerritory {
    {QLocale::English, "English"},
    {QLocale::Spanish, "Español"},
  };

  if (const auto it = withoutTerritory.constFind(lang); it != withoutTerritory.constEnd())
    return *it;

  QString name = QLocale(lang).nativeLanguageName();
  if (not name.isEmpty())
    name[0] = name[0].toUpper();

  return name;
}
