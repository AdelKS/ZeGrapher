#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QtQml/qqmlregistration.h>

class UpdateCheck : public QObject
{
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(Status status MEMBER status NOTIFY statusChanged)
  Q_PROPERTY(QString currentVersion READ getCurrentVersion)
  Q_PROPERTY(QString latestVersion READ getLatestVersion)

public:
  explicit UpdateCheck(QObject *parent = 0);

  enum Status {IDLE, CHECKING, ERROR, UPDATE_MAYBE_AVAILABLE, UPDATE_AVAILABLE, UP_TO_DATE};
  Q_ENUM(Status);

  Q_INVOKABLE QString getCurrentVersion() const { return currentVersion; }
  Q_INVOKABLE QString getLatestVersion() const { return latestVersion; }

public slots:
  void refresh();

protected slots:
  void downloadFinished(QNetworkReply *reply);

signals:
  void statusChanged();

protected:
  const static QString currentVersion;
  QString latestVersion;
  Status status = IDLE;
  QNetworkAccessManager manager;

};
