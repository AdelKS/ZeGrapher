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

#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QList>
#include <QLocale>
#include <QString>

/// @brief every language the app can show, English first
/// @note the list is read from the translations the build embedded, so a new
///       language is a new .ts file and nothing else
inline const QList<QLocale::Language>& supportedLangs()
{
  static const QList<QLocale::Language> langs = []
  {
    // English needs no translation file: it is what the source code holds
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

/// @brief the OS language if the app can show it, English otherwise
inline QLocale::Language systemLanguage()
{
  const auto lang = QLocale::system().language();
  return supportedLangs().contains(lang) ? lang : QLocale::English;
}

/// @brief the name of a language written in that language, "Français" for French
inline QString langToNativeName(QLocale::Language lang)
{
  // QLocale cannot name a language on its own: it picks a territory first, and
  // the name of the locale it lands on can carry that territory. English comes
  // out as "American English" and Spanish as "español de España". The two
  // below are the languages where that happens.
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
