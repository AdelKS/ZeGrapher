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

#include "Utils/updatecheck.h"
#include "Utils/versionparser.h"
#include "buildconfig.h"
#include "information.h"

#include <QSysInfo>
#include <QUrlQuery>

UpdateCheck::UpdateCheck(QObject *parent) : QObject(parent)
{
  connect(&manager, &QNetworkAccessManager::finished, this, &UpdateCheck::downloadFinished);
}

void UpdateCheck::refresh()
{
  // a removed check leaves this branch, and the URL it holds, out of the binary
  if constexpr (available)
  {
    // 'windows', 'macos', or the Linux distro, such as 'ubuntu'
    // Qt 6.11 strips only double quotes from the os-release ID, and Gentoo
    // writes ID='gentoo'. An ID never contains quotes, so remove them all
    const QString os = QSysInfo::productType().remove(u'\'');
    const QString arch = QSysInfo::currentCpuArchitecture();
    // the language the app shows, which can differ from the system's
    const QString lang = QLocale::languageToCode(QLocale::Language(information->appSettings.language));

    // website/scripts/build-website.py writes this file at the site root. The
    // server ignores the query: Matomo custom dimensions read it from the access
    // log. Nothing in the request tells one install from another
    QUrl url(HOMEPAGE_URL "/latest_tag");
    url.setQuery(QUrlQuery({{"version", SOFTWARE_VERSION}, {"lang", lang}, {"os", os}, {"arch", arch}}));

    QNetworkRequest request(url);
    // without a timeout, a missing reply would keep the status at CHECKING
    request.setTransferTimeout();
    // Qt fills an empty Accept-Language with the system languages
    request.setRawHeader("Accept-Language", "*");
    request.setHeader(QNetworkRequest::UserAgentHeader,
                      QString("ZeGrapher/%1 (%2; %3)").arg(SOFTWARE_VERSION, os, arch));
    manager.get(request);
    status = CHECKING;
    emit statusChanged();
  }
}

void UpdateCheck::downloadFinished(QNetworkReply *reply)
{
  if(reply->error() != QNetworkReply::NoError)
  {
    status = ERROR;
    emit statusChanged();

    reply->deleteLater();
    return;
  }

  QTextStream reader(reply);
  latestVersion = reader.readAll().trimmed();

  // the site writes a tag, 'v4.0.0', and SOFTWARE_VERSION is a version, '4.0.0'.
  // parse_version_string() reads both forms, so compare their parts
  const auto current = build_version();
  const auto latest = parse_version_string(latestVersion);

  // a captive portal answers with its own page instead of the tag
  if (not latest)
  {
    latestVersion.clear();
    status = ERROR;
  }
  else if (current)
    status = *current < *latest ? UPDATE_AVAILABLE : UP_TO_DATE;
  else
    status = UPDATE_MAYBE_AVAILABLE;

  emit statusChanged();

  reply->deleteLater();
}
