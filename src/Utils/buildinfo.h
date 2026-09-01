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

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "Utils/releasenotes.h"
#include "Utils/resources.h"
#include "buildconfig.h"

namespace zg {

/// @brief the facts about this build that QML cannot read on its own
///
/// QML reads the version of the app from Qt.application, but Qt gives its own
/// version to C++ only. The license of the bundled font and the release notes
/// are files inside the resources, which QML cannot read either. The site, the
/// GitHub repository and the contact address are the ones that
/// website/build-config/build.yaml names.
class BuildInfo : public QObject
{
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  /// @brief the Qt version the app runs on. Distributions can update Qt
  ///        separately, so read it at run time
  Q_PROPERTY(QString qtVersion READ getQtVersion CONSTANT)

  /// @brief the license of the Latin Modern Math font that the app embeds
  Q_PROPERTY(QString fontLicense READ getFontLicense CONSTANT)

  /// @brief every release in appdata/release-notes.md, newest first, each a
  ///        {tag, span, summary, released} map
  Q_PROPERTY(QVariantList releaseNotes READ getReleaseNotes CONSTANT)

  /// @brief ZeGrapher's website, 'https://zegrapher.com'
  Q_PROPERTY(QString homepage READ getHomepage CONSTANT)

  /// @brief ZeGrapher's GitHub repository, 'https://github.com/AdelKS/ZeGrapher'
  Q_PROPERTY(QString repository READ getRepository CONSTANT)

  /// @brief the author's email address, 'contact@zegrapher.com'
  Q_PROPERTY(QString contact READ getContact CONSTANT)

public:
  explicit BuildInfo(QObject* parent = nullptr): QObject(parent) {}

  QString getQtVersion() const { return QString::fromLatin1(qVersion()); }

  QString getFontLicense() const
  {
    return readTextFile(":/fonts/latinmodern-math-license.txt").trimmed();
  }

  QVariantList getReleaseNotes() const
  {
    return toVariantList(allReleases(readTextFile(notesFile)));
  }

  /// @brief the releases since 'version', which the app shows the first time
  ///        it runs after an update. See releasesSince()
  /// @param version  the last ZeGrapher version the user ran, empty on a first
  ///                 start
  Q_INVOKABLE QVariantList releaseNotesSince(const QString& version) const
  {
    return toVariantList(releasesSince(readTextFile(notesFile), version));
  }

  QString getHomepage() const { return HOMEPAGE_URL; }

  QString getRepository() const { return REPOSITORY_URL; }

  QString getContact() const { return CONTACT_ADDRESS; }

private:
  /// @brief appdata/releases.yaml, which appdata/meson.build embeds
  static inline const QString notesFile = ":/releases/notes.yaml";
};

}
