#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QtQmlIntegration/qqmlintegration.h>

class UpdateCheck : public QObject
{
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(Status status READ getStatus NOTIFY statusChanged)
  Q_PROPERTY(QString latestVersion READ getLatestVersion NOTIFY statusChanged)

public:
  explicit UpdateCheck(QObject *parent = nullptr);

  enum Status {IDLE, CHECKING, ERROR, UPDATE_MAYBE_AVAILABLE, UPDATE_AVAILABLE, UP_TO_DATE};
  Q_ENUM(Status);

  Status getStatus() const { return status; }
  QString getLatestVersion() const { return latestVersion; }

public slots:
  void refresh();

protected slots:
  void downloadFinished(QNetworkReply *reply);

signals:
  void statusChanged();

protected:
  QString latestVersion;
  Status status = IDLE;
  QNetworkAccessManager manager;

};
