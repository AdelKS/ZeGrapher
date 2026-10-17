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
