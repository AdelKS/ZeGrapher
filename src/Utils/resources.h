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
#include <QFile>
#include <QLocale>
#include <QString>

/// @brief a file's whole content, empty when it can't be read
inline QString readTextFile(const QString& path)
{
  QFile file(path);
  if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    return {};

  return QString::fromUtf8(file.readAll());
}

/// @brief a language's embedded website folder, ':/website/fr'. Falls back to
///        ':/website/en' when the build has no such folder
/// @note website/meson.build embeds the two files that the app and site share:
///       the manual and the donation text
inline QString websiteFolder(QLocale::Language lang)
{
  const QString folder = ":/website/" + QLocale::languageToCode(lang);
  return QDir(folder).exists() ? folder : ":/website/en";
}
