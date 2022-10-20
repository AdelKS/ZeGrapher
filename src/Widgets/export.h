#ifndef EXPORT_H
#define EXPORT_H

#include <QWidget>
#include <QTimer>

#include "information.h"

namespace Ui {
class Export;
}

class Export : public QWidget
{
    Q_OBJECT

public:
    explicit Export(QWidget *parent = nullptr);
    ~Export();

signals:
    void exportGraphSignal();

public slots:
    void enableExportButton();
    void getFileName();
    void exportGraph();

protected slots:
    void updateFileNameExtension();

private:
    void makeConnects();

    QString fileName, extension;
    Ui::Export *ui;
    QTimer timer;
};

#endif // EXPORT_H
