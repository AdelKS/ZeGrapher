#ifndef EXPORT_H
#define EXPORT_H

#include <QWidget>
#include <QTimer>

#include "structures.h"
#include "information.h"

namespace Ui {
class Export;
}

class Export : public QWidget
{
    Q_OBJECT

public:
    explicit Export(Information *information, QWidget *parent = nullptr);
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
    Information *information;
    Ui::Export *ui;
    QTimer timer;
};

#endif // EXPORT_H
