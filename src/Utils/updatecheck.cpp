#include "Utils/updatecheck.h"
#include "Utils/versionparser.h"

const QString UpdateCheck::currentVersion = SOFTWARE_VERSION;

UpdateCheck::UpdateCheck(QObject *parent) : QObject(parent)
{
  connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
}

void UpdateCheck::refresh()
{
  QNetworkRequest request(QUrl("https://zegrapher.com/latest_tag"));
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
    qDebug() << "[Update check] return error: " << QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(reply->error());

    reply->deleteLater();
    return;
  }

  QTextStream reader(reply);
  latestVersion = reader.readAll().trimmed();

  qDebug() << "[Update check] current version: " << currentVersion;
  qDebug() << "[Update check] remote version: " << latestVersion;

  if (latestVersion == currentVersion)
  {
    status = UP_TO_DATE;
  }
  else
  {
    const static auto current_version_parsing = parse_version_string(currentVersion);
    auto latest_version_parsing = parse_version_string(latestVersion);

    if (current_version_parsing and latest_version_parsing)
    {
      if (std::ranges::lexicographical_compare(*current_version_parsing, *latest_version_parsing, std::ranges::less{}))
        status = UPDATE_AVAILABLE;
      else status = UP_TO_DATE;
    }
    else status = UPDATE_MAYBE_AVAILABLE;
  }

  emit statusChanged();

  reply->deleteLater();
}
