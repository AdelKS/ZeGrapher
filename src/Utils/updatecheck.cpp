#include "Utils/updatecheck.h"
#include "Utils/versionparser.h"
#include "buildconfig.h"

UpdateCheck::UpdateCheck(QObject *parent) : QObject(parent)
{
  connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
}

void UpdateCheck::refresh()
{
  // website/scripts/build-website.py writes this file at the site root
  QNetworkRequest request(QUrl(HOMEPAGE_URL "/latest_tag"));
  manager.get(request);
  status = CHECKING;
  emit statusChanged();
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

  if (current and latest)
    status = *current < *latest ? UPDATE_AVAILABLE : UP_TO_DATE;
  else
    status = UPDATE_MAYBE_AVAILABLE;

  emit statusChanged();

  reply->deleteLater();
}
