/****************************************************************************
**  Copyright (c) 2016, Adel Kara Slimane <adel.ks@zegrapher.com>
**
**  This file is part of ZeGrapher's source code.
**
**  ZeGrapher is free software: you may copy, redistribute and/or modify it
**  under the terms of the GNU General Public License as published by the
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




#ifndef PRINT_H
#define PRINT_H

#include <QWidget>
#include <QPrintPreviewWidget>
#include "GraphDraw/exportpreview.h"

#define PRINT_SHEET true
#define PRINT_FILE false

namespace Ui {
    class Export;
}

class Export : public QWidget
{
    Q_OBJECT

public:
    explicit Export(Information *info, QWidget *parent = 0);
    ~Export();

public slots:
    void orientationChanged(int);
    void setGraphSize(double H, double W);
    void enablePrintButton();
    void getFileName();
    void exportGraph();
    void setPrintButtonText(bool);

protected slots:
    void exportGroupStateChanged(bool state);

protected:

    Ui::Export *ui;
    QTabWidget *tabWidget;
    ExportPreview *exportPreview;
    QList<QPrinterInfo> printersList;
    QTimer timer;
    QString fileName;
    bool orthonormal;
};

#endif // PRINT_H
