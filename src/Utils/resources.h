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
#include <QFile>
#include <QLocale>
#include <QString>

/// @brief the whole content of a file, empty when the file cannot be read
inline QString readTextFile(const QString& path)
{
  QFile file(path);
  if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    return {};

  return QString::fromUtf8(file.readAll());
}

/// @brief the embedded website folder of a language, ':/website/fr'. It falls
///        back to ':/website/en' when the build embedded no such folder
/// @note website/meson.build embeds the manual, which the app and the site share
inline QString websiteFolder(QLocale::Language lang)
{
  const QString folder = ":/website/" + QLocale::languageToCode(lang);
  return QDir(folder).exists() ? folder : ":/website/en";
}
