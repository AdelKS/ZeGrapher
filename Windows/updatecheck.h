#ifndef UPDATECHECK_H
#define UPDATECHECK_H

#include <QDialog>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QLabel>
#include <QHBoxLayout>
#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "structures.h"


class UpdateCheck : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateCheck(QWidget *parent = 0);
    ~UpdateCheck();


public slots:
    void checkForUpdate();
    void silentCheckForUpdate(); // checks for an update and only notifies if there's one


protected slots:
    void downloadFinished(QNetworkReply *reply);
    void timedOut();


protected:
    QNetworkAccessManager manager;
    QTimer timer;
    QLabel *statusLabel;
    QString manualCheckErrorText;

};

#endif // UPDATECHECK_H
