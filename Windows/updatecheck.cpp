#include "Windows/updatecheck.h"

UpdateCheck::UpdateCheck(QWidget *parent) : QDialog(parent)
{
    QHBoxLayout *hLayout = new QHBoxLayout();
    statusLabel = new QLabel();
    statusLabel->setOpenExternalLinks(true);    
    statusLabel->setTextFormat(Qt::RichText);

    hLayout->addWidget(statusLabel);
    setLayout(hLayout);

    setWindowTitle(tr("Update check"));

    timer.setSingleShot(true);
    timer.setInterval(5000);

    manualCheckErrorText = tr("Please check that you are connected to the internet then try again.<br/>"
                              "If the problem persists, you can check manually at <a href='zegrapher.com'>zegrapher.com</a><br/>"
                              "Current version: ") + SOFTWARE_VERSION_STR;

    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
    connect(&timer, SIGNAL(timeout()), this, SLOT(timedOut()));
}

void UpdateCheck::timedOut()
{
    statusLabel->setText(tr("Failed to lookup the latest version.<br/>") + manualCheckErrorText);
}

void UpdateCheck::silentCheckForUpdate()
{
    statusLabel->setText(tr("Looking for a possible update, please wait..."));

    QNetworkRequest request(QUrl("https://zegrapher.com/latest"));
    timer.start();
    manager.get(request);
}


void UpdateCheck::checkForUpdate()
{
    statusLabel->setText(tr("Looking for a possible update, please wait..."));

    QNetworkRequest request(QUrl("https://zegrapher.com/latest"));
    timer.start();
    manager.get(request);

    QDialog::exec();
}

void UpdateCheck::downloadFinished(QNetworkReply *reply)
{
    timer.stop();

    if(reply->error() != QNetworkReply::NoError)
    {
        statusLabel->setText(tr("Failed to lookup the latest version.<br/>") + manualCheckErrorText);
        reply->deleteLater();
        return;
    }

    bool ok = false;
    QTextStream reader(reply);
    double latestVersion = reader.readAll().toDouble(&ok);

    if(!ok)
    {
        statusLabel->setText(tr("Error while reading latest version file...") + manualCheckErrorText);
    }
    else
    {
        if(latestVersion > SOFTWARE_VERSION)
        {
            statusLabel->setText(tr("A new version is available!<br/>"
                                    "To download it, visit <a href=http://zegrapher.com>zegrapher.com</a><br/><br/>"
                                    "<u>Note:</u> You can enable/disable the automatic update check in the settings"));

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
