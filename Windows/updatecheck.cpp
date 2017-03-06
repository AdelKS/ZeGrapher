#include "Windows/updatecheck.h"

UpdateCheck::UpdateCheck(QWidget *parent) : QDialog(parent)
{
    QHBoxLayout *hLayout = new QHBoxLayout();
    statusLabel = new QLabel();
    statusLabel->setOpenExternalLinks(true);
    hLayout->addWidget(statusLabel);
    setLayout(hLayout);

    setWindowTitle(tr("Update check"));

    timer.setSingleShot(true);
    timer.setInterval(5000);

    silentCheck = true;

    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
    connect(&timer, SIGNAL(timeout()), this, SLOT(timedOut()));
}

void UpdateCheck::timedOut()
{
    statusLabel->setText(tr("Timed out while trying to check for an update. Please check that you're connected to the internet then try again."));
}

void UpdateCheck::silentCheckForUpdate()
{
    silentCheck = true;

    statusLabel->setText(tr("Looking for a possible update, please wait..."));

    QNetworkRequest request(QUrl("http://zegrapher.com/latest"));
    timer.start();
    manager.get(request);
}


void UpdateCheck::checkForUpdate()
{
    silentCheck = false;

    statusLabel->setText(tr("Looking for a possible update, please wait..."));

    QNetworkRequest request(QUrl("http://zegrapher.com/latest"));
    timer.start();
    manager.get(request);

    QDialog::exec();
}

void UpdateCheck::downloadFinished(QNetworkReply *reply)
{
    timer.stop();

    if(reply->error() != QNetworkReply::NoError)
    {
        statusLabel->setText(tr("Failed to lookup for the latest version, please check that you're currently connected to the internet then try again."));
        reply->deleteLater();
        return;
    }

    bool ok = false;
    QTextStream reader(reply);
    double latestVersion = reader.readAll().toDouble(&ok);

    if(!ok)
    {
        statusLabel->setText(tr("Error while reading latest version file... Please try again later."));
    }
    else
    {
        if(latestVersion > SOFTWARE_VERSION)
        {
            statusLabel->setTextFormat(Qt::RichText);
            statusLabel->setText(tr("A new version is available!<br/><br/>To download it, visit <a href=http://zegrapher.com>zegrapher.com</a>"));
            if(silentCheck)
                QDialog::exec();
        }
        else
        {
            statusLabel->setText(tr("You have the latest version."));
        }
    }

    reply->deleteLater();

}

UpdateCheck::~UpdateCheck()
{

}
